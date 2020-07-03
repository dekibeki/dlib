#pragma once

#include <cstdint>
#include <variant>
#include <vector>
#include <chrono>
#include <memory>
#include <cmath>

#include <dlib/arrays.hpp>
#include <dlib/args.hpp>

namespace dlib::raft {

  using Term = uint64_t;
  using Id = uint64_t;
  using NodeId = uint64_t;
  using Index = uint64_t;
  using TimeoutToken = uint64_t;

  enum class Result {
    Success,
    Failure
  };

  struct Vote {
    NodeId who;
    Term when;
  };

  struct FollowerInfo {
    NodeId peer;
    Index nextIndex;
    Index matchIndex;
  };

  struct LeaderState {
    std::vector<FollowerInfo> followers;
  };

  struct CandidateState {
    int64_t votesReceived;
  };

  struct FollowerState {

  };

  struct Entry {
    Term term;
    Array_view<std::byte> data;
  };

  struct AppendEntries {
    Term leadersTerm;
    Index leadersPrevLogIndex;
    Term leadersPrevLogTerm;
    Index leadersCommitIndex;
    Array_view<Entry> entries;
  };

  struct AppendEntriesReply {
    Term currentTerm;
    Result success;
  };

  struct RequestVote {
    Term candidatesTerm;
    Index lastLogIndex;
    Term lastLogTerm;
  };

  struct RequestVoteReply {
    Term currentTerm;
    Result voteGranted;
  };

  struct EntryInfo {
    Index index;
    Term term;
  };

  class Raft {
  public:
    Raft() :
      volatileState_{} {

    }

    void timeout(TimeoutToken token) noexcept {
      if (token != timeoutToken_) {
        //old timeout, ignore
        return;
      }
      return std::visit([this](auto&& state) { return this->timeout_(std::forward<decltype(state)>(state)); }, volatileState_);
    }

    void recv(NodeId from, AppendEntries rpc) noexcept {

      Term currentTerm = currentTerm_();

      if (rpc.leadersTerm < currentTerm) { //leader is old
        send_(from, AppendEntriesReply{ currentTerm, Result::Failure });
      } else if (rpc.leadersTerm > currentTerm) { //leader is new
        currentTerm_(rpc.leadersTerm);
        flush_();
        //go to a follower
        goToFollower_();
      }

      Index written = written_();

      if (written < rpc.leadersPrevLogIndex) { //we aren't as up to date as the leader's prev
        return send_(from, AppendEntriesReply{ currentTerm, Result::Failure });
      }

      Term prevTerm = readTerm_(rpc.leadersPrevLogIndex);

      if (prevTerm != rpc.leadersPrevLogTerm) { //prev log term's don't match
        return send_(from, AppendEntriesReply{currentTerm, Result::Failure});
      }

      writeEntries_(rpc.leadersPrevLogIndex + 1, rpc.entries);

      committed_(rpc.leadersCommitIndex);

      flush_();

      send_(from, AppendEntriesReply{ currentTerm, Result::Success });

      tryNewCommitted_(committed_(), rpc.leadersCommitIndex);

      return setFollowerTimeout_();
    }

    void recv(NodeId from, RequestVote rpc) noexcept {

      Term ourTerm = currentTerm_();

      if (rpc.candidatesTerm < ourTerm) {
        return send_(from, RequestVoteReply{ ourTerm, Result::Failure });
      }

      Vote ourVote = votedFor_();

      if (ourVote.when > rpc.candidatesTerm) {
        return send_(from, RequestVoteReply{ ourTerm, Result::Failure });
      }

      if (ourVote.when == rpc.candidatesTerm && ourVote.who != from) {
        return send_(from, RequestVoteReply{ ourTerm, Result::Failure });
      }

      if (ourVote.when == rpc.candidatesTerm && ourVote.who == from) {
        return send_(from, RequestVoteReply{ ourTerm, Result::Success });
      }

      Index ourLastIndex = written_();

      Term ourLastTerm = readTerm_(ourLastIndex);

      if (upToDate_(ourLastIndex, ourLastTerm, rpc.lastLogIndex, rpc.lastLogTerm)) {

        votedFor_(Vote{ from, rpc.candidatesTerm });

        flush_();

        goToFollower_();

        send_(from, RequestVoteReply{ ourTerm, Result::Success });

        return setFollowerTimeout_();
      } else {
        return send_(from, RequestVoteReply{ ourTerm, Result::Failure });
      }
    }

