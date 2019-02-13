#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <array>
#include <vector>
#include <memory>

#include <dlib/iterators.hpp>

BOOST_AUTO_TEST_CASE(range) {
  std::array v1{ 0, 1, 2, 3, 10 };
  std::array v2{ 1.0f, 2.0f, 5.0f, 10.0f, 20.0f };

  for (auto const&[i1, i2] : dlib::make_soa_range(v1, v2)) {
    BOOST_TEST((i1 < i2));
  }
}

BOOST_AUTO_TEST_CASE(range_with_const) {
  std::array<const int, 5> v1{ 0, 1, 2, 3, 10 };
  std::array v2{ 1.0f, 2.0f, 5.0f, 10.0f, 20.0f };

  for (auto const&[i1, i2] : dlib::make_soa_range(v1, v2)) {
    BOOST_TEST((i1 < i2));
  }
}