#pragma once

namespace dlib {
  template<typename T>
  constexpr T vmin(T a, T b) noexcept {
    return a < b ? a : b;
  }

  template<typename T>
  constexpr T vmax(T a, T b) noexcept {
    return a > b ? a : b;
  }

  template<typename T>
  constexpr T vclamp(T min, T val, T max) noexcept {
    return vmax(min, vmin(val, max));
  }
}