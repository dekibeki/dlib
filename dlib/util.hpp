#pragma once

#include <array>
#include <type_traits>
#include <tuple>
#include <string_view>

namespace dlib {

  template<typename Iterator, size_t size>
  bool start_combination(std::array<Iterator, size>& combos, Iterator begin, Iterator end) {
    if constexpr (size == 0) {
      return true;
    } else {
      if (begin == end) {
        return false;
      }
      combos[0] = begin;
      for (size_t i = 1; i < size; ++i) {
        combos[i] = combos[i - 1];
        ++combos[i];
        if (combos[i] == end) {
          return false;
        }
      }

      return true;
    }
  }

  template<typename Iterator, size_t size>
  bool next_combination(std::array<Iterator, size>& combos, Iterator end) {
    for (size_t i = 0; i < size; ++i) {
      ++combos[size - 1 - i];
      if (combos[size - 1 - i] != end) {
        for (size_t j = size - i; j < size; ++j) {
          combos[j] = combos[j - 1];
          ++combos[j];
          if (combos[j] == end) {
            return false;
          }
        }
        return true;
      }
    }
    return false;
  }

  template<size_t size, typename Iterator, typename Functor>
  void for_each_combination(Iterator begin, Iterator end, Functor functor) {
    std::array<Iterator, size> combo;

    for (bool is_valid = start_combination(combo, begin, end);
      is_valid; is_valid = next_combination(combo, end)) {
      functor(combo);
    }
  }

  template<typename Self, typename ...Pack>
  constexpr bool isSelf = std::is_same_v<std::tuple<std::decay_t<Self>>, std::tuple<std::decay_t<Pack>...>>;

  template<bool check, typename Type = void>
  using disable_if_t = std::enable_if_t<!check, Type>;

  template<typename Iterator, typename Functor>
  void for_each_with_index(Iterator begin, Iterator end, Functor&& functor) {
    std::for_each(begin, end, [functor, i = 0](auto&& item) mutable {
      functor(std::forward<decltype(item)>(item), i);
      ++i;
    });
  }

  template<typename Container, typename Functor>
  void for_each_with_index(Container&& container, Functor&& functor) {
    for_each_with_index(container.begin(), container.end(), std::forward<Functor>(functor));
  }

  constexpr std::string_view as_string_view(std::string_view sv) noexcept {
    return sv;
  }

  std::string_view as_string_view(std::string const& string) noexcept;

  template<size_t n>
  constexpr std::string_view as_string_view(const char(&name)[n]) noexcept {
    return std::string_view(name, n - 1);
  }

  namespace util_impl {
    template<size_t on, typename Functor, typename ...Tuples>
    decltype(auto) for_each_tuples_call(Functor&& functor, Tuples&&... tuples) {
      return functor(std::get<on>(tuples)...);
    }

    template<typename Functor, typename ...Tuples, size_t ...i>
    auto for_each_tuples_impl(std::index_sequence<i...>, Functor&& functor, Tuples&&... tuples) {

      constexpr bool create_tuple = (true && ... && !std::is_same_v<decltype(for_each_tuples_call<i>(std::forward<Functor>(functor), std::forward<Tuples>(tuples)...)), void>);

      if constexpr (create_tuple) {
        return std::tuple{
            for_each_tuples_call<i>(std::forward<Functor>(functor), std::forward<Tuples>(tuples)...)... };
      } else {
        (..., for_each_tuples_call<i>(std::forward<Functor>(functor), std::forward<Tuples>(tuples)...));
      }
    }
  }

  template<typename Functor, typename First_tuple, typename...Rest_tuples>
  auto for_each_tuples(Functor&& functor, First_tuple&& first, Rest_tuples&&... rest) {
    constexpr size_t tuple_size = std::tuple_size_v<std::decay_t<First_tuple>>;

    static_assert((... && (tuple_size == std::tuple_size_v<std::decay_t<Rest_tuples>>)),
      "All tuples must be the same length");

    return util_impl::for_each_tuples_impl(
      std::make_index_sequence<tuple_size>{},
      std::forward<Functor>(functor),
      std::forward<First_tuple>(first),
      std::forward<Rest_tuples>(rest)...);
  }

  namespace util_impl {
    template<size_t on, template<typename> typename Check>
    constexpr void find_if_tuples_impl() noexcept {
      static_assert(on == 0, "Could not satisfy Check");
    }

    template<size_t on, template<typename> typename Check, typename First, typename ...Rest>
    constexpr decltype(auto) find_if_tuples_impl(First&& first, Rest&&... rest) noexcept {
      constexpr size_t first_size = std::tuple_size_v<std::decay_t<First>>;

      if constexpr (on < first_size) {
        if constexpr (Check<std::tuple_element_t<on, std::decay_t<First>>>::value) {
          using std::get;
          return get<on>(std::forward<First>(first));
        } else {
          return find_if_tuples_impl<on + 1, Check>(std::forward<First>(first), std::forward<Rest>(rest)...);
        }
      } else {
        return find_if_tuples_impl<0, Check>(std::forward<Rest>(rest)...);
      }
    }
  }

  template<template<typename> typename Check, typename ...Tuples>
  constexpr decltype(auto) find_if_tuples(Tuples&&... tuples) noexcept {
    return util_impl::find_if_tuples_impl<0, Check>(std::forward<Tuples>(tuples)...);
  }

  namespace util_impl {
    template<typename Functor, typename First_end, typename First_iter, typename ...Rest_iter>
    void for_each_impl(Functor&& functor, First_iter&& first_iter, First_end&& first_end, Rest_iter&&... rest_iters) {
      for (; first_iter != first_end; ++first_iter, (..., ++rest_iters)) {
        functor(*first_iter, *rest_iters...);
      }
    }
  }

  template<typename Functor, typename First_container, typename ...Rest>
  void for_each(Functor&& functor, First_container&& first, Rest&&... rest) {
    if constexpr(sizeof...(Rest) > 1) {
      assert((... == rest.size()));
    }
    
    util_impl::for_each_impl(std::forward<Functor>(functor), first.begin(), first.end(), rest.begin()...);
  }
}