#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <dlib/db.hpp>

#include <dlib/dummy_db.hpp>

namespace {
  using Db = dlib::Dummy_db;
}

BOOST_AUTO_TEST_CASE(db_states) {
  Db db;
  BOOST_TEST((!db.driver()));
  BOOST_TEST((!!db.open("")));
  BOOST_TEST((!db.open("")));
  BOOST_TEST((!!db.driver()));
  BOOST_TEST((!!db.close()));
  BOOST_TEST((!db.close()));
  BOOST_TEST((!db.driver()));
}

BOOST_AUTO_TEST_CASE(db_execute) {
  Db db;
  BOOST_TEST((!db.execute("", []() {})));
  BOOST_TEST((!!db.open("")));
  BOOST_TEST((!!db.execute("", []() {})));
  BOOST_TEST((!!db.close()));
  BOOST_TEST((!db.execute("", []() {})));
}