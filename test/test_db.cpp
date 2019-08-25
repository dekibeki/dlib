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

BOOST_AUTO_TEST_CASE(db_stmt) {
  {
    dlib::Stmt stmt{ "" };
    static_assert(std::is_same_v<
      decltype(stmt),
      dlib::Stmt<const char*, dlib::Stmt_results<>, dlib::Stmt_arguments<>>>);
  }
  {
    dlib::Stmt stmt{ "", dlib::stmt_results<int> };
    static_assert(std::is_same_v<
      decltype(stmt),
      dlib::Stmt<const char*, dlib::Stmt_results<int>, dlib::Stmt_arguments<>>>);
  }
  {
    dlib::Stmt stmt{ "", dlib::stmt_arguments<int> };
    static_assert(std::is_same_v<
      decltype(stmt),
      dlib::Stmt<const char*, dlib::Stmt_results<>, dlib::Stmt_arguments<int>>>);
  }
  {
    dlib::Stmt stmt{ "", dlib::stmt_results<int>, dlib::stmt_arguments<char> };
    static_assert(std::is_same_v<
      decltype(stmt),
      dlib::Stmt<const char*, dlib::Stmt_results<int>, dlib::Stmt_arguments<char>>>);
  }
  {
    dlib::Stmt stmt{ "", dlib::stmt_arguments<int>, dlib::stmt_results<char> };
    static_assert(std::is_same_v<
      decltype(stmt),
      dlib::Stmt<const char*, dlib::Stmt_results<char>, dlib::Stmt_arguments<int>>>);
  }
}

BOOST_AUTO_TEST_CASE(db_execute_stmt) {
  dlib::Stmt stmt1{ "" };
  dlib::Stmt stmt2{ "", dlib::stmt_results<int> };
  dlib::Stmt stmt3{ "", dlib::stmt_arguments<int> };
  dlib::Stmt stmt4{ "", dlib::stmt_results<int>, dlib::stmt_arguments<char> };
  dlib::Stmt stmt5{ "", dlib::stmt_arguments<int>, dlib::stmt_results<char> };

  Db db;

  BOOST_TEST((!!db.open((""))));
  BOOST_TEST((!!db.execute(stmt1, []() {})));
  BOOST_TEST((!!db.execute(stmt2, [](int) {})));
  BOOST_TEST((!!db.execute(stmt3, []() {}, 0)));
  BOOST_TEST((!!db.execute(stmt4, [](int) {}, 'a')));
  BOOST_TEST((!!db.execute(stmt5, [](char) {}, 0)));
  BOOST_TEST((!!db.close()));
}