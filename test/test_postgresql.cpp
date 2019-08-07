#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <dlib/postgresql.hpp>
#include "test_db_framework.hpp"

namespace {

  constexpr const char* connection_string = "host=localhost port=5432 dbname=testing user=testing password=testing connect_timeout=10";

}
BOOST_AUTO_TEST_CASE(build_schema) {
  BOOST_TEST((!!dlib_tests::test_build_schema<dlib::Postgresql_driver>(connection_string)));
}

BOOST_AUTO_TEST_CASE(multi_statement) {
  BOOST_TEST((!!dlib_tests::test_insert_and_select_statement<dlib::Postgresql_driver>(connection_string)));
}

BOOST_AUTO_TEST_CASE(timestamps) {
  dlib::Postgresql_db db;

  constexpr auto create_table =
    "CREATE TABLE Test("
    "id INTEGER NOT NULL PRIMARY KEY,"
    "data TIMESTAMP NOT NULL);";

  BOOST_TEST((!!db.open(connection_string)));

  BOOST_TEST((!!db.execute(create_table, []() {})));

  constexpr auto insert_into_table =
    "INSERT INTO Test(id,data) VALUES ($1,$2);";

  BOOST_TEST((!!db.execute(insert_into_table, []() {}, 0, std::chrono::system_clock::now())));
  BOOST_TEST((!!db.execute(insert_into_table, []() {}, 1, std::chrono::system_clock::now())));

  constexpr auto select_from_table =
    "SELECT data FROM Test;";

  const auto cb = [](std::chrono::system_clock::time_point t) noexcept {

  };

  BOOST_TEST((!!db.template execute<std::chrono::system_clock::time_point>(select_from_table, cb)));
}

BOOST_AUTO_TEST_CASE(intervals) {
  dlib::Postgresql_db db;

  constexpr auto create_table =
    "CREATE TABLE Test("
    "id INTEGER NOT NULL PRIMARY KEY,"
    "data INTERVAL NOT NULL);";

  BOOST_TEST((!!db.open(connection_string)));

  BOOST_TEST((!!db.execute(create_table, []() {})));

  constexpr auto insert_into_table =
    "INSERT INTO Test(id,data) VALUES ($1,$2);";

  const std::chrono::system_clock::duration inserting{ 5000000000000LL };

  BOOST_TEST((!!db.execute(insert_into_table, []() {}, 0, inserting)));

  const auto select_from_table =
    "SELECT data FROM Test;";

  const auto cb = [](std::chrono::system_clock::duration t) noexcept {

  };

  BOOST_TEST((!!db.template execute<std::chrono::system_clock::duration>(select_from_table, cb)));
}