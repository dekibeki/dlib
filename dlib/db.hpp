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

  struct Null {
  };
  constexpr Null null = Null{};

  template<typename T>
  class Nullable {
  public:
    constexpr Nullable() noexcept :
      datum_{ std::nullopt } {

    }
    constexpr Nullable(T data) noexcept :
      datum_{ std::move(data) } {

    }

    constexpr Nullable(Null) noexcept :
      datum_{ std::nullopt } {

    }

    bool is_null() const noexcept {
      return !datum_.has_value();
    }
    T& data() noexcept {
      return datum_.value();
    }
    T const& data() const noexcept {
      return datum_.value();
    }

  private:
    std::optional<T> datum_;
  };

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
        using Apply_ret = decltype(std::apply(callback, columns));
        if constexpr (is_result<Apply_ret>) {
          DLIB_TRY((std::apply(callback, columns)));
        } else {
          std::apply(callback, columns);
        }
        return success;
      };

      return execute(driver, sql, cb, args...);
    }

    struct Closed {

    };

    constexpr Closed closed = Closed{};
  }

  template<typename ...Result_types>
  struct Stmt_results {};

  template<typename ...Result_types>
  constexpr Stmt_results<Result_types...> stmt_results = {};

  template<typename ...Argument_types>
  struct Stmt_arguments {};

  template<typename ...Argument_types>
  constexpr Stmt_arguments<Argument_types...> stmt_arguments = {};

  /*Colums and Arguments must be two lists of types for the respective things*/
  template<typename Query, typename Results, typename Arguments>
  struct Stmt {

    constexpr Stmt(Query query_) noexcept :
      query{ std::move(query_) } {

    }

    template<typename Arg1>
    constexpr Stmt(Query query_, Arg1) noexcept :
      query{ std::move(query_) } {

    }

    template<typename Arg1, typename Arg2>
    constexpr Stmt(Query query_, Arg1, Arg2) noexcept :
      query{ std::move(query_) } {

    }

    Query query;
  };

  template<typename Query>
  Stmt(Query)->Stmt<Query, Stmt_results<>, Stmt_arguments<>>;

  template<typename Query, typename ...Result_types>
  Stmt(Query, Stmt_results<Result_types...>)->Stmt<Query, Stmt_results<Result_types...>, Stmt_arguments<>>;

  template<typename Query, typename ...Argument_types>
  Stmt(Query, Stmt_arguments<Argument_types...>)->Stmt<Query, Stmt_results<>, Stmt_arguments<Argument_types...>>;

  template<typename Query, typename ...Result_types, typename ...Argument_types>
  Stmt(Query, Stmt_results<Result_types...>, Stmt_arguments<Argument_types...>)->Stmt<Query, Stmt_results<Result_types...>, Stmt_arguments<Argument_types...>>;

  template<typename Query, typename ...Result_types, typename ...Argument_types>
  Stmt(Query, Stmt_arguments<Argument_types...>, Stmt_results<Result_types...>)->Stmt<Query, Stmt_results<Result_types...>, Stmt_arguments<Argument_types...>>;

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
        return error("db is not closed");
      }
      Driver driver;
      DLIB_TRY((db_impl::open(driver, location)));
      state_ = std::move(driver);
      return success;
    }

    Result<void> close() noexcept {
      Driver* driver = std::get_if<Driver>(&state_);
      if (driver == nullptr) {
        return error("db was not in an openned state");
      }
      DLIB_TRY((db_impl::close(*driver)));
      state_ = db_impl::closed;
      return success;
    }

    template<typename ...Columns, typename ...Args, typename Callback>
    Result<void> execute(std::string_view query, Callback&& callback, Args const& ... args) noexcept {
      static_assert(std::is_invocable_v<Callback, Columns...>);
      Driver* driver = std::get_if<Driver>(&state_);
      if (driver == nullptr) {
        return error("db is not open");
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

    template<template<typename...> typename Columns_holder, typename ...Columns, typename Query, template<typename...> typename Args_holder, typename ...Args, typename Callback>
    Result<void> execute(
      Stmt<Query, Columns_holder<Columns...>, Args_holder<Args...>> const& stmt,
      Callback&& callback,
      Args const& ... args) noexcept {
      return execute<Columns...>(stmt.query, std::forward<Callback>(callback), args...);
    }

    template<typename Callback>
    Result<void> transaction(Callback&& cb) noexcept {
      Driver* driver = std::get_if<Driver>(&state_);
      if (driver == nullptr) {
        return error("db is not open");
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
        return res;
      }
    }

    Result<Driver*> driver() noexcept {
      Driver* driver = std::get_if<Driver>(&state_);
      if (driver == nullptr) {
        return error("db is not open");
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