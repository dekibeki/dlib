#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <dlib/arrays.hpp>

BOOST_AUTO_TEST_CASE(array_view_raw_construction) {
  size_t test[5];
  dlib::Array_view view{test};
  for (size_t i = 0; i < view.size(); ++i) {
    view[i] = i;
  }
}

BOOST_AUTO_TEST_CASE(array_view_std_array_construction) {
  std::array<size_t, 5> test;
  dlib::Array_view view{ test };
  for (size_t i = 0; i < view.size(); ++i) {
    view[i] = i;
  }
}

BOOST_AUTO_TEST_CASE(array_view_std_vector_construction) {
  std::vector<size_t> test;
  test.resize(5);
  dlib::Array_view view{ test };
  for (size_t i = 0; i < view.size(); ++i) {
    view[i] = i;
  }
}

BOOST_AUTO_TEST_CASE(array_vew_const_value_type) {
  std::array<const int, 5> test{ 0 };

  dlib::Array_view view{ test };
}
