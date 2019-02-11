#pragma once

#include <array>
#include <dlib/vector.hpp>
#include <dlib/iterators.hpp>

namespace dlib {
  template<typename Distance>
  class Quaternion {
  public:
    constexpr Quaternion() noexcept :
      components_{ 0,0,0,0 } {

    }
    constexpr Quaternion(Vector<Distance, 3> const& vec) noexcept :
      components_{ 0, vec[0], vec[1], vec[2] } {

    }

    constexpr Distance& operator[](size_t i) noexcept {
      return components_[i];
    }
    constexpr Distance const& operator[](size_t i) const noexcept {
      return components_[i];
    }

    constexpr auto begin() noexcept {
      return components_.begin();
    }
    constexpr auto begin() const noexcept {
      return components_.begin();
    }
    constexpr auto cbegin() const noexcept {
      return components_.cbegin();
    }
    
    constexpr auto end() noexcept {
      return components_.end();
    }
    constexpr auto end() const noexcept {
      return components_.end();
    }
    constexpr auto cend() const noexcept {
      return components_.cend();
    }

    Quaternion& operator+=(Quaternion const& other) noexcept {
      for (auto&[our_component, their_component] : make_soa_range(*this, other)) {
        our_component += their_component;
      }
      return *this;
    }
    Quaternion operator+(Quaternion const& other) const noexcept {
      Quaternion returning{ *this };
      returning += other;
      return returning;
    }
    Quaternion& operator-=(Quaternion const& other) noexcept {
      for (auto&[our_component, their_component] : make_soa_range(*this, other)) {
        our_component -= their_component;
      }
      return *this;
    }
    Quaternion operator-(Quaternion const& other) const noexcept {
      Quaternion returning{ *this };
      returning += other;
      return returning;
    }
    Quaternion& operator*=(Quaternion const& other) noexcept {
      *this = operator*(other);
      return *this;
    }
    Quaternion operator*(Quaternion const& other) const noexcept {
      Quaternion ret;

      std::array<Distance, 4> const& us = components_;

      ret[0] = us[0] * other[0] - us[1] * other[1] - us[2] * other[2] - us[3] * other[3];
      ret[1] = us[0] * other[1] + us[1] * other[0] + us[2] * other[3] - us[3] * other[2];
      ret[2] = us[0] * other[2] - us[1] * other[3] + us[2] * other[0] + us[3] * other[1];
      ret[3] = us[0] * other[3] + us[1] * other[2] - us[2] * other[1] + us[3] * other[0];

      return ret;
    }
    Quaternion& operator/=(Quaternion const& other) noexcept {
      *this = operator/(other);
      return *this;
    }
    Quaternion operator/(Quaternion const& other) const noexcept;
  
    constexpr Distance scalar() const noexcept {
      return components_[0];
    }
    constexpr Vector<Distance, 3> vector() const noexcept {
      return { components_[1], components_[2], components_[3] };
    }
  private:
    std::array<Distance, 4> components_;
  };

  template<typename Distance, typename Angle>
  constexpr Quaternion<Distance> make_rotation(Vector<Distance, 3> axis, Angle theta) noexcept {
    Quaternion<Distance> returning;

    const Distance sin_coeff = sin(theta / 2);

    returning[0] = std::cos(theta / 2);
    returning[1] = axis[0] * sin_coeff;
    returning[2] = axis[1] * sin_coeff;
    returning[3] = axis[2] * sin_coeff;

    return returning;
  }

  template<typename Distance, typename Scalar>
  constexpr Quaternion<Distance> scale(Quaternion<Distance> a, Scalar scalar) {
    Quaternion<Distance> returning;
    for (auto&[a_component, returning_component] : make_soa_range(a, returning)) {
      returning_component = static_cast<Distance>(a_component * scalar);
    }
  }

  template<typename Distance>
  constexpr Distance length_squared(Quaternion<Distance> a) noexcept {
    Distance result{ 0 };
    for (Distance& component : a) {
      result += component * component;
    }
    return result;
  }

  template<typename Distance>
  constexpr Distance length(Quaternion<Distance> a) noexcept {
    return std::sqrt(length_squared(std::move(a)));
  }

  template<typename Distance>
  constexpr Quaternion<Distance> normalize(Quaternion<Distance> a) noexcept {
    return scale(a, 1.0 / length(a));
  }

  template<typename Distance>
  constexpr Quaternion<Distance> inverse(Quaternion<Distance> a) noexcept {
    Quaternion<Distance> returning;

    const Distance length = ::dlib::length(a);

    returning[0] = a[0] / length;
    returning[1] = -a[1] / length;
    returning[2] = -a[2] / length;
    returning[3] = -a[3] / length;

    return returning;
  }
  
  template<typename Distance>
  constexpr Vector<Distance, 3> rotate(Vector<Distance, 3> vec, Quaternion<Distance> rotation) noexcept {
    return (rotation * Quaternion<Distance>{ vec } * inverse(rotation)).vector();
  }

  template<typename Distance, typename Angle>
  constexpr Vector<Distance, 3> rotate(Vector<Distance, 3> vec, Vector<Distance, 3> axis, Angle theta) {
    return rotate(vec, make_rotation(axis, theta));
  }

  template<typename Distance>
  Quaternion<Distance> Quaternion<Distance>::operator/(Quaternion const& other) const noexcept {
    return operator*(inverse(other));
  }
}