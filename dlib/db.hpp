#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <optional>
#include <dlib/arrays.hpp>
#include <dlib/args.hpp>
#include <dlib/cache.hpp>
#include <dlib/outcome.hpp>
#include <dlib/util.hpp>
#include <dlib/finder_interface.hpp>

#include <boost/unordered_map.hpp>

namespace dlib {

  template<typename>
  struct Column_type {};

  template<typename T>
  constexpr Column_type<T> column_type = Column_type<T>();

  enum class Stmt_step {
    Done,
    Data
  };

  using Blob = Array_view<const std::byte>;

  template<typename T>
  using Nullable = std::optional<T>;

  namespace db_impl {
    template<typename Impl>
    using Stmt_impl = typename Impl::Stmt;

    template<typename Impl>
    using Driver_impl = typename Impl::Driver;

    template<typename Driver>
    Result<void> open(Driver&& driver, std::string_view location) noexcept {
      return driver.open(location);
    }

    template<typename Impl>
    Result<void> begin(Driver_impl<Impl>& driver) noexcept {
      return Impl::begin(driver);
    }

    template<typename Impl>
    Result<void> commit(Driver_impl<Impl>& driver) noexcept {
      return Impl::commit(driver);
    }

    template<typename Impl>
    Result<void> rollback(Driver_impl<Impl>& driver) noexcept {
      return Impl::rollback(driver);
    }

    /* Returns Result<Stmt> */
    template<typename Impl>
    Result<Stmt_impl<Impl>> prepare(Driver_impl<Impl>& driver, std::string_view sql) noexcept {
      return Impl::prepare(driver, sql);
    }

    template<typename Impl>
    Result<void> reset(Driver_impl<Impl>& driver, Stmt_impl<Impl>& stmt) noexcept {
      return Impl::reset(driver, stmt);
    }

    template<typename Impl>
    Result<void> finalize(Driver_impl<Impl>& driver, Stmt_impl<Impl>& stmt) noexcept {
      return Impl::finalize(driver, stmt);
    }

    template<typename Impl, typename ...T>
    Result<void> bind(Driver_impl<Impl>& driver, Stmt_impl<Impl>& stmt, int id, T const&... val) noexcept {
      return Impl::bind(driver, stmt, id, val...);
    }

    template<typename Impl, typename ...T>
    Result<void> bind(Driver_impl<Impl>& driver, Stmt_impl<Impl>& stmt, std::string_view name, T const&... val) noexcept {
      return Impl::bind(driver, stmt, name, val...);
    }

    template<typename Impl>
    Result<Stmt_step> step(Driver_impl<Impl>& driver, Stmt_impl<Impl>& stmt) noexcept {
      return Impl::step(driver, stmt);
    }

    template<typename Impl, typename T>
    Result<T> get_column(Driver_impl<Impl>& driver, Stmt_impl<Impl>& stmt, int id, Column_type<T> c_type = column_type<T>) noexcept {
      return Impl::get_column(driver, stmt, id, c_type);
    }

    template<typename Impl>
    constexpr bool thread_safe = Impl::thread_safe;

    template<typename Impl, size_t i = 0, typename Tuple = void>
    Result<void> get_columns_(Driver_impl<Impl>& driver, Stmt_impl<Impl>& stmt, Tuple& tuple) noexcept {
      if constexpr (i == std::tuple_size_v<Tuple>) {
        return success();
      } else {
        DLIB_TRY(col_data, (get_column<Impl>(driver, stmt, i, column_type<std::tuple_element_t<i, Tuple>>)));
        std::get<i>(tuple) = std::move(col_data);
        return get_columns_<Impl, i + 1>(driver, stmt, tuple);
      }
    }

    template<typename Impl, int i = 0>
    constexpr Result<void> bind_args_(Driver_impl<Impl>&, Stmt_impl<Impl>&) {
      return success();
    }

    template<typename Impl, int i = 0, typename First, typename ...Rest>
    Result<void> bind_args_(Driver_impl<Impl>& driver, Stmt_impl<Impl>& stmt, First const& first, Rest const&... rest) {
      DLIB_TRY((bind<Impl>(driver, stmt, i, first)));
      return bind_args_<Impl, i + 1>(driver, stmt, rest...);
    }

    template<typename Impl, typename ...Columns, typename ...Args, typename Callback>
    Result<void> execute_(Driver_impl<Impl>& driver, Stmt_impl<Impl>& stmt, Callback&& callback, Args&&... args) noexcept {
      using Tuple = std::tuple<Columns...>;
      Tuple columns;
      DLIB_TRY((reset<Impl>(driver, stmt)));
      DLIB_TRY((bind_args_<Impl>(driver, stmt, std::forward<Args>(args)...)));
      for (;;) {
        DLIB_TRY(step_res, step<Impl>(driver, stmt));
        switch (step_res) {
        case Stmt_step::Data:
        {
          DLIB_TRY((get_columns_<Impl>(driver, stmt, columns)));
          if constexpr (is_result<decltype(std::apply(callback, std::move(columns)))>) {
            DLIB_TRY((std::apply(callback, std::move(columns))));
          } else {
            std::apply(callback, std::move(columns));
          }
          continue;
        }
        case Stmt_step::Done:
          return success();
        }
      }
    }

    template<typename Pointer>
    using Queries = Cache<std::string, std::string, Pointer_is<Pointer>>;
  }

