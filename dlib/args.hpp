#pragma once

#include <type_traits>
#include <dlib/meta.hpp>

namespace dlib {
  namespace args_impl {
    template<template<typename> typename Target, typename Default, typename ...Options>
    struct GetImpl;

    template<template<typename> typename Target, typename Default>
    struct GetImpl<Target, Default> {
      using type = Default;
      static constexpr bool found = false;
    };

    template<template<typename> typename Target, typename Default, typename First, typename ...Rest>
    struct GetImpl<Target, Default, First, Rest...> {
      using type = typename GetImpl<Target, Default, Rest...>::type;
    };

    template<template<typename> typename Target, typename Default, typename Found, typename ...Rest>
    struct GetImpl<Target, Default, Target<Found>, Rest...> {
      using type = Found;
      static constexpr bool found = true;
    };

    template<typename Type, typename Checking>
    constexpr bool is_get = std::is_same_v<Type, std::decay_t<Checking>>;
    template<template<typename> typename Type, typename Checking>
    constexpr bool is_vget = isWrappedBy<Type, std::decay_t<Checking>>;

    template<typename Type, typename First, typename ...Rest>
    constexpr decltype(auto) get(First&& first, Rest&&... rest) noexcept {
      if constexpr (is_get<Type, First>) {
        return std::forward<First>(first);
      } else {
        return get<Type>(std::forward<Rest>(rest)...);
      }
    }

    template<template<typename> typename Type, typename First, typename ...Rest>
    constexpr decltype(auto) vget(First&& first, Rest&&... rest) noexcept {
      if constexpr (is_vget<Type, First>) {
        return std::forward<First>(first);
      } else {
        return vget<Type>(std::forward<Rest>(rest)...);
      }
    }
  }

  /*
  Allows for named arguments kind of like:
    do_stuff(Times{10},Initial{20})
  which is the same as
    do_stuff(Initial{20},Times{10})

  do_stuff will look like:

  template<typename ...Ins>
  auto do_stuff(Ins&&... ins) {
    const auto times = ::dlib::option::get<Times>(std::forward<Ins>(ins)...);
    const auto initial = ::dlib::option::get<Initial>(std::forward<Ins>(ins)...);

    return do_stuff_impl(times,initial);
  }

  the parameters can also be defaulted by using getDefaulted.

  For example, we can use a default value of 5 for initial if no other value is supplied:

  ::dlib::option::getDefaulted<Initial>(Initial{5},std::forward<Ins>(ins)...);
  */

  template<typename Tag = void>
  struct Arg {
    template<typename T>
    struct Holder {
      Holder(T val_) :
        val{ std::move(val_) } {

      }
      using Type = T;
      T val;
    };
  };

  template<typename Type, typename ...Options>
  constexpr bool contains_arg = (false || ... || args_impl::is_get<Type, Options>);

  template<template<typename> typename Type, typename ...Options>
  constexpr bool contains_varg = (false || ... || args_impl::is_vget<Type, Options>);

  template<typename Type, typename Fallback, typename ...Options>
  constexpr decltype(auto) get_arg_defaulted(Fallback&& fallback, Options&&... options) {

    if constexpr(contains_arg<Type,Options...>) {
      return arg_impl::get<Type>(std::forward<Options>(options)...);
    }  else {
      return std::forward<Fallback>(fallback);
    } 
  }

  template<template<typename> typename Type, typename Fallback, typename ...Options>
  constexpr decltype(auto) get_varg_defaulted(Fallback&& fallback, Options&&... options) {

    if constexpr (contains_varg<Type, Options...>) {
      return arg_impl::vget<Type>(std::forward<Options>(options)...);
    } else {
      return std::forward<Fallback>(fallback);
    }
  }

  template<typename Type, typename ...Options>
  constexpr decltype(auto) get_arg(Options&&... options) {
    static_assert(contains_arg<Type,Options...>,
      "The type we are looking for ('Type') was not found in Options...");
    if constexpr(contains_arg<Type, Options...>) { //to make error messages a bit cleaner
      return args_impl::get<Type>(std::forward<Options>(options)...);
    }
  }

  template<template<typename> typename Type, typename ...Options>
  constexpr decltype(auto) get_varg(Options&&... options) {
    static_assert(contains_varg<Type, Options...>,
      "The type we are looking for ('Type') was not found in Options...");
    if constexpr (contains_varg<Type, Options...>) { //to make error messages a bit cleaner
      return args_impl::vget<Type>(std::forward<Options>(options)...);
    }
  }

  /*
  Allows for named template arguments of the kind:
    do_stuff<Time<size_t>,Initial<int>>()
  Which is the same as:
    do_stuff<Initial<int>,Time<size_t>>()

  do_stuff will look like:

  template<typename ...Ins>
  auto do_stuff() {
    using times = Get<Time,Ins...>;
    using initial = Get<Initial,Ins...>;

    return do_stuff_impl<times,initial>();
  }
  */

  template<template<typename> typename Target, typename Default, typename ...Options>
  using Get_arg_defaulted = typename args_impl::GetImpl<Target, Default, Options...>::type;

  template<template<typename> typename Target, typename ...Options>
  using Get_arg = ::std::enable_if_t<args_impl::GetImpl<Target, void, Options...>::found,
    typename args_impl::GetImpl<Target, void, Options...>::type>;
}