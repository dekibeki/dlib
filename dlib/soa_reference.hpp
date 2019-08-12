#pragma once

#include <tuple>
#include <type_traits>
#include <functional>

#include <dlib/util.hpp>

namespace dlib {
  template<typename ...Ts>
  class Soa_reference {
  public:
    using Reference = std::tuple<std::reference_wrapper<Ts>...>;
    using Value = std::tuple<Ts...>;

    Soa_reference(Ts&... ts) :
      holding_{ ts... } {

    }

    Soa_reference(Soa_reference const&) = delete;
    Soa_reference(Soa_reference&&) = delete;

    Soa_reference& operator=(Soa_reference const& other) {
      for_each_tuples(
        [](auto& left, auto const& right) { left.get() = right.get(); },
        holding_, other.holding_);

      return *this;
    }

    Soa_reference& operator=(Reference const& other) {
      for_each_tuples(
        [](auto& left, auto const& right) { left.get() = right.get(); },
        holding_, other);

      return *this;
    }
    Soa_reference& operator=(Value const& other) {
      for_each_tuples(
        [](auto& left, auto const& right) {left.get() = right;},
        holding_, other);

      return *this;
    }

    Soa_reference& operator=(Soa_reference&& other) {
      for_each_tuples(
        [](auto& left, auto&& right) { left.get() = std::move(right.get());},
        holding_, other.holding_);

      return *this;
    }

    Soa_reference& operator=(Reference&& other) {
      for_each_tuples(
        [](auto& left, auto&& right) {left.get() = std::move(right.get()); },
        holding_, other);

      return *this;
    }

    Soa_reference& operator=(Value&& other) {
      for_each_tuples(
        [](auto& left, auto&& right) {left.get() = std::move(right);},
        holding_, other);

      return *this;
    }

    void swap(Soa_reference& other) {
      for_each_tuples(
        [](auto& left, auto& right) { std::swap(left.get(), right.get());},
        holding_, other.holding_);
    }

    Reference const& get_underlying() const noexcept {
      return holding_;
    }
    constexpr operator Reference&() noexcept {
      return holding_;
    }
    constexpr operator Reference const&() const noexcept {
      return holding_;
    }
    operator Value() const noexcept {
      return std::apply(
        [](auto... refs) { return Value{ refs.get()... };},
        holding_);
    }
  private:
    Reference holding_;
  };

  namespace soa_reference_impl {

    template<typename Searching_for>
    struct Get_checker {
      template<typename T>
      using Check = std::is_same<std::decay_t<T>, std::decay_t<Searching_for>>;
    };
  }

  template<::std::size_t i, typename ...Ts>
  constexpr decltype(auto) get(::dlib::Soa_reference<Ts...> const& t) noexcept {
    return std::get<i>(t.get_underlying()).get();
  }

  template<typename T, typename ...Ts>
  constexpr decltype(auto) get(::dlib::Soa_reference<Ts...> const& t) noexcept {
    return find_if_tuples<soa_reference_impl::Get_checker<T>::template Check>(t);
  }

  template<typename ...Ts>
  void swap(Soa_reference<Ts...>& r1, Soa_reference<Ts...>& r2) noexcept {
    return r1.swap(r2);
  }
}

namespace std {
  template<typename ...Ts>
  class tuple_size<::dlib::Soa_reference<Ts...>> :
    public ::std::integral_constant<::std::size_t, sizeof...(Ts)> {

  };

  template<::std::size_t i, typename ...Ts>
  class tuple_element<i, ::dlib::Soa_reference<Ts...>> {
  public:
    using type = ::std::tuple_element_t<i, ::std::tuple<Ts...>>;
  };
}