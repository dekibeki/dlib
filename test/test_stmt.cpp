#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <dlib/stmt.hpp>

#include <dlib/dummy_db.hpp>

using Db = dlib::Dummy_db;

BOOST_AUTO_TEST_CASE(stmt_execute) {
  constexpr dlib::Stmt test_stmt{
    []() {return "hello"; } };
  Db db;
  BOOST_TEST((!!db.open("")));
  BOOST_TEST((!!test_stmt.execute(db, []() {})));
}

BOOST_AUTO_TEST_CASE(stmt_binds_execute) {
  constexpr dlib::Stmt test_stmt{
    []() {return "hello"; },
    dlib::binds<int> };

  Db db;
  BOOST_TEST((!!db.open("")));
  BOOST_TEST((!!test_stmt.execute(db, []() {}, 0)));
}

BOOST_AUTO_TEST_CASE(stmt_columns_execute) {
  constexpr dlib::Stmt test_stmt{
    []() {return "hello"; },
    dlib::columns<int> };

  Db db;
  BOOST_TEST((!!db.open("")));
  BOOST_TEST((!!test_stmt.execute(db, [](int) {})));
}

BOOST_AUTO_TEST_CASE(stmt_binds_columns_execute) {
  constexpr dlib::Stmt test_stmt{
    []() {return "hello"; },
    dlib::columns<int>,
    dlib::binds<int> };

  Db db;
  BOOST_TEST((!!db.open("")));
  BOOST_TEST((!!test_stmt.execute(db, [](int) {}, 0)));
}

BOOST_AUTO_TEST_CASE(stmt_extras_execute) {
  constexpr dlib::Stmt test_stmt{
    []() {return "hello"; },
    "some other crap",
    0 };

  Db db;
  BOOST_TEST((!!db.open("")));
  BOOST_TEST((!!test_stmt.execute(db, []() {})));
}