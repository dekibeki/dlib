#pragma once

#include <string_view>
#include <string>
#include <vector>
#include <variant>
#include <memory>

namespace dlib {

  class Error_frame {
  public:
    Error_frame(
      std::string reason) noexcept;

    std::string_view reason() const noexcept;

  private:
    std::string reason_;
  };

  class Error {
  public:
    //no error
    constexpr Error() noexcept {
    }

    Error(
      std::string reason) noexcept;

    //returns true if there is an error
    explicit operator bool() const noexcept;

    Error& add(
      std::string reason) noexcept;

    Error clone() const noexcept;

  private:
    using Frames = std::vector<Error_frame>;

    std::unique_ptr<Frames> frames_;
  };

  static_assert(sizeof(Error) == sizeof(void*));
}