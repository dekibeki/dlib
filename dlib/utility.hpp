#pragma once

#include <array>
#include <type_traits>

namespace dlib::utility {
  template<typename T>
  constexpr T vMin(T t1, T t2) {
    if (t1 < t2) {
      return t1;
    } else {
      return t2;
    }
  }

  template<typename T>
  constexpr T vMax(T t1, T t2) {
    if (t1 > t2) {
      return t1;
    } else {
      return t2;
    }
  }

  template<typename T, size_t n, typename Enum>
  constexpr bool hasEnumCases(T(&p)[n], Enum maxValue) {
    return n == static_cast<size_t>(maxValue);
  }

  namespace impl {
    namespace utility {
      template<typename D, typename ...Args>
      using ArrayTypeHelper = std::array<std::conditional_t<std::is_same_v<D, void>,
        std::common_type<Args...>,
        D>, sizeof...(Args)>;
    }
  }

  template<typename D = void, typename ...Args>
  constexpr impl::utility::ArrayTypeHelper<D, Args...> makeArray(Args&&... args) noexcept {
    return impl::utility::ArrayTypeHelper<D, Args...>{std::forward<Args>(args)...};
  }

  namespace impl {
    template<typename ...ListArgs>
    struct ConcatImpl;

    template<typename T, typename ListArg>
    struct ContainsImpl;

    template<template<typename ...> typename Expanding, typename ToExpand>
    struct ExpandImpl;

    template<template<typename, typename> typename Function, typename ListArg>
    struct LeftFoldImpl;

    template<template<typename, typename> typename Function, typename ListArg>
    struct RightFoldImpl;

    template<template<typename> typename Predicate, typename ListArg>
    struct FilterImpl;

    template<template<typename> typename Function, typename ListArg>
    struct TransformImpl;

    template<typename ListArg>
    struct UnwrapImpl;

    template<template<typename ...> typename Wrapper, typename Arg>
    struct IsWrappedByImpl;

    template<typename ListArg>
    struct CountImpl;

    template<auto val>
    struct ToIntegralConstantImpl;

    template<template<typename> typename Predicate, typename ListArg>
    struct ThereExistsImpl;

    template<template<typename> typename Predicate, typename ListArg>
    struct ForAllImpl;

    template<size_t i, typename ListArg>
    struct GetImpl;

    template<template<typename ...> typename NewList, typename ListArg>
    struct ChangeContainerImpl;

    template<typename NewContained, typename ListArg>
    struct ChangeContainedImpl;
  }

  template<typename...>
  struct List {};

  struct Placeholder {};

  template<typename ...Args>
  constexpr List<Args...> list = List<Args...>{};

  template<typename ...ListArgs>
  using Concat = typename impl::ConcatImpl<ListArgs..., List<>>::Value;

  template<typename T, typename ListArg>
  constexpr bool contains = impl::ContainsImpl<T, ListArg>::value;

  template<typename T, typename ...Args>
  constexpr bool containsEx = contains<T, List<Args...>>;

  template<template<typename ...> typename Expanding, typename ListArg>
  using Expand = typename impl::ExpandImpl<Expanding, ListArg>::Value;

  template<template<typename ...> typename Expanding, typename ...Args>
  using ExpandEx = Expand<Expanding, List<Args...>>;

  template<template<typename, typename> typename Function, typename Nil, typename Args>
  using LeftFold = typename impl::LeftFoldImpl<Function, Concat<Args, List<Nil>>>::Value;

  template<template<typename, typename> typename Function, typename Nil, typename ...Args>
  using LeftFoldEx = LeftFold<Function, Nil, List<Args...>>;

  template<typename Functor, typename First>
  constexpr auto leftFold(Functor&& functor, First first) {
    return first;
  }

  template<typename Functor, typename First, typename Second, typename ...Rest>
  constexpr auto leftFold(Functor&& functor, First first, Second second, Rest... rest) {
    return leftFold(std::forward<Functor>(functor), functor(first, second), rest...);
  }

  template<template<typename, typename> typename Function, typename Nil, typename Args>
  using RightFold = typename impl::RightFoldImpl<Function, Concat<Args, List<Nil>>>::Value;

  template<template<typename, typename> typename Function, typename Nil, typename ...Args>
  using RightFoldEx = RightFold<Function, Nil, List<Args...>>;