    void recv(NodeId from, AppendEntriesReply rpc) {
      Term currentTerm = currentTerm_();

      if (rpc.currentTerm < currentTerm) {
        //old, ignore
        return;
      }

      if (rpc.currentTerm > currentTerm) {
        //from the future?
        currentTerm_(rpc.currentTerm);
        flush_();
        goToFollower_();
        setFollowerTimeout_();
        return;
      }

      if (!isLeader()) {
        //confusing, just ignore though?
        return;
      }

      LeaderState& state = std::get<LeaderState>(volatileState_);

      const auto fromFollower = std::find_if(
        state.followers.begin(),
        state.followers.end(),
        [from](FollowerInfo checking) { return checking.peer == from;});

      if (fromFollower == state.followers.end()) {
        //from non existent follower?
        return;
      }

      fromFollower->matchIndex = fromFollower->nextIndex;
      ++fromFollower->nextIndex;

      const auto quorumCount = quorumCount_();

      const Index committed = committed_();

      if (fromFollower->matchIndex > committed) {
        const auto commitCount = std::count_if(
          state.followers.begin(),
          state.followers.end(),
          [fromFollower](FollowerInfo checking) {return checking.matchIndex >= fromFollower->matchIndex;});

        if (commitCount >= quorumCount) {
          tryNewCommitted_(committed, fromFollower->matchIndex);
        }
      }
    }

    void recv(NodeId from, RequestVoteReply rpc) {
      Term currentTerm = currentTerm_();
      
      if (rpc.currentTerm < currentTerm) {
        //old, ignore
        return;
      }

      if (rpc.currentTerm > currentTerm) {
        //from the future?
        currentTerm_(rpc.currentTerm);
        flush_();
        goToFollower_();
        setFollowerTimeout_();
        return;
      }

      if (!isCandidate()) {
        //confusing, just ignore though?
        return;
      }

      auto votesFor = ++std::get<CandidateState>(volatileState_).votesReceived;

      if (votesFor > quorumCount_()) {
        goToLeader_(written_(), currentTerm);
        setLeaderTimeout_();
      }
    }

    bool isLeader() const noexcept {
      return std::holds_alternative<LeaderState>(volatileState_);
    }

    bool isCandidate() const noexcept {
      return std::holds_alternative<CandidateState>(volatileState_);
    }

    bool isFollower() const noexcept {
      return std::holds_alternative<FollowerState>(volatileState_);
    }
  private:
    void timeout_(CandidateState& state) noexcept {

      Term currentTerm = currentTerm_();
      Index written = written_();
      Term writtenTerm = readTerm_(written);

      startElection_(currentTerm, written, writtenTerm, state);
      
      setCandidateTimeout_();
    }

    void timeout_(LeaderState const& state) noexcept {
      sendAppendEntries_(state);
      setLeaderTimeout_();
    }

    void timeout_(FollowerState const& state) noexcept {
      CandidateState& newState = goToCandidate_();

      Term currentTerm = currentTerm_();
      Index written = written_();
      Term writtenTerm = readTerm_(written);

      startElection_(currentTerm, written, writtenTerm, newState);

      setCandidateTimeout_();
    }

    static void timeout_(std::monostate) noexcept {
    }

    static bool upToDate_(Index refIndex, Term refTerm, Index checkingIndex, Term checkingTerm) noexcept {
      if (checkingTerm > refTerm) {
        return true;
      } else if (refTerm < checkingTerm) {
        return false;
      } else { //rightTerm == leftTerm
        return checkingIndex >= refIndex;
      }
    }
    
    FollowerState& goToFollower_() noexcept {
      volatileState_ = FollowerState{};
      return std::get<FollowerState>(volatileState_);
    }

    void setFollowerTimeout_() noexcept {
      auto followerTimeout = followerTimeout_();
      setTimeout_(followerTimeout, ++timeoutToken_);
    }

    CandidateState& goToCandidate_() noexcept {
      volatileState_ = CandidateState{ 0 };
      return std::get<CandidateState>(volatileState_);
    }

    void setCandidateTimeout_() noexcept {
      auto candidateTimeout = candidateTimeout_();
      return setTimeout_(candidateTimeout, ++timeoutToken_);
    }

