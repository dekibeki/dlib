#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <dlib/pool.hpp>



namespace {
  template<typename T>
  constexpr auto default_constructor = []() {return T();};
}

BOOST_AUTO_TEST_CASE(pool) {
  dlib::Pool<int, dlib::Pointer_is<dlib::Raw_pointer>> pool;
}

BOOST_AUTO_TEST_CASE(pool_finite) {
  dlib::Pool<int, dlib::Pointer_is<dlib::Raw_pointer>> pool;
  BOOST_TEST((!pool.get()));
  pool.give_back(1);
  BOOST_TEST((!!pool.get()));
  BOOST_TEST((!pool.get()));
}

BOOST_AUTO_TEST_CASE(pool_construct) {
  dlib::Pool<int, dlib::Pointer_is<dlib::Raw_pointer>> pool;
  BOOST_TEST((!pool.get()));
  BOOST_TEST((!!pool.get([]() {return 0;})));
}