#pragma once

#include <cstdint>
#include <variant>

#include <dlib/outcome.hpp>
#include <dlib/span.hpp>
#include <dlib/strong_type.hpp>
#include <dlib/args.hpp>

namespace dlib::raft {

  using Term = strongValue::StrongValue<
    uint64_t,
    strongValue::Construct<uint64_t>,
    strongValue::Regular,
    struct TermTag>;
  using Id = strongValue::StrongValue<
    uint64_t,
    strongValue::Construct<uint64_t>,
    struct IdTag>;
  using NodeId = strongValue::StrongValue<
    uint64_t,
    strongValue::Construct<uint64_t>,
    strongValue::Equal::Self<>,
    strongValue::Copy,
    strongValue::NotEqual::Self<>,
    //strongValue::Regular,
    struct NodeIdTag>;
  using Index = strongValue::StrongValue<
    uint64_t,
    strongValue::Construct<uint64_t>,
    strongValue::Regular,
    strongValue::Add::LeftOf<uint64_t>,
    strongValue::PreIncrement::Using<>,
    struct IndexTag>;

  struct Vote {
    template<typename ...Args>
    Vote(Args&&... args_) :
      who{ args::get<NodeId>(std::forward<Args>(args_)...) },
      when{ args::get<Term>(std::forward<Args>(args_)...) } {

    }
    NodeId who;
    Term when;
  };

  struct FollowerInfo {
    Index nextIndex;
    Index matchIndex;
  };

  struct LeaderState {
    std::vector<FollowerInfo> followers;
  };

  struct Entry {
    Term term;
    ByteSpan data;
  };

  struct AppendEntries {
    Term leadersTerm;
    Index leadersPrevLogIndex;
    Term leadersPrevLogTerm;
    Index leadersCommitIndex;
    Span<Entry> entries;
  };

  struct AppendEntriesReply {
    template<typename ...Args>
    AppendEntriesReply(Args&&... args_) :
      currentTerm{ args::get<Term>(std::forward<Args>(args_)...) },
      success{ args::get<Result>(std::forward<Args>(args_)...) } {

    }
    Term currentTerm;
    Result success;
  };

  struct RequestVote {
    Term candidatesTerm;
    Index lastLogIndex;
    Term lastLogTerm;
  };

  struct RequestVoteReply {
    template<typename ...Args>
    RequestVoteReply(Args&&... args_) :
      currentTerm{ args::get<Term>(std::forward<Args>(args_)...) },
      voteGranted{ args::get<Result>(std::forward<Args>(args_)...) } {

    }

    Term currentTerm;
    Result voteGranted;
  };

  class Raft {
  public:
    Raft() :
      volatileState_{} {

    }

    Outcome<void> timeout() noexcept;

    Outcome<void> recv(NodeId from, AppendEntries rpc) noexcept {
      const Outcome<Term> currentTermO = currentTerm_();

      if (!currentTermO) {
        return currentTermO.as_failure();
      }

      const Term currentTerm = std::move(currentTermO.value());

      if (rpc.leadersTerm < currentTerm) { //leader is old
        return send_(from, AppendEntriesReply( currentTerm, Result::Failure ));
      } else if (rpc.leadersTerm > currentTerm) { //leader is new
        if (const auto res = currentTerm_(rpc.leadersTerm); !res) {
          return res.as_failure();
        }
      }

      const Outcome<Index> writtenO = written_();

      if (!writtenO) {
        return writtenO.as_failure();
      }

      const Index written = std::move(writtenO.value());

      if (written < rpc.leadersPrevLogIndex) { //we aren't as up to date as the leader's prev
        return send_(from, AppendEntriesReply( currentTerm, Result::Failure ));
      }

      const Outcome<Term> prevTermO = readTerm_(rpc.leadersPrevLogIndex);

      if (!prevTermO) {
        return prevTermO.as_failure();
      }

      const Term prevTerm = std::move(prevTermO.value());

      if (prevTerm != rpc.leadersPrevLogTerm) { //prev log term's don't match
        return send_(from, AppendEntriesReply( currentTerm, Result::Failure ));
      }

      const Outcome<Index> prevCommittedO = committed_();

      if (!prevCommittedO) {
        return prevCommittedO.as_failure();
      }

      const Index prevCommitted = std::move(prevCommittedO.value());

      if (const auto res = writeEntries_(rpc.leadersPrevLogIndex + 1ULL, rpc.entries); !res) {
        return res.as_failure();
      }

      if (const auto res = committed_(rpc.leadersCommitIndex); !res) {
        return res.as_failure();
      }

      if (const auto res = send_(from, AppendEntriesReply{ currentTerm, Result::Success }); !res) {
        return res.as_failure();
      }

      //from here we've responded, now we apply committed entries

      for (Index i = prevCommitted; i <= rpc.leadersCommitIndex; ++i) {
        auto entryO = readData_(i);
        if (!entryO) {
          return entryO.as_failure();
        }
        const auto entry =  std::move(entryO.value());
        if (const auto res = apply_(*entry); !res) {
          return res.as_failure();
        }
      }

      return outcome::success();
    }

