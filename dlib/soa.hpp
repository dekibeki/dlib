#pragma once

#include <tuple>
#include <dlib/unordered_vector.hpp>

namespace dlib {
  template<typename ...Members>
  class Soa {
  public:
    template<typename Member>
    Member& get(size_t i) noexcept {
      return get<Member>()[i];
    }
    template<typename Member>
    Member const& get(size_t i) const noexcept {
      return get<Member>()[i];
    }
    template<typename Member>
    auto begin() noexcept {
      return get<Member>().begin();
    }
    template<typename Member>
    auto end() noexcept {
      return get<Member>().end();
    }

    template<typename Member>
    Unordered_vector<Member>& get() noexcept {
      return std::get<std::vector<Member>>(holding_);
    }
    template<typename Member>
    Unordered_vector<Member> const& get() const noexcept {
      return std::get<std::vector<Member>>(holding_);
    }
    void push(Members... members) noexcept {
      (..., get<Members>().emplace_back(std::move(members)));
    }
    void erase(size_t i) noexcept {
      (..., get<Members>().erase(get<Members>().begin() + i));
    }
    auto size() const noexcept {
      return std::get<0>(holding_).size();
    }

    void reserve(size_t i) noexcept {
      (..., get<Members>().reserve(i));
    }
  private:
    std::tuple<Unordered_vector<Members>...> holding_;
  };
}