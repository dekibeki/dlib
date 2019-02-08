#pragma once

#include <variant>
#include <optional>
#include <type_traits>
#include <system_error>

/*

  //OLD outcome2 result, started causing msvc internal compiler errors, so I made a shitty version

#include "outcome_orig.hpp"

namespace dlib {
  namespace outcome = outcome_v2_4d0d18fc;

  template<typename T>
  using Result = outcome::result<T>;
  template<typename T>
  using Outcome = outcome::outcome<T>;

  enum class ResultVal : bool {
    Success = true,
    Failure = false
  };

  template<typename T>
  bool isSuccess(Outcome<T> const& o) {
    return static_cast<bool>(o);
  }

  bool isSuccess(ResultVal res);

  constexpr auto success() noexcept {
    return outcome::success();
  }

  template<typename T>
  constexpr auto success(T&& t) noexcept {
    return outcome::success(std::forward<T>(t));
  }

  template<typename T>
  constexpr auto failure(T&& err) noexcept {
    return outcome::failure(std::forward<T>(t));
  }

  template<typename T>
  struct Is_result {
    static constexpr bool value = false;
  };

  template<typename T>
  struct Is_result<Result<T>> {
    static constexpr bool value = true;
  };

  template<typename T>
  constexpr bool is_result = Is_result<T>::value;
}*/

namespace dlib {
  enum class Result_val : bool {
    success = true,
    failure = false
  };

  enum class Errors : int {
    empty,
    not_found,
    buffer_too_small,
  };

  std::error_code make_error_code(Errors val) noexcept;

  template<typename T>
  class Result {
  public:
    constexpr Result(T res) noexcept :
      val_{ std::move(res) } {

    }
    constexpr Result(std::error_code ec) noexcept :
      val_{ std::move(ec) } {

    }
    template<typename Val, typename = std::enable_if_t<std::is_error_code_enum_v<Val>>>
    constexpr Result(Val val) noexcept :
      val_{ std::error_code{std::move(val)} } {

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
    std::error_code error() const noexcept {
      return std::get<1>(val_);
    }
    constexpr explicit operator bool() const noexcept {
      return success();
    }
    constexpr operator Result_val() const noexcept {
      if (success()) {
        return Result_val::success;
      } else {
        return Result_val::failure;
      }
    }
  private:
    std::variant<
      T,
      std::error_code> val_;
  };

  template<>
  class Result<void> {
  public:
    constexpr Result(std::nullopt_t) noexcept :
      val_{ std::nullopt } {

    }
    template<typename Res>
    constexpr Result(Res&& res) noexcept :
      val_{ std::forward<Res>(res) } {

    }

    constexpr bool success() const noexcept {
      return !val_.has_value();
    }
    constexpr bool failure() const noexcept {
      return !success();
    }
    static constexpr void value() noexcept {

    }
    std::error_code error() const noexcept {
      return val_.value();
    }
    constexpr explicit operator bool() const noexcept {
      return success();
    }
    constexpr operator Result_val() const noexcept {
      if (success()) {
        return Result_val::success;
      } else {
        return Result_val::failure;
      }
    }
  private:
    std::optional<std::error_code> val_;
  };

  constexpr Result<void> success() noexcept {
    return { std::nullopt };
  }

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

namespace std {
  template<>
  struct is_error_code_enum<::dlib::Errors> :
    public ::std::true_type {

  };
}

#define DLIB_CONCAT_(a, b) a##b

#define DLIB_CONCAT(a, b) DLIB_CONCAT_(a,b)



#define DLIB_TRY1(stmt) \
  do {\
    auto res = (stmt);\
    if(res.failure()) {\
      return res.error(); \
    }\
  } while(false)

#define DLIB_TRY2(var_name, stmt) \
  auto DLIB_CONCAT(var_name,_result_temporary) = (stmt); \
  if(DLIB_CONCAT(var_name,_result_temporary).failure()) {\
    return DLIB_CONCAT(var_name,_result_temporary).error(); \
  }\
  auto&& var_name = DLIB_CONCAT(var_name,_result_temporary).value()

#define DLIB_EXPAND(a) a

#define DLIB_GET_OVERLOAD(a,b,name,...) name

#define DLIB_TRY(...) DLIB_EXPAND(DLIB_GET_OVERLOAD(__VA_ARGS__, DLIB_TRY2, DLIB_TRY1)(__VA_ARGS__))
