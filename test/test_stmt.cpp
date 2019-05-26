#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <dlib/stmt.hpp>

namespace {
  struct Result_set {
    template<typename T>
    dlib::Result<void> get_column(size_t id, T& returning) noexcept {
      returning = T{};
      return dlib::success;
    }
  };

  struct Driver {
    dlib::Result<void> open(std::string_view location) noexcept {
      return dlib::success;
    }
    dlib::Result<void> close() noexcept {
      return dlib::success;
    }
    dlib::Result<void> begin() noexcept {
      return dlib::success;
    }
    dlib::Result<void> commit() noexcept {
      return dlib::success;
    }
    dlib::Result<void> rollback() noexcept {
      return dlib::success;
    }
    template<typename Cb, typename ...Args>
    dlib::Result<void> execute(std::string_view sql, Cb&& cb, Args const& ... args) noexcept {
      Result_set results;
      return cb(results);
    }
  };

  using Db = dlib::Db<Driver>;
}

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