  template<typename Impl, typename Pointer_>
  class Db_ex final :
    public Get_pointer_from<Pointer_> {
  public:
    using Driver = db_impl::Driver_impl<Impl>;
    using Stmt = db_impl::Stmt_impl<Impl>;
    using Pointer_arg = Pointer_;
    using Pointer_to = Get_pointer_to<Pointer_arg, Db_ex>;
    using Holder = Get_holder<Pointer_arg, Db_ex>;

    using Query_finder = Finder_interface<std::string_view, std::string>;

    template<typename ...Columns, typename ...Args, typename Callback>
    Result<void> execute(std::string_view name, Callback&& callback, Args const&... args) {
      static_assert(std::is_invocable_v<Callback, Columns...>);
      DLIB_TRY(pooled, (get_stmt_(name)));
      return db_impl::execute_<Impl, Columns...>(driver(), *pooled, std::forward<Callback>(callback), args...);
    }

    template<typename Callback>
    Result<void> transaction(Callback&& cb) noexcept {
      DLIB_TRY((db_impl::begin<Impl>(driver())));
      if constexpr (std::is_same_v<void, decltype(cb(*this))>) {
        cb(*this);
        return db_impl::commit<Impl>(driver());
      } else {
        auto res = cb(*this);
        if (!res) {
          DLIB_TRY((db_impl::rollback<Impl>(driver())));
        } else {
          DLIB_TRY((db_impl::commit<Impl>(driver())));
        }
        return dlib::success();
      }
    }

    Driver& driver() noexcept {
      return driver_;
    }
    Driver const& driver() const noexcept {
      return driver_;
    }

    static Result<Db_ex> make(std::string_view location, Query_finder queries) {
      DLIB_TRY(driver, (Impl::open(location)));
      return Db_ex(std::move(driver), std::move(queries));
    }

  private:
    Db_ex(Driver&& driver, Query_finder queries) noexcept :
      driver_(std::move(driver)),
      queries_(std::move(queries)) {

    }

    Result<Stmt*> get_stmt_(std::string_view name) noexcept {
      auto query_optional{ queries_.get(name) };
      auto found = stmts_.find(as_string_view(name), std::hash<std::string_view>(), std::equal_to<>());

      if (!query_optional.has_value()) {
        /* If the query no longer exists in the cache, the stmt shouldn't exist either.
         We then try to finalize and remove it */
        if (found != stmts_.end()) {
          DLIB_TRY((db_impl::finalize<Impl>(driver_, found->second.stmt)));
          stmts_.erase(found);
        }
        return Errors::not_found;
      } else {
        std::string& query = query_optional.value();
        if (found == stmts_.end()) {
          /*If it doesn't exist yet, make it*/
          DLIB_TRY(new_stmt_impl, (db_impl::prepare<Impl>(driver_, query)));
          Stmt_holder new_stmt{ std::move(new_stmt_impl), std::move(query) };
          found = stmts_.emplace(name, std::move(new_stmt)).first;
        } else if (found->second.query != query) {
          /*If we're stale, finalize the old, prepare the new*/
          DLIB_TRY((db_impl::finalize<Impl>(driver_, found->second.stmt)));
          DLIB_TRY(new_stmt_impl, (db_impl::prepare<Impl>(driver_, query)));
          found->second.query = std::move(query);
          found->second.stmt = std::move(new_stmt_impl);
        }
        return &found->second.stmt;
      }
    }

    struct Stmt_holder {
      Stmt stmt;
      std::string query;
    };

    Driver driver_;
    Query_finder queries_;
    boost::unordered_map<std::string, Stmt_holder, std::hash<std::string>, std::equal_to<>> stmts_;
  };

  template<typename Driver, typename ...Args>
  using Db = Db_ex < Driver, 
    Get_arg_defaulted<Pointer_is, Raw_pointer, Args...>>;

  template<typename Returning, typename ...Columns, typename ...Db_args, typename Name, typename ...Args, typename Callback>
  Result<Returning> db_get_cb(
      Db_ex<Db_args...>& db,
      Name&& name,
      Callback&& cb,
      Args&&... args) noexcept {
    std::optional<Returning> returning{ std::nullopt };

    const auto cb = [&cb, &returning](auto... in) noexcept {
      returning = cb(std::move(in)...);
    };

    DLIB_TRY((db.execute<Columns...>(std::forward<Name>(name), cb, std::forward<Args>(args)...)));

    if (returning) {
      return returning.value();
    } else {
      return std::errc::bad_address;
    }
  }

  template<typename Returning, typename ...Columns, typename ...Db_args, typename Name, typename ...Args>
  Result<Returning> db_get(
      Db_ex<Db_args...>& db,
      Name&& name,
      Args&&... args) noexcept {
    const auto cb = [](auto... in) {
      return Returning{ std::move(in)... };
    };

    return db_get_cb<Returning, Columns...>(db, std::forward<Name>(name), cb, std::forward<Args>(args)...);
  }

  template<typename Returning, typename ...Columns, typename ...Db_args, typename Name, typename ...Args, typename Callback>
  Result<std::vector<Returning>> db_get_all_cb(
      Db_ex<Db_args...>& db,
      Name&& name,
      Callback&& cb,
      Args&& ...args) noexcept {
    std::vector<Returning> returning;

    const auto callback = [&cb, &returning](Columns&&... columns) noexcept {
      returning.emplace_back(cb(std::move(columns)...));
    };

    DLIB_TRY((db.execute<Columns...>(std::forward<Name>(name), callback, std::forward<Args>(args)...)));

    return returning;
  }

  template<typename Returning, typename ...Columns, typename ...Db_args, typename Name, typename ...Args>
  Result<std::vector<Returning>> db_get_all(
      Db_ex<Db_args...>& db,
      Name&& name,
      Args&& ...args) noexcept {

    const auto cb = [](auto... in) {
      return Returning{ std::move(in)... };
    }

    return db_get_all_cb<Returning, Columns...>(db, std::forward<Name>(name), cb, std::forward<Args>(args)...);
  }
}