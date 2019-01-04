#include <dlib/util.hpp>

std::string_view dlib::as_string_view(std::string const& string) noexcept {
  return std::string_view(string.data(), string.size());
}