#include <dlib/outcome.hpp>

#include <array>
#include <string_view>

//  Errors

namespace {
  constexpr std::string_view default_error_string = "Unknown error code value";

  constexpr std::array error_strings{
    std::string_view{"Success"},
    std::string_view{"The container is empty"},
    std::string_view{"Could not find what was being searched for"},
    std::string_view{"The buffer is smaller then is needed/was expected"}
  };

  struct Dlib_error_category final :
    public std::error_category {
    virtual const char* name() const noexcept override final {
      return "dlib errors";
    }
    virtual std::string message(int condition) const override final {
      if (condition < 0 || condition >= error_strings.size()) {
        return static_cast<std::string>(default_error_string);
      } else {
        return static_cast<std::string>(error_strings[condition]);
      }
    }
  };

  const Dlib_error_category dlib_error_category;
}

std::error_code dlib::make_error_code(Errors val) noexcept {
  return std::error_code{ static_cast<int>(val), dlib_error_category };
}