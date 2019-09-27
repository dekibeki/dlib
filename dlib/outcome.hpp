#pragma once

#include <variant>
#include <optional>
#include <type_traits>
#include <system_error>

#include <dlib/error.hpp>

namespace dlib {
  template<typename T>
  class Result {
  public:
    constexpr Result(T res) noexcept :
      val_{ std::move(res) } {

    }
    Result(std::error_code ec) noexcept :
      val_{ Error{ ec.message() } } {

    }
    Result(Error error) noexcept :
      val_{ std::move(error) } {

    }

    template<typename Val, typename = std::enable_if_t<std::is_error_code_enum_v<Val>>>
    Result(Val val) noexcept :
      Result{ std::error_code{std::move(val)} } {

    }

    constexpr bool success() const noexcept {
      return val_.index() == 0;
    }
    constexpr bool failure() const noexcept {
      return !success();
    }
    constexpr T& value() noexcept {
      return std::get<0>(val_);
    }
    constexpr T const& value() const noexcept {
      return std::get<0>(val_);
    }
    Error& error() noexcept {
      return std::get<1>(val_);
    }
    Error const& error() const noexcept {
      return std::get<1>(val_);
    }
    constexpr explicit operator bool() const noexcept {
      return success();
    }
  private:
    std::variant<
      T,
      Error> val_;
  };

  namespace outcome_impl {
    struct Success {};

  }

  template<>
  class Result<void> {
  public:
    constexpr Result(outcome_impl::Success) noexcept :
      val_{ } {

    }

    Result(Error error) noexcept;

    Result(std::error_code ec) noexcept;

    template<typename Val, typename = std::enable_if_t<std::is_error_code_enum_v<Val>>>
    Result(Val val) noexcept :
      Result{ std::error_code{std::move(val)} } {

    }

    bool success() const noexcept;
    bool failure() const noexcept;

    static constexpr void value() noexcept {

    }
    Error& error() noexcept;
    Error const& error() const noexcept;
    explicit operator bool() const noexcept;
  private:
    Error val_;
  };

  constexpr outcome_impl::Success success;

  Error error(std::string reason) noexcept;

  namespace outcome_impl {
    template<typename T>
    struct Is_result_impl :
      std::false_type {

    };

    template<typename T>
    struct Is_result_impl<Result<T>> :
      std::true_type {

    };
  }

  template<typename T>
  constexpr bool is_result = outcome_impl::Is_result_impl<T>::value;
}

#define DLIB_CONCAT_(a, b) a##b

#define DLIB_CONCAT(a, b) DLIB_CONCAT_(a,b)

#define DLIB_TRY1(stmt) \
  do {\
    auto res = (stmt); \
    if(res.failure()) {\
      return ::std::move(res.error()); \
    }\
  } while(false)

#define DLIB_TRY2(var_name, stmt) \
  auto DLIB_CONCAT(var_name,_result_temporary) = (stmt); \
  if(DLIB_CONCAT(var_name,_result_temporary).failure()) {\
    return std::move(DLIB_CONCAT(var_name,_result_temporary).error()); \
  }\
  auto&& var_name = DLIB_CONCAT(var_name,_result_temporary).value()

#define DLIB_EXPAND(a) a

#define DLIB_GET_OVERLOAD(a,b,name,...) name

#define DLIB_TRY(...) DLIB_EXPAND(DLIB_GET_OVERLOAD(__VA_ARGS__, DLIB_TRY2, DLIB_TRY1)(__VA_ARGS__))
