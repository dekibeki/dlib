#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <optional>
#include <variant>
#include <dlib/arrays.hpp>
#include <dlib/args.hpp>
#include <dlib/cache.hpp>
#include <dlib/outcome.hpp>
#include <dlib/util.hpp>

#include <boost/type_traits/is_assignable.hpp> 
#include <boost/unordered_map.hpp>

namespace dlib {
  using Blob = Array_view<const std::byte>;

  template<typename T>
  using Nullable = std::optional<T>;

  namespace db_impl {
    template<typename Driver, typename String>
    Result<void> open(Driver& driver, String&& location) noexcept {
      return driver.open(std::forward<String>(location));
    }

    template<typename Driver>
    Result<void> close(Driver& driver) noexcept {
      return driver.close();
    }

    template<typename Driver>
    Result<void> begin(Driver& driver) noexcept {
      return driver.begin();
    }

    template<typename Driver>
    Result<void> commit(Driver& driver) noexcept {
      return driver.commit();
    }

    template<typename Driver>
    Result<void> rollback(Driver& driver) noexcept {
      return driver.rollback();
    }

    template<typename Driver, typename String, typename Cb, typename ...Args>
    Result<void> execute(Driver& driver, String&& sql, Cb&& cb, Args const&... args) noexcept {
      return driver.execute(std::forward<String>(sql), std::forward<Cb>(cb), args...);
    }

    template<typename Results, typename T>
    Result<void> get_column(Results& results, size_t id, T& val) noexcept {
      return results.get_column(id, val);
    }

    template<size_t i = 0, typename Results = void, typename Tuple = void>
    Result<void> get_columns_(Results& results, Tuple& tuple) noexcept {
      if constexpr (i == std::tuple_size_v<Tuple>) {
        return success;
      } else {
        DLIB_TRY((get_column(results, i, std::get<i>(tuple))));
        return get_columns_<i + 1>(results, tuple);
      }
    }

    template<typename ...Columns, typename Driver, typename ...Args, typename Callback>
    Result<void> execute_(Driver& driver, std::string_view sql, Callback&& callback, Args const&... args) noexcept {
      using Tuple = std::tuple<Columns...>;
      Tuple columns;
      auto cb = [&columns, &callback](auto& results) noexcept -> Result<void> {
        DLIB_TRY((get_columns_(results, columns)));
        DLIB_TRY((std::apply(callback, columns)));
        return success;
      };

      return execute(driver, sql, cb, args...);
    }

    struct Closed {

    };

    constexpr Closed closed = Closed{};
  }

  template<typename Driver>
  class Db final {
  public:
    Db() :
      state_{ db_impl::closed } {

    }
    Db(Db const&) = delete;
    Db(Db&& other) :
      state_{ std::move(other.state_) } {
      other.state_ = db_impl::closed;
    }

    Db& operator=(Db const&) = delete;
    Db& operator=(Db&& other) noexcept {
      close();
      state_ = std::move(other.state_);
      other.state_ = db_impl::closed;
    }
    ~Db() {
      close();
    }

    Result<void> open(std::string_view location) noexcept {
      if (!std::holds_alternative<db_impl::Closed>(state_)) {
        return Errors::wrong_state;
      }
      Driver driver;
      DLIB_TRY((db_impl::open(driver, location)));
      state_ = std::move(driver);
      return success;
    }

    Result<void> close() noexcept {
      Driver* driver = std::get_if<Driver>(&state_);
      if (driver == nullptr) {
        return Errors::wrong_state;;
      }
      DLIB_TRY((db_impl::close(*driver)));
      state_ = db_impl::closed;
      return success;
    }

    template<typename ...Columns, typename Query, typename ...Args, typename Callback>
    Result<void> execute(Query query, Callback&& callback, Args const& ... args) noexcept {
      return execute<Columns...>(query.query_string(), std::forward<Callback>(callback), args...);
    }

    template<typename ...Columns, typename ...Args, typename Callback>
    Result<void> execute(std::string_view query, Callback&& callback, Args const& ... args) noexcept {
      static_assert(std::is_invocable_v<Callback, Columns...>);
      Driver* driver = std::get_if<Driver>(&state_);
      if (driver == nullptr) {
        return Errors::wrong_state;
      }
      return db_impl::execute_<Columns...>(*driver, query, std::forward<Callback>(callback), args...);
    }

    template<typename ...Columns, typename ...Args, typename Callback>
    Result<void> execute(std::string const& query, Callback&& callback, Args const& ... args) noexcept {
      return execute<Columns...>(std::string_view{ query }, std::forward<Callback>(callback), args...);
    }

    template<typename ...Columns, typename ...Args, typename Callback>
    Result<void> execute(const char* query, Callback&& callback, Args const& ... args) noexcept {
      return execute<Columns...>(std::string_view{ query }, std::forward<Callback>(callback), args...);
    }

    template<typename Callback>
    Result<void> transaction(Callback&& cb) noexcept {
      Driver* driver = std::get_if<Driver>(&state_);
      if (driver == nullptr) {
        return Errors::wrong_state;;
      }
      DLIB_TRY((db_impl::begin(*driver)));
      if constexpr (std::is_same_v<void, decltype(cb(*this))>) {
        cb(*this);
        return db_impl::commit(*driver);
      } else {
        auto res = cb(*this);
        if (!res) {
          DLIB_TRY((db_impl::rollback(*driver)));
        } else {
          DLIB_TRY((db_impl::commit(*driver)));
        }
        return dlib::success;
      }
    }

    Result<Driver*> driver() noexcept {
      Driver* driver = std::get_if<Driver>(&state_);
      if (driver == nullptr) {
        return Errors::wrong_state;
      }
      return driver;
    }

    Result<const Driver*> driver() const noexcept {
      const Driver* driver = std::get_if<Driver>(&state_);
      if (driver == nullptr) {
        return Errors::wrong_state;
      }
      return driver;
    }

  private:
    std::variant<db_impl::Closed, Driver> state_;
  };
}