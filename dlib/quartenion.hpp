#pragma once

#include <array>
#include <dlib/vector.hpp>
#include <dlib/iterators.hpp>

namespace dlib {
  template<typename Distance>
  class Quartenion {
  public:
    constexpr Quartenion() noexcept :
      components_{ 0,0,0,0 } {

    }
    constexpr Quartenion(Vector<Distance, 3> const& vec) noexcept :
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

    Quartenion& operator+=(Quartenion const& other) noexcept {
      for (auto&[our_component, their_component] : make_soa_range(*this, other)) {
        our_component += their_component;
      }
      return *this;
    }
    Quartenion operator+(Quartenion const& other) const noexcept {
      Quartenion returning{ *this };
      returning += other;
      return returning;
    }
    Quartenion& operator-=(Quartenion const& other) noexcept {
      for (auto&[our_component, their_component] : make_soa_range(*this, other)) {
        our_component -= their_component;
      }
      return *this;
    }
    Quartenion operator-(Quartenion const& other) const noexcept {
      Quartenion returning{ *this };
      returning += other;
      return returning;
    }
    Quartenion& operator*=(Quartenion const& other) noexcept {
      *this = operator*(other);
      return *this;
    }
    Quartenion operator*(Quartenion const& other) const noexcept {
      Quartenion ret;

      std::array<Distance, 4> const& us = components_;

      ret[0] = us[0] * other[0] - us[1] * other[1] - us[2] * other[2] - us[3] * other[3];
      ret[1] = us[0] * other[1] + us[1] * other[0] + us[2] * other[3] - us[3] * other[2];
      ret[2] = us[0] * other[2] - us[1] * other[3] + us[2] * other[0] + us[3] * other[1];
      ret[3] = us[0] * other[3] + us[1] * other[2] - us[2] * other[1] + us[3] * other[0];

      return ret;
    }
    Quartenion& operator/=(Quartenion const& other) noexcept {
      *this = operator/(other);
      return *this;
    }
    Quartenion operator/(Quartenion const& other) const noexcept;
  
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
  constexpr Quartenion<Distance> make_rotation(Vector<Distance, 3> axis, Angle theta) noexcept {
    Quartenion<Distance> returning;

    const Distance sin_coeff = sin(theta / 2);

    returning[0] = std::cos(theta / 2);
    returning[1] = axis[0] * sin_coeff;
    returning[2] = axis[1] * sin_coeff;
    returning[3] = axis[2] * sin_coeff;

    return returning;
  }

  template<typename Distance, typename Scalar>
  constexpr Quartenion<Distance> scale(Quartenion<Distance> a, Scalar scalar) {
    Quartenion<Distance> returning;
    for (auto&[a_compoment, returning_component] : make_soa_range(a, returning)) {
      returning_component = static_cast<Distance>(a_component * scalar);
    }
  }

  template<typename Distance>
  constexpr Distance length_squared(Quartenion<Distance> a) noexcept {
    Distance result{ 0 };
    for (Distance& component : a) {
      result += component * component;
    }
    return result;
  }

  template<typename Distance>
  constexpr Distance length(Quartenion<Distance> a) noexcept {
    return std::sqrt(length_squared(std::move(a)));
  }

  template<typename Distance>
  constexpr Quartenion<Distance> normalize(Quartenion<Distance> a) noexcept {
    return scale(a, 1.0 / length(a));
  }

  template<typename Distance>
  constexpr Quartenion<Distance> inverse(Quartenion<Distance> a) noexcept {
    Quartenion<Distance> returning;

    const Distance length = ::dlib::length(a);

    returning[0] = a[0] / length;
    returning[1] = -a[1] / length;
    returning[2] = -a[2] / length;
    returning[3] = -a[3] / length;

    return returning;
  }
  
  template<typename Distance>
  constexpr Vector<Distance, 3> rotate(Vector<Distance, 3> vec, Quartenion<Distance> rotation) noexcept {
    return (rotation * Quartenion<Distance>{ vec } * inverse(rotation)).vector();
  }

  template<typename Distance, typename Angle>
  constexpr Vector<Distance, 3> rotate(Vector<Distance, 3> vec, Vector<Distance, 3> axis, Angle theta) {
    return rotate(vec, make_rotation(axis, theta));
  }

  template<typename Distance>
  Quartenion<Distance> Quartenion<Distance>::operator/(Quartenion const& other) const noexcept {
    return operator*(inverse(other));
  }
}