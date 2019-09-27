#include <dlib/error.hpp>

dlib::Error_frame::Error_frame(
  std::string reason) noexcept :
  reason_{ std::move(reason) } {

}

std::string_view dlib::Error_frame::reason() const noexcept {
  return reason_;
}

dlib::Error::Error(
  std::string reason) noexcept {
  add(std::move(reason));
}

dlib::Error::operator bool() const noexcept {
  return frames_ != nullptr;
}

dlib::Error& dlib::Error::add(
  std::string reason) noexcept {

  if (frames_ == nullptr) {
    frames_ = std::make_unique<Frames>();
  }

  frames_->emplace_back(std::move(reason));

  return *this;
}

dlib::Error dlib::Error::clone() const noexcept {
  dlib::Error returning;

  if (frames_ != nullptr) {
    returning.frames_ = std::make_unique<Frames>(*frames_);
  }

  return returning;
}