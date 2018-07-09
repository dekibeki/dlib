#pragma once

#include <type_traits>

namespace dlib::args {
  namespace impl {
    template<typename Type, typename ...Options>
    constexpr bool contains = (false || ... || ::std::is_same_v<std::decay_t<Type>, ::std::decay_t<Options>>);

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

    template<typename Type, typename First, typename ...Rest>
    constexpr auto get(First&& first, Rest&&... rest) noexcept {
      constexpr bool isSame = ::std::is_same_v<Type, std::decay_t<First>>;

      if constexpr (isSame) {
        return std::forward<First>(first);
      } else {
        return get<Type>(std::forward<Rest>(rest)...);
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

  template<typename Type, typename Fallback, typename ...Options>
  constexpr decltype(auto) getDefaulted(Fallback&& fallback, Options&&... options) {

    if constexpr(impl::contains<Type,Options...>) {
      return impl::get<Type>(std::forward<Options>(options)...);
    }  else {
      return std::forward<Fallback>(fallback);
    } 

    return impl::get<Type>(std::forward<Type>(fallback), std::forward<Options>(options)...);
  }

  template<typename Type, typename ...Options>
  constexpr decltype(auto) get(Options&&... options) {
    static_assert(impl::contains<Type,Options...>,
      "The type we are looking for ('Type') was not found in Options...");
    if constexpr(impl::contains<Type, Options...>) { //to make error messages a bit cleaner
      return impl::get<Type>(std::forward<Options>(options)...);
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
  using GetDefaulted = typename impl::GetImpl<Target, Default, Options...>::type;

  template<template<typename> typename Target, typename ...Options>
  using Get = ::std::enable_if_t<impl::GetImpl<Target, void, Options...>::found,
    typename impl::GetImpl<Target, void, Options...>::type>;
}