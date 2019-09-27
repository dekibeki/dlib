#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <dlib/error.hpp>

namespace {
  dlib::Error no_error() noexcept {
    return {};
  }

  dlib::Error initial_error(
    const char* reason) noexcept {
    return { reason };
  }

  template<typename F>
  dlib::Error add_error(
    F&& f,
    const char* reason) noexcept {
    return std::move(f().add(reason));
  }
}

BOOST_AUTO_TEST_CASE(error_no_error) {
  BOOST_TEST((!no_error()));
}

BOOST_AUTO_TEST_CASE(error_single_error) {
  BOOST_TEST((!!initial_error("test")));
}

BOOST_AUTO_TEST_CASE(error_add_errors) {
  const auto f = []() {
    return initial_error("initial test");
  };

  BOOST_TEST((!!add_error(f, "second frame")));
}

BOOST_AUTO_TEST_CASE(error_clone) {

  const auto t1 = no_error();

  BOOST_TEST((!t1));
  BOOST_TEST((!t1.clone()));

  const auto t2 = initial_error("t2");

  BOOST_TEST((!!t2));
  BOOST_TEST((!!t2.clone()));
}