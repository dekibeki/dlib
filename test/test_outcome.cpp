#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <dlib/outcome.hpp>

namespace {
  dlib::Result<void> success_function() noexcept {
    return dlib::success;
  }

  dlib::Result<void> failure_function() noexcept {
    return dlib::error("test1");
  }

  template<typename F>
  dlib::Result<void> check(F f) noexcept {
    
    DLIB_TRY((f()));

    return dlib::success;
  }
}

BOOST_AUTO_TEST_CASE(outcome_try1) {
  check([]() { return success_function(); });
  check([]() {return failure_function(); });
}