    LeaderState& goToLeader_(Index written, Term currentTerm) noexcept {
      volatileState_ = LeaderState{};
      LeaderState& state = std::get<LeaderState>(volatileState_);

      for (NodeId peer : peers_) {
        state.followers.emplace_back(FollowerInfo{ peer, written, Index{0ULL} });
      }

      sendAppendEntries_(state);
      return state;
    }

    void setLeaderTimeout_() noexcept {
      auto leaderTimeout = leaderTimeout_();
      return setTimeout_(leaderTimeout, ++timeoutToken_);
    }

    void startElection_(Term currentTerm, Index written, Term writtenTerm, CandidateState& state) noexcept {
      ++currentTerm;

      currentTerm_(currentTerm);
      votedFor_(Vote{ me_, currentTerm });
      flush_();

      state.votesReceived = 1;

      RequestVote request{ currentTerm, written, writtenTerm };

      for (NodeId const& peer : peers_) {
        send_(peer, request);
      }
    }

    void sendAppendEntries_(LeaderState const& leaderState) noexcept {
      Index written = written_();
      Term currentTerm = currentTerm_();
      Index commitIndex = committed_();

      for (FollowerInfo follower : leaderState.followers) {
        sendAppendEntries_(written, currentTerm, commitIndex, follower);
      }
    }

    void sendAppendEntries_(Index written, Term currentTerm, Index commitIndex, FollowerInfo to) noexcept {
      Index prevIndex = to.nextIndex - 1;
      Term prevTerm = readTerm_(prevIndex);

      if (prevIndex == written) {
        AppendEntries sending{
          currentTerm,
          prevIndex,
          prevTerm,
          commitIndex,
          nullptr};

        return send_(to.peer, sending);
      } else {
        EntryInfo snapshotInfo = snapshotInfo_();

        if (snapshotInfo.index > prevIndex) {
          //send snapshot
        } else {
          AppendEntries sending{
            currentTerm,
            prevIndex,
            prevTerm,
            commitIndex,
            nullptr};

          std::vector<Entry> entries;
          //1 entry for now
          std::shared_ptr<Array_view<std::byte>> data = readData_(to.nextIndex);
          Term term = readTerm_(to.nextIndex);
          entries.emplace_back(Entry{ term, *data });
          sending.entries = Array_view<Entry>{ entries };
          return send_(to.peer, sending);
        };
      }
    }

    void commitIndex_(Index index) noexcept {
      std::shared_ptr<Array_view<std::byte>> entry = readData_(index);
      apply_(*entry);
    }

    void tryNewCommitted_(Index prevCommitted, Index newCommitted) noexcept {
      if (prevCommitted >= newCommitted) {
        return;
      }
      
      committed_(newCommitted);
      flush_();
      for (Index i = prevCommitted + 1; i <= newCommitted; ++i) {
        commitIndex_(i);
      }
    }

    std::intmax_t quorumCount_() const noexcept {
      return static_cast<std::intmax_t>(std::floor(peers_.size() / 2)) + 1;
    }

    EntryInfo snapshotInfo_();
    std::shared_ptr<Array_view<std::byte>> snapshot_();
    Term currentTerm_();
    void currentTerm_(Term term);
    Term readTerm_(Index index);
    std::shared_ptr<Array_view<std::byte>> readData_(Index index);
    void writeEntries_(Index location, Array_view<Entry> entries);
    Index committed_();
    void committed_(Index entry);
    Index written_();
    Vote votedFor_();
    void votedFor_(Vote who);
    void startVote_(Term newTerm, Index lastLogIndex, Term lastLogTerm);
    void flush_();
    template<typename Rep, typename Period>
    void setTimeout_(std::chrono::duration<Rep,Period> till, TimeoutToken token);

    //placeholder, doesn't have to be milliseconds
    std::chrono::milliseconds followerTimeout_();
    std::chrono::milliseconds candidateTimeout_();
    std::chrono::milliseconds leaderTimeout_();

    void apply_(Array_view<std::byte> data);

    void send_(NodeId who, AppendEntries appendEntries);
    void send_(NodeId who, AppendEntriesReply appendEntriesReply);
    void send_(NodeId who, RequestVote requestVote);
    void send_(NodeId who, RequestVoteReply requestVoteReply);

    std::variant<std::monostate, CandidateState, FollowerState, LeaderState> volatileState_;
    NodeId me_;
    std::vector<NodeId> peers_;
    TimeoutToken timeoutToken_;
  };
}