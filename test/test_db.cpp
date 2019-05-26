#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <dlib/db.hpp>

struct Test_db;

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