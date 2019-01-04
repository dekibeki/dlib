#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <dlib/unordered_vector.hpp>



BOOST_AUTO_TEST_CASE(insert_into_unordered_vector) {
  dlib::Unordered_vector<int> testing;
  testing.emplace(0);
  testing.emplace(5);
  testing.insert(6);
  testing.insert(-1);
  BOOST_TEST((testing.size() == 4));
}

BOOST_AUTO_TEST_CASE(erase_from_unordered_vector) {
  dlib::Unordered_vector<int> testing;
  testing.insert(0);
  testing.insert(1);
  testing.insert(2);
  testing.erase(testing.begin());
  BOOST_TEST((testing.size() == 2));
}