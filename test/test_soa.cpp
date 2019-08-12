#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <dlib/soa.hpp>

namespace {
  using Testing_soa = ::dlib::Soa<int, float, std::string>;

  Testing_soa generate_testing_soa() {
    Testing_soa returning;
    returning.push_back(std::numeric_limits<int>::min(), std::numeric_limits<float>::max(), "row1");
    returning.push_back(std::numeric_limits<int>::max(), 0.0f, "row2");
    returning.push_back(0, std::numeric_limits<float>::min(), "row3");

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
    using std::get;
    return get<0>(v1) < get<0>(v2);
    });
}

BOOST_AUTO_TEST_CASE(soa_erase) {
  Testing_soa soa{ generate_testing_soa() };
  soa.erase(soa.begin(), soa.end());
  BOOST_TEST((soa.empty()));
  soa = generate_testing_soa();
  soa.erase(soa.begin() + 1, soa.end());
  BOOST_TEST((soa.size() == 1));
  BOOST_TEST((::dlib::get<std::string>(*soa.begin()) == "row1"));
  soa = generate_testing_soa();
  soa.erase(soa.begin(), soa.end() - 1);
  BOOST_TEST((soa.size() == 1));
  BOOST_TEST((::dlib::get<std::string>(*soa.begin()) == "row3"));
}

BOOST_AUTO_TEST_CASE(soa_swap_references) {
  Testing_soa soa{ generate_testing_soa() };

  swap(*soa.begin(), *(soa.begin() + 1));
}

BOOST_AUTO_TEST_CASE(soa_move_and_copy) {
  Testing_soa soa{ generate_testing_soa() };

  Testing_soa copy{ soa };
  Testing_soa move{ std::move(soa) };

  Testing_soa copy_assign;
  copy_assign = move;
  Testing_soa move_assign;
  move_assign = std::move(move);
}