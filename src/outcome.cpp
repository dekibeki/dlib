#include <dlib/outcome.hpp>

#include <array>
#include <string_view>

dlib::Result<void>::Result(Error error) noexcept :
  val_{ std::move(error) } {

}

dlib::Result<void>::Result(std::error_code ec) noexcept :
  val_{ ec.message() } {

}

bool dlib::Result<void>::success() const noexcept {
  return !val_;
}

bool dlib::Result<void>::failure() const noexcept {
  return !success();
}

dlib::Error& dlib::Result<void>::error() noexcept {
  return val_;
}

dlib::Error const& dlib::Result<void>::error() const noexcept {
  return val_;
}

dlib::Result<void>::operator bool() const noexcept {
  return success();
}

dlib::Error dlib::error(std::string reason) noexcept {
  return { reason };
}