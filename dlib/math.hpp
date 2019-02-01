#pragma once

namespace dlib {
  constexpr long double pi = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986;

  template<typename T>
  constexpr T to_radians(T in) noexcept {
    return in / 180 * pi;
  }

  template<typename T>
  constexpr T to_degres(T in) noexcept {
    return in * 180 / pi;
  }

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