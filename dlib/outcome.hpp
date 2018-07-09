#include "outcome_orig.hpp"

namespace outcome = outcome_v2_cc218e85;

namespace dlib {
  template<typename T>
  using Outcome = outcome::outcome<T>;

  enum class Result : bool {
    Success = true,
    Failure = false
  };

  template<typename T>
  bool isSuccess(Outcome<T>&& o) {
    return o;
  }

  bool isSuccess(Result res) {
    return res == Result::Success;
  }
}