  template<typename Functor, typename Initial, typename ...Rest>
  constexpr auto rightFold(Functor&& functor, Initial initial, Rest... rest) {
    if constexpr(sizeof...(rest) == 0) {
      return initial;
    } else {
      return functor(initial, rightFold(std::forward<Functor>(functor), rest...));
    }
  }

  template<template<typename,typename> typename Function, typename Nil, typename ListArg>
  using Fold = RightFold<Function, Nil, ListArg>;

  template<template<typename,typename> typename Function, typename Nil, typename ...Args>
  using FoldEx = Fold<Function, Nil, List<Args...>>;

  template<typename Functor, typename ...Args>
  constexpr auto fold(Functor&& functor, Args... args) {
    return rightFold(std::forward<Functor>(functor), args...);
  }

  template<template<typename> typename Predicate, typename ListArg>
  using Filter = typename impl::FilterImpl<Predicate, ListArg>::Value;

  template<template<typename> typename Predicate, typename ...Args>
  using FilterEx = Filter<Predicate, List<Args...>>;

  template<template<typename> typename Function, typename ListArg>
  using Transform = typename impl::TransformImpl<Function, ListArg>::Value;

  template<template<typename> typename Function, typename ...Args>
  using TransformEx = Transform<Function, List<Args...>>;

  template<typename ListArg>
  using Unwrap = typename impl::UnwrapImpl<ListArg>::Value;

  template<template<typename ...> typename Wrapper, typename Arg>
  constexpr bool isWrappedBy = impl::IsWrappedByImpl<Wrapper, Arg>::value;

  template<typename ListArg>
  constexpr size_t count = impl::CountImpl<ListArg>::value;

  template<auto val>
  using ToIntegralConstant = typename impl::ToIntegralConstantImpl<val>::Value;

  template<template<typename> typename Predicate, typename ListArg>
  constexpr bool thereExists = impl::ThereExistsImpl<Predicate, ListArg>::value;

  template<template<typename> typename Predicate, typename ...Args>
  constexpr bool thereExistsEx = thereExists<Predicate, List<Args...>>;

  template<template<typename> typename Predicate, typename ListArg>
  constexpr bool forAll = impl::ForAllImpl<Predicate, ListArg>::value;

  template<template<typename> typename Predicate, typename ...Args>
  constexpr bool forAllEx = forAll<Predicate, List<Args...>>;

  template<size_t i, typename ListArg>
  using Get = typename impl::GetImpl<i, ListArg>::Value;

  template<size_t i, typename ...Args>
  using GetEx = Get<i, List<Args...>>;

  template<typename ListArg>
  using First = Get<0, ListArg>;

  template<template<typename ...> typename NewList, typename ListArg>
  using ChangeContainer = typename impl::ChangeContainerImpl<NewList, ListArg>::Value;

  template<typename OldContained, typename NewContained>
  using ChangeContained = typename impl::ChangeContainedImpl<NewContained, OldContained>::Value;

  template<typename OldContained, typename ...Args>
  using ChangeContainedEx = ChangeContained<OldContained, List<Args...>>;

  namespace impl{
    template<template<typename ...> typename List, typename ...Args>
    struct ConcatImpl<List<Args...>> {
      using Value = List<Args...>;
    };

    template<template<typename ...> typename LList, typename ...Left, template<typename ...> typename RList, typename ...Right, typename ...Rest>
    struct ConcatImpl<LList<Left...>, RList<Right...>, Rest...> {
      using Value = typename ConcatImpl<LList<Left..., Right...>,Rest...>::Value;
    };

    template<typename T, template<typename ...> typename List, typename ...Args>
    struct ContainsImpl<T, List<Args...>> {
      static constexpr bool value = fold([](bool a, bool b) {return a || b;},false,std::is_same_v<T, Args>...);
    };

    template<template<typename ...> typename Expanding, typename ToExpand>
    struct ExpandHelperImpl {
      using Value = List<ToExpand>;
    };

    template<template<typename ...> typename Expanding, typename ...Args>
    struct ExpandHelperImpl<Expanding, Expanding<Args...>> {
      using Value = Expanding<Args...>;
    };

    template<template<typename ...> typename Expanding, typename ToExpand>
    using ExpandHelper = typename ExpandHelperImpl<Expanding, ToExpand>::Value;

