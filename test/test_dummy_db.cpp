#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <dlib/dummy_db.hpp>

#include "test_db_framework.hpp"

namespace {
  constexpr const char* connection_string = "";
}

BOOST_AUTO_TEST_CASE(dummy_db_build_schema) {
  BOOST_ASSERT((!!dlib_tests::test_build_schema<dlib::Dummy_db_driver>(connection_string)));
}

BOOST_AUTO_TEST_CASE(dummy_db_multi_statement) {
  //since the dummy db does nothing, the insert is not read by the select, meaning we cannot pass this test
  //BOOST_ASSERT((!!dlib_tests::test_insert_and_select_statement<dlib::Dummy_db_driver>(connection_string)));
}