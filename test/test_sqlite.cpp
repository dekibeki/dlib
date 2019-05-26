#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <dlib/sqlite.hpp>
#include "test_db_framework.hpp"

BOOST_AUTO_TEST_CASE(build_schema) {
  BOOST_ASSERT((!!dlib_tests::test_build_schema<dlib::Sqlite_impl>(":memory:")));
}

BOOST_AUTO_TEST_CASE(multi_statement) {
  BOOST_ASSERT((!!dlib_tests::test_multi_statement<dlib::Sqlite_impl>(":memory:")));
}