    Outcome<void> recv(NodeId from, RequestVote rpc) noexcept {
      const Outcome<Term> ourTermO = currentTerm_();

      if (!ourTermO) {
        return ourTermO.as_failure();
      }

      const Term ourTerm = std::move(ourTermO.value());

      if (rpc.candidatesTerm < ourTerm) {
        return send_(from, RequestVoteReply( ourTerm, Result::Failure ));
      }

      const Outcome<Vote> ourVoteO = votedFor_();

      if (!ourVoteO) {
        return ourVoteO.as_failure();
      }

      const Vote ourVote = std::move(ourVoteO.value());

      if (ourVote.when > rpc.candidatesTerm) {
        return send_(from, RequestVoteReply( ourTerm, Result::Failure ));
      }

      if (ourVote.when == rpc.candidatesTerm && ourVote.who != from) {
        return send_(from, RequestVoteReply( ourTerm, Result::Failure ));
      }

      if (ourVote.when == rpc.candidatesTerm && ourVote.who == from) {
        return send_(from, RequestVoteReply( ourTerm, Result::Success ));
      }

      const Outcome<Index> ourLastIndexO = written_();

      if (!ourLastIndexO) {
        return ourLastIndexO.as_failure();
      }

      const Index ourLastIndex = std::move(ourLastIndexO.value());

      const Outcome<Term> ourLastTermO = readTerm_(ourLastIndex);

      if (!ourLastTermO) {
        return ourLastTermO.as_failure();
      }

      const Term ourLastTerm = std::move(ourLastTermO.value());

      if (upToDate_(ourLastIndex, ourLastTerm, rpc.lastLogIndex, rpc.lastLogTerm)) {
        if (const auto res = votedFor_(Vote{ from,rpc.candidatesTerm }); !res) {
          return res.as_failure();
        }

        if (const auto res = flush_(); !res) {
          return res.as_failure();
        }

        return send_(from, RequestVoteReply( ourTerm, Result::Success ));
      } else {
        return send_(from, RequestVoteReply( ourTerm, Result::Success ));
      }
    }
  private:
    static bool upToDate_(Index refIndex, Term refTerm, Index checkingIndex, Term checkingTerm) {
      if (checkingTerm > refTerm) {
        return true;
      } else if (refTerm < checkingTerm) {
        return false;
      } else { //rightTerm == leftTerm
        return checkingIndex >= refIndex;
      }
    }
    
    Outcome<Term> currentTerm_();
    Outcome<void> currentTerm_(Term term);
    Outcome<Term> readTerm_(Index index);
    Outcome<std::unique_ptr<ByteSpan>> readData_(Index index);
    Outcome<void> writeEntries_(Index location, Span<Entry> entries);
    Outcome<Index> committed_();
    Outcome<void> committed_(Index entry);
    Outcome<Index> written_();
    Outcome<Vote> votedFor_();
    Outcome<void> votedFor_(Vote who);
    Outcome<void> startVote_(Term newTerm, Index lastLogIndex, Term lastLogTerm);
    Outcome<void> flush_();

    Outcome<void> apply_(ByteSpan data);

    Outcome<void> send_(NodeId who, AppendEntries appendEntries);
    Outcome<void> send_(NodeId who, AppendEntriesReply appendEntriesReply);

    std::variant<std::monostate, LeaderState> volatileState_;
  };
}