#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <dlib/stmt.hpp>

namespace {
  constexpr int initial_counter_val = 3;
  struct Impl {
    struct Stmt {
      int counter = initial_counter_val;
    };

    struct Driver {

    };

    static dlib::Result<Stmt> prepare(Driver&, std::string_view sql) noexcept {
      return Stmt{};
    }
    static dlib::Result<void> reset(Driver&, Stmt& stmt) noexcept {
      stmt.counter = initial_counter_val;
      return dlib::success();
    }
    static dlib::Result<void> finalize(Driver&, Stmt& stmt) noexcept {
      return dlib::success();
    }
    template<typename ...T>
    static dlib::Result<void> bind(Driver&, Stmt const&, T&& ...) noexcept {
      return dlib::success();
    }

    static dlib::Result<dlib::Stmt_step> step(Driver&, Stmt& stmt) noexcept {
      if (stmt.counter == 0) {
        return dlib::Stmt_step::Done;
      } else {
        --stmt.counter;
        return dlib::Stmt_step::Data;
      }
    }

    template<typename T>
    static dlib::Result<T> get_column(Driver&, Stmt const&, int, dlib::Column_type<T>) noexcept {
      return T();
    }

    static dlib::Result<Driver> open(std::string_view location) noexcept {
      return Driver();
    }

    static dlib::Result<void> close(Driver&) noexcept {
      return dlib::success();
    }

    static constexpr bool thread_safe = true;
  };

  using Db = dlib::Db<Impl>;
}

BOOST_AUTO_TEST_CASE(stmt_execute) {
  constexpr dlib::Stmt test_stmt{
    []() {return "hello"; },
    dlib::list<int>,
    dlib::list<float> };

  auto made_res{ dlib::make<Db>("") };
  BOOST_TEST((!!made_res));
  auto&& db = made_res.value();
  {
    auto execute_res = test_stmt.execute(*db, [](int) {}, 1.0f);
    BOOST_TEST((!!execute_res));
  }
}