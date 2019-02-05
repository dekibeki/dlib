#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <dlib/db.hpp>

struct Test_db;

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
    static dlib::Result<void> bind(Driver&, Stmt const&, T&&...) noexcept {
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

    static constexpr bool thread_safe = true;
  };

  using Db = dlib::Db<Impl>;
  using My_cache = dlib::Cache<std::string_view, std::string, dlib::Pointer_is<dlib::Shared_pointer>>;
}

namespace dlib {
  template<>
  struct Finder_default_get<My_cache> {
    using Me = My_cache;

    std::optional<std::string> operator()(Me* ptr, std::string_view key) noexcept {
      auto read = ptr->read(key);
      if (read.has_value()) {
        return *read.value();
      } else {
        return std::nullopt;
      }
    }
  };

  template<>
  struct Finder_default_contains<My_cache> {
    using Me = My_cache;

    bool operator()(Me* ptr, std::string_view key) noexcept {
      return ptr->read(key).has_value();
    }
  };
}

BOOST_AUTO_TEST_CASE(db_dummy_set_and_get) {
  auto cache_res{ dlib::make<My_cache>() };

  BOOST_TEST(!!cache_res);
  auto&& cache{ cache_res.value() };
  Db::Query_finder finder{ cache.get_pointer_to() };
  auto made_res{ dlib::make<dlib::Db<Impl>>("", finder) };
  BOOST_TEST((!!made_res));
  auto&& db = made_res.value(); 
  {
    auto execute_res = db->execute("test1", []() {});
    BOOST_TEST((!execute_res));
  }
  cache->set("test1", "test1---sql");
  {
    auto execute_res = db->execute<int>("test1", [](int) {});
    BOOST_TEST((!!execute_res));
  }
}