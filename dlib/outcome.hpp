#pragma once
#include "outcome_orig.hpp"

namespace outcome = outcome_v2_cc218e85;

namespace dlib {
  template<typename T>
  using Result = outcome::result<T>;
  template<typename T>
  using Outcome = outcome::outcome<T>;

  enum class ResultVal : bool {
    Success = true,
    Failure = false
  };

  template<typename T>
  bool isSuccess(Outcome<T> const& o) {
    return static_cast<bool>(o);
  }

  bool isSuccess(ResultVal res);

  constexpr auto success() noexcept {
    return outcome::success();
  }

  template<typename T>
  constexpr auto success(T&& t) noexcept {
    return outcome::success(std::forward<T>(t));
  }

  template<typename T>
  constexpr auto failure(T&& err) noexcept {
    return outcome::failure(std::forward<T>(t));
  }

  template<typename T>
  struct Is_result {
    static constexpr bool value = false;
  };

  template<typename T>
  struct Is_result<Result<T>> {
    static constexpr bool value = true;
  };

  template<typename T>
  constexpr bool is_result = Is_result<T>::value;
}