    template<template<typename ...> typename Expanding, template<typename ...> typename List, typename ...ToExpand> 
    struct ExpandImpl<Expanding, List<ToExpand...>> {
      using Value = Concat<ExpandHelper<Expanding, ToExpand>...>;
    };

    template<template<typename, typename> typename Function, template<typename ...> typename List, typename First, typename Second, typename Third, typename ...Rest>
    struct LeftFoldImpl<Function, List<First, Second, Third, Rest...>> {
      using Value = typename LeftFoldImpl<Function, List<Function<First, Second>, Third, Rest...>>::Value;
    };

    template<template<typename, typename> typename Function, template<typename ...> typename List, typename First, typename Second>
    struct LeftFoldImpl<Function, List<First, Second>> {
      using Value = Function<First, Second>;
    };

    template<template<typename ...> typename List, typename First, template<typename,typename> typename Function>
    struct LeftFoldImpl<Function, List<First>> {
      using Value = First;
    };

    template<template<typename ...> typename List, typename First, typename Second, typename ...Rest, template<typename,typename> typename Function>
    struct RightFoldImpl<Function, List<First, Second, Rest...>> {
      using Value = Function<First, typename RightFoldImpl<Function, List<Second, Rest...>>::Value>;
    };

    template<template<typename ...> typename List, typename Last, template<typename,typename> typename Function>
    struct RightFoldImpl<Function, List<Last>> {
      using Value = Last;
    };

    template<template<typename> typename Predicate, template<typename ...> typename List, typename ...Args>
    struct FilterImpl<Predicate, List<Args...>> {

      template<typename Arg>
      using Helper_ = std::conditional_t<
        Predicate<Arg>::value,
        List<Arg>,
        List<>>;

      template<typename L, typename R>
      using Concat2 = Concat<L, R>;

      using Value = FoldEx<Concat2, List<>, Helper_<Args>...>;
    };

    template<template<typename> typename Function, template<typename ...> typename List,
      typename ...Args>
      struct TransformImpl<Function, List<Args...>> {
      using Value = List<Function<Args>...>;
    };

    template<template<typename ...> typename List, typename First>
    struct UnwrapImpl<List<First>> {
      using Value = First;
    };

    template<template<typename ...> typename Wrapper, typename Arg>
    struct IsWrappedByImpl {
      static constexpr bool value = false;
    };

    template<template<typename ...> typename Wrapper, typename ...Wrapped>
    struct IsWrappedByImpl<Wrapper, Wrapper<Wrapped...>> {
      static constexpr bool value = true;
    };

    template<template<typename ...> typename List, typename ...Args>
    struct CountImpl<List<Args...>> {
      static constexpr bool value = sizeof...(Args);
    };

    template<auto val>
    struct ToIntegralConstantImpl {
      using Value = std::integral_constant<decltype(val), val>;
    };

    template<template<typename> typename Predicate, template<typename ...> typename List, typename ...Args>
    struct ThereExistsImpl<Predicate, List<Args...>> {
      static constexpr bool value = fold([](bool a, bool b) {return a || b;}, false, Predicate<Args>::value...);
    };

    template<template<typename> typename Predicate, template<typename ...> typename List, typename ...Args>
      struct ForAllImpl<Predicate, List<Args...>> {
      static constexpr bool value = fold([](bool a, bool b) {return a && b;}, true, Predicate<Args>::value...);
    };

    template<size_t i,template<typename ...> typename List, typename First, typename ...Args>
    struct GetImpl<i, List<First, Args...>> {
      using Value = Get<i - 1, List<Args...>>;
    };

    template<template<typename ...> typename List, typename First, typename ...Args>
    struct GetImpl<0, List<First, Args...>> {
      using Value = First;
    };

    template<template<typename ...> typename NewList, template<typename ...> typename OldList, typename ...Args>
    struct ChangeContainerImpl<NewList, OldList<Args...>> {
      using Value = NewList<Args...>;
    };

    template<template<typename ...> typename NewList, typename ...NewContained, template<typename...> typename OldList, typename ...OldContained>
    struct ChangeContainedImpl<NewList<NewContained...>, OldList<OldContained...>> {
      using Value = OldList<NewContained...>;
    };
  }
}