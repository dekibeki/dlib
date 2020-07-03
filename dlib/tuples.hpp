#pragma once

#include <tuple>
#include <dlib/meta.hpp>

namespace dlib {
  namespace tuple_impl {
    template<std::size_t i, typename Cb, typename Init, typename ...T>
    constexpr auto right_fold_impl(std::tuple<T...> const& tuple, Cb&& cb, Init&& init) {
      if constexpr (i == sizeof...(T)) {
        return init;
      } else {
        return cb(
          std::get<i>(tuple),
          right_fold_impl<i + 1>(tuple, std::forward<Cb>(cb), std::forward<Init>(init)));
      }
    }

    template<std::size_t i, typename Cb, typename ...T>
    constexpr void for_each_impl(std::tuple<T...> const& tuple, Cb&& cb) {
      if constexpr (i == sizeof...(T)) {

      } else {
        cb(std::get<i>(tuple));
        for_each_impl<i + 1>(tuple, std::forward<Cb>(cb));
      }
    }
  }

  template<typename Cb, typename Init, typename ...T>
  constexpr auto right_fold_tuple(std::tuple<T...> const& tuple, Cb&& cb, Init&& init) {
    return tuple_impl::right_fold_impl<0>(tuple, std::forward<Cb>(cb), std::forward<Init>(init));
  }

  template<typename Cb, typename ...T>
  constexpr void for_each_tuple(std::tuple<T...> const& tuple, Cb&& cb) {
    tuple_impl::for_each_impl<0>(tuple, std::forward<Cb>(cb));
  }

  template<typename Cb, typename ...T, std::size_t ...is>
  constexpr auto transform_tuple(std::tuple<T...> const& tuple, Cb&& cb, std::index_sequence<is...> = std::make_index_sequence<sizeof...(T)>) {
    return std::tuple{ cb(std::get<is>(tuple))... };
  }

  namespace tuple_impl {
    template<std::size_t i, std::size_t found, std::size_t target, template<typename> typename Cb, typename ...T>
    constexpr auto get_ith_for_filter(std::tuple<T...> const& tuple) {
      if constexpr (Cb<std::tuple_element_t<i, std::tuple<T...>>>::value) {
        if constexpr (found == target) {
          return std::get<i>(tuple);
        } else {
          return get_ith_for_filter<i + 1, found + 1, target, Cb>(tuple);
        }
      } else {
        return get_ith_for_filter<i + 1, found, target, Cb>(tuple);
      }
    }

    template<template<typename> typename Cb, typename ...T, std::size_t ...is>
    constexpr Filter<Cb, std::tuple<T...>> filter_tuple_impl(std::tuple<T...> const& tuple, std::index_sequence<is...>) {
      return Filter<Cb, std::tuple<T...>>{ get_ith_for_filter<0,0,is,Cb>(tuple)... };
    }
  }

  template<template<typename> typename Cb, typename ...T>
  constexpr Filter<Cb, std::tuple<T...>> filter_tuple(std::tuple<T...> const& tuple) {
    return tuple_impl::filter_tuple_impl<Cb>(tuple,
      std::make_index_sequence<count<Filter<Cb, std::tuple<T...>>>>{});
  }
}