#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <dlib/vector_adaptors.hpp>

#include <vector>


BOOST_AUTO_TEST_CASE(insert_into_unordered_vector) {
  dlib::Unordered_vector<int, std::vector> testing;
  testing.emplace_back(0);
  testing.emplace_back(5);
  BOOST_TEST((testing.size() == 2));
}

BOOST_AUTO_TEST_CASE(erase_from_unordered_vector) {
  dlib::Unordered_vector<int, std::vector> testing;
  testing.emplace_back(0);
  testing.emplace_back(1);
  testing.emplace_back(2);
  testing.erase(testing.begin());
  BOOST_TEST((testing.size() == 2));
}