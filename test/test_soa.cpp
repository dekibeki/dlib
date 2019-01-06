#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <dlib/soa.hpp>

namespace {
  using Testing_soa = ::dlib::Soa<int, float, std::string>;

  Testing_soa generate_testing_soa() {
    Testing_soa returning;
    returning.push(std::numeric_limits<int>::min(), std::numeric_limits<float>::max(), "row1");
    returning.push(std::numeric_limits<int>::max(), 0.0f, "row2");
    returning.push(0, std::numeric_limits<float>::min(), "row3");

    return returning;
  }
}

BOOST_AUTO_TEST_CASE(soa_iterator) {
  Testing_soa soa{ generate_testing_soa() };
  for (auto&& iter : soa) {

  }
}

BOOST_AUTO_TEST_CASE(soa_sort) {
  Testing_soa soa{ generate_testing_soa() };

  std::sort(soa.begin(), soa.end(), [](auto&& v1, auto&& v2) {
    return std::get<0>(v1) < std::get<0>(v2);
    });
}