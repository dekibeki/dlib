#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <dlib/postgresql.hpp>
#include "test_db_framework.hpp"

namespace {

  constexpr const char* connection_string = "host=localhost port=5432 dbname=testing user=testing password=testing connect_timeout=10";

}
BOOST_AUTO_TEST_CASE(build_schema) {
  BOOST_ASSERT((!!dlib_tests::test_build_schema<dlib::Postgresql_driver>(connection_string)));
}

BOOST_AUTO_TEST_CASE(multi_statement) {
  BOOST_ASSERT((!!dlib_tests::test_insert_and_select_statement<dlib::Postgresql_driver>(connection_string)));
}