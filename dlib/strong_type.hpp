#pragma once

#include <type_traits>
#include <dlib/meta.hpp>

/*

  Used for creating strong types.
  
  All operations including construction are disallowed be default, and must be enabled
  by specifying them in the template args. A tag to make each template a unique type can be given by any type, as these allowed operations are pulled out of the variadic template args.

*/

namespace dlib::strong_type {
  namespace impl {
    /*
      This is used to allow one option to allow multiple operations.
      e.g. <A, B, ExpandTo<C, ExpandTo<D, E>>> is seen as <A, B, C, D, E> when looking
      for allowed operations
    */
    template<typename ...>
    struct ExpandTo;

    /*

    */
    template<typename ...Args>
    using NestedExpandTo = ChangeContainer<ExpandTo, ExpandEx<ExpandTo, Args...>>;

    /* Forward decl of the impl version with no variadic*/
    template<typename Type, typename Options>
    struct Strong_type;

    template<typename T>
    constexpr bool is_valid_impl(void*) {
      return false;
    }
    template<typename T, typename = typename T::Strong_type_tag>
    constexpr bool is_valid_impl(nullptr_t) {
      return true;
    }
  }
  template<typename T>
  constexpr auto is_strong_type = impl::is_valid_impl<std::decay_t<T>>(nullptr);

  /*Declaration of a class to pull type and options out of a strong value*/
  template<typename T, bool is_strong_type = is_strong_type<T>>
  struct Strong_type_info;

  /*Partial spec to pull type and options out of a strong value*/
  template<typename T>
  struct Strong_type_info<T, true> {
    using Type = typename std::decay_t<T>::Type;
    using Options = typename std::decay_t<T>::Options;
  };
  /*If it isn't a strong_type, just return what it is*/
  template<typename T>
  struct Strong_type_info<T, false> {
    using Type = std::decay_t<T>;
    using Options = List<>;
  };

  /* Helper typedef to get type of a strong value*/
  template<typename T>
  using Strong_type_type = typename Strong_type_info<T>::Type;

  /* Helper typedef to get the options of a strong value*/
  template<typename T>
  using Strong_type_options = typename Strong_type_info<T>::Options;

  /*Unwraps a strong value if it is one, otherwise returns t*/
  template<typename T>
  constexpr decltype(auto) unwrap(T&& t) noexcept {
    if constexpr (is_strong_type<T>) {
      return t.get();
    } else {
      return std::forward<T>(t);
    }
  }

  /*
  Gets the unwrapped (the type if its a strong value, otherwise T) type of T
*/
  template<typename T>
  using Unwrap = decltype(unwrap(std::declval<T>()));

  namespace defaults {
    /*Default conversion operator functor*/
    template<typename To>
    struct Convert_to {
      template<typename From, typename = decltype(static_cast<To>(std::declval<From>()))>
      To operator()(From&& t) {
        return static_cast<To>(t);
      }
    };
    /*Default < operator functor*/
    struct Less {
      template<typename L, typename R, typename = decltype(std::declval<L>() < std::declval<R>())>
          constexpr decltype(auto) operator()(L&& l, R&& r) const {
            return l < r;
          }
    };
    /*Default > operator functor*/
    struct Greater {
      template<typename L, typename R, typename = decltype(std::declval<L>() > std::declval<R>())>
                constexpr decltype(auto) operator()(L&& l, R&& r) const {
                  return l > r;
                }
    };
    /*Default <= operator functor*/
    struct Less_equal {
      template<typename L, typename R, typename = decltype(std::declval<L>() <= std::declval<R>())>
      constexpr decltype(auto) operator()(L&& l, R&& r) const {
        return  l <= r;
      }
    };
    /*Default >= operator functor*/
    struct Greater_equal {
      template<typename L, typename R, typename = decltype(std::declval<L>() >= std::declval<R>())>
      constexpr decltype(auto) operator()(L&& l, R&& r) const {
        return l >= r;
      }
    };
    /*Default == operator functor*/
    struct Equal {
      template<typename L, typename R, typename = decltype(std::declval<L>() == std::declval<R>())>
      constexpr decltype(auto) operator()(L&& l, R&& r) const {
        return l == r;
      }
    };
    /*Default != operator functor*/
    struct Not_equal {
      template<typename L, typename R, typename = decltype(std::declval<L>() != std::declval<R>())>
      constexpr decltype(auto) operator()(L&& l, R&& r) const {
        return l != r;
      }
    };
    /* Default = operator functor*/
    struct Assign {
      template<typename L, typename R, typename = decltype(std::declval<L>() = std::declval<R>())>
      constexpr decltype(auto) operator()(L&&l, R&& r) const {
        return l = r;
      }
    };
    /*Default + operator functor*/
    struct Add {
      template<typename L, typename R, typename = decltype(std::declval<L>() + std::declval<R>())>
      constexpr decltype(auto) operator()(L&& l, R&& r) const {
        return l + r;
      }
    };
    /*Default += operator functor*/
    struct Add_assign {
      template<typename L, typename R, typename = decltype(std::declval<L>() += std::declval<R>())>
      constexpr decltype(auto) operator()(L&&l, R&& r) const {
        return l += r;
      }
    };
    /*Default - operator functor*/
    struct Sub {
      template<typename L, typename R, typename = decltype(std::declval<L>() - std::declval<R>())>
      constexpr decltype(auto) operator()(L&& l, R&& r) const {
        return l - r;
      }
    };
    /*Default -= operator functor*/
    struct Sub_assign {
      template<typename L, typename R, typename = decltype(std::declval<L>() -= std::declval<R>())>
      constexpr decltype(auto) operator()(L&&l, R&& r) const {
        return l -= r;
      }
    };
    /*Default * operator functor*/
    struct Mult {
      template<typename L, typename R, typename = decltype(std::declval<L>() * std::declval<R>())>
      constexpr decltype(auto) operator()(L&& l, R&& r) const {
        return l * r;
      }
    };
    /*Default *= operator functor*/
    struct Mult_assign {
      template<typename L, typename R, typename = decltype(std::declval<L>() *= std::declval<R>())>
      constexpr decltype(auto) operator()(L&&l, R&& r) const {
        return l *= r;
      }
    };
    /*Default / operator functor*/
    struct Div {
      template<typename L, typename R, typename = decltype(std::declval<L>() / std::declval<R>())>
      constexpr decltype(auto) operator()(L&& l, R&& r) const {
        return l / r;
      }
    };
    /*Default /= operator functor*/
    struct Div_assign {
      template<typename L, typename R, typename = decltype(std::declval<L>() /= std::declval<R>())>
      constexpr decltype(auto) operator()(L&&l, R&& r) const {
        return l /= r;
      }
    };
    /*Default % operator functor*/
    struct Mod {
      template<typename L, typename R, typename = decltype(std::declval<L>() % std::declval<R>())>
      constexpr decltype(auto) operator()(L&& l, R&& r) const {
        return l % r;
      }
    };
    /*Default %= operator functor*/
    struct Mod_assign {
      template<typename L, typename R, typename = decltype(std::declval<L>() %= std::declval<R>())>
      constexpr decltype(auto) operator()(L&&l, R&& r) const {
        return l %= r;
      }
    };
    /*Default & operator functor*/
    struct Binary_and {
      template<typename L, typename R, typename = decltype(std::declval<L>() & std::declval<R>())>
      constexpr decltype(auto) operator()(L&& l, R&& r) const {
        return l & r;
      }
    };
    /*Default &= operator functor*/
    struct Binary_and_assign {
      template<typename L, typename R, typename = decltype(std::declval<L>() &= std::declval<R>())>
      constexpr decltype(auto) operator()(L&&l, R&& r) const {
        return l &= r;
      }
    };
    /*Default | operator functor*/
    struct Binary_or {
      template<typename L, typename R, typename = decltype(std::declval<L>() | std::declval<R>())>
      constexpr decltype(auto) operator()(L&& l, R&& r) const {
        return l | r;
      }
    };
    /*Default |= operator functor*/
    struct Binary_or_assign {
      template<typename L, typename R, typename = decltype(std::declval<L>() |= std::declval<R>())>
      constexpr decltype(auto) operator()(L&&l, R&& r) const {
        return l |= r;
      }
    };
    /*Default ^ operator functor*/
    struct Xor {
      template<typename L, typename R, typename = decltype(std::declval<L>() ^ std::declval<R>())>
      constexpr decltype(auto) operator()(L&& l, R&& r) const {
        return l ^ r;
      }
    };
    /*Default ^= operator functor*/
    struct Xor_assign {
      template<typename L, typename R, typename = decltype(std::declval<L>() ^= std::declval<R>())>
      constexpr decltype(auto) operator()(L&&l, R&& r) const {
        return l ^= r;
      }
    };
    /*Default << operator functor*/
    struct Left_shift {
      template<typename L, typename R, typename = decltype(std::declval<L>() << std::declval<R>())>
      constexpr decltype(auto) operator()(L&& l, R&& r) const {
        return l << r;
      }
    };
    /*Default <<= operator functor*/
    struct Left_shift_assign {
      template<typename L, typename R, typename = decltype(std::declval<L>() <<= std::declval<R>())>
      constexpr decltype(auto) operator()(L&&l, R&& r) const {
        return l <<= r;
      }
    };
    /*Default >> operator functor*/
    struct Right_shift {
      template<typename L, typename R, typename = decltype(std::declval<L>() >> std::declval<R>())>
      constexpr decltype(auto) operator()(L&& l, R&& r) const {
        return l >> r;
      }
    };
    /*Default >>= operator functor*/
    struct Right_shift_assign {
      template<typename L, typename R, typename = decltype(std::declval<L>() >>= std::declval<R>())>
      constexpr decltype(auto) operator()(L&&l, R&& r) const {
        return l >>= r;
      }
    };
    /*Default ~ operator functor*/
    struct Binary_not {
      template<typename L, typename = decltype(~std::declval<L>())>
      constexpr decltype(auto) operator()(L&&l) const {
        return ~l;
      }
    };
    /*Default unary + (e.g. +strongType) operator functor*/
    struct Unary_plus {
      template<typename L, typename = decltype(+std::declval<L>())>
      constexpr decltype(auto) operator()(L&&l) const {
        return +l;
      }
    };
    /*Default unary - (e.g. -strongType) operator functor*/
    struct Unary_minus {
      template<typename L, typename = decltype(-std::declval<L>())>
      constexpr decltype(auto) operator()(L&&l) const {
        return -l;
      }
    };
    /*Default [] operator functor*/
    struct Subscript {
      template<typename L, typename R, typename = decltype(std::declval<L>()[std::declval<R>()])>
      constexpr decltype(auto) operator()(L&& l, R && r) const {
        return l[r];
      }
    };
    /*Default unary * / indirection (e.g. *strongType) operator functor */
    struct Indirection {
      template<typename L, typename = decltype(*std::declval<L>())>
      constexpr decltype(auto) operator()(L&& l) const {
        return *l;
      }
    };
    /*Default pre ++ (e.g. ++strongType) operator functor*/
    struct Pre_increment {
      template<typename L, typename = decltype(++std::declval<L>())>
      constexpr decltype(auto) operator()(L&&l) const {
        return ++l;
      }
    };
    /*Default pre -- (e.g. --strongType) operator functor*/
    struct Pre_decrement {
      template<typename L, typename = decltype(--std::declval<L>())>
      constexpr decltype(auto) operator()(L&&l) const {
        return --l;
      }
    };
    /*Default post ++ (e.g. strongType++) operator functor*/
    struct Post_increment {
      template<typename L, typename = decltype(std::declval<L>()++)>
      constexpr decltype(auto) operator()(L&&l) const {
        return l++;
      }
    };
    /*Default post -- (e.g. strongType--) operator functor*/
    struct Post_decrement {
      template<typename L, typename = decltype(std::declval<L>()--)>
      constexpr decltype(auto) operator()(L&&l) const {
        return l--;
      }
    };
    /*Default ! operator functor*/
    struct Logical_not {
      template<typename L, typename = decltype(!std::declval<L>())>
      constexpr decltype(auto) operator()(L&& l) const {
        return !l;
      }
    };
    /*Default && operator functor*/
    struct Logical_and {
      template<typename L, typename R, typename = decltype(std::declval<L>() && std::declval<R>())>
      constexpr decltype(auto) operator()(L&& l, R&& r) const {
        return l && r;
      }
    };
    /*Default || operator functor*/
    struct Logical_or {
      template<typename L, typename R, typename = decltype(std::declval<L>() || std::declval<R>())>
      constexpr decltype(auto) operator()(L&& l, R&& r) const {
        return l || r;
      }
    };
    /*Default & operator functor*/
    struct Address_of {
      template<typename L, typename = decltype(&std::declval<L>())>
      constexpr decltype(auto) operator()(L&& l) const {
        return &l;
      }
    };
    /*Default -> operator functor*/
    struct Member_of_pointer {
      template<typename L, typename = std::enable_if_t<std::is_pointer_v<std::decay_t<L>>>>
      constexpr decltype(auto) operator()(L&& l) const {
        return l;
      }

      template<typename L, typename = std::enable_if_t<!std::is_pointer_v<std::decay_t<L>>>, typename = decltype(std::declval<L>().operator->())>
      constexpr decltype(auto) operator()(L&& l) const {
        return l.operator->();
      }
    };
    /*Default () operator functor*/
    struct Function_call {
      template<typename L, typename ...Rest, typename = std::enable_if_t<std::is_invocable_v<L, Rest...>>>
      constexpr decltype(auto) operator()(L&& l, Rest&&... rest) const {
        return l(std::forward<Rest>(rest)...);
      }
    };
    /*Default , operator functor*/
    struct Comma {
      template<typename L, typename R, typename = decltype(std::declval<L>(), std::declval<R>())>
      constexpr decltype(auto) operator()(L&& l, R&& r) const {
        return l, r;
      }
    };
  }
  /*
    These allow or disallow operators based on type signatures.
    e.g. An int + char is allowed in C++, but not having an appropriate filter disallows it
  */
  namespace filters {
    template<typename Operator, typename T>
    decltype(auto) operator_forwarder(T&& t) {
      return Operator{}(unwrap(std::forward<T>(t)));
    }
    template<typename Operator, typename L, typename R>
    decltype(auto) operator_forwarder(L&& l, R&& r) {
      return Operator{}(unwrap(std::forward<L>(l)), unwrap(std::forward<R>(r)));
    }
    /*Allows operations with a specific type on the right of the operator*/
    template<typename Operator, typename Right>
    struct Right_is {
    private:
      template<typename L, typename R>
      static constexpr bool enabled_ = is_strong_type<L>
        && std::is_same_v<std::decay_t<R>, Right>
        && std::is_invocable_v<Operator, Unwrap<L>, Unwrap<R>>;
    public:
      template<typename L, typename R, typename = std::enable_if_t<enabled_<L, R>>>
      constexpr decltype(auto) operator()(L&& l, R&& r) const {
        return operator_forwarder<Operator>(std::forward<L>(l), std::forward<R>(r));
      }
    };
    /*Allows operations with a specific type on the left of the operator*/
    template<typename Operator, typename Left>
    struct Left_is {
    private:
      template<typename L, typename R>
      static constexpr bool enabled_ = is_strong_type<R>
        && std::is_same_v<std::decay_t<L>, Left>
        && std::is_invocable_v<Operator, Unwrap<L>, Unwrap<R>>;
    public:
      template<typename L, typename R, typename = std::enable_if_t<enabled_<L, R>>>
      constexpr decltype(auto) operator()(L&& l, R&& r) const {
        return operator_forwarder<Operator>(std::forward<L>(l), std::forward<R>(r));
      }
    };
    /*Allows operations if both types are the same (same type as self)*/
    template<typename Operator>
    struct Both_same {
    private:
      template<typename L, typename R>
      static constexpr bool enabled_ = is_strong_type<L>
        && std::is_same_v<std::decay_t<L>, std::decay_t<R>>
        && std::is_invocable_v<Operator, Unwrap<L>, Unwrap<R>>;
    public:
      template<typename L, typename R, typename = std::enable_if_t<enabled_<L, R>>>
      constexpr decltype(auto) operator()(L&& l, R&& r) const {
        return operator_forwarder<Operator>(std::forward<L>(l), std::forward<R>(r));
      }
    };
    /*Allow operations with specific types*/
    template<typename Operator, typename ...Args>
    struct Variadic_is {
    private:
      template<typename L, typename ...GivenArgs>
      static constexpr bool enabled_ = is_strong_type<L>
        && std::is_same_v<List<std::decay_t<Args>...>, List<std::decay_t<GivenArgs>...>>
        && std::is_invocable_v<Operator, Args...>;
    public:
      template<typename L, typename ...GivenArgs, typename = std::enable_if_t<enabled_<L, GivenArgs...>>>
      constexpr decltype(auto) operator()(L&& l, GivenArgs&&... rest) const {
        return Operator{}(std::forward<L>(l), std::forward<GivenArgs>(rest)...);
      }
    };
  }
  /*
      These are used to change the return values of the operators.
      E.g. + should probably rewrap the result as the leftmost strong value while ()
      should probably return as is
    */
  namespace return_wrapping {
    /*Returns a value as is*/
    template<typename Nested>
    struct No_wrapping {
    private:
      template<typename ...Args>
      using Sfinae = std::enable_if_t<
        std::is_invocable_v<Nested, Args...>>;
    public:
      template<typename ...Args, typename = Sfinae<Args...>>
      constexpr decltype(auto) operator()(Args&&... args) const {
        return Nested{}(std::forward<Args>(args)...);
      }
    };

    /*Wrap the output in a strongvalue the same as the ith type*/
    template<typename Nested, size_t i>
    struct Wrap_as_ith_arg {
      template<typename ...Args>
      using Sfinae = std::enable_if_t<
        std::is_invocable_v<Nested, Args...>>;

      template<typename ...Args, typename = Sfinae<Args...>>
      constexpr decltype(auto) operator()(Args&&... args) const {
        return GetEx<i, std::decay_t<Args>...>::wrap(
          Nested{}(std::forward<Args>(args)...));
      }
    };

    /*Wrap the result as a Wrapping*/
    template<typename Nested, typename Wrapping>
    struct Static_wrapping {
      template<typename ...Args>
      using Sfinae = std::enable_if_t<
        std::is_invocable_v<Nested, Args...>>;

      template<typename ...Args, typename = Sfinae<Args...>>
      constexpr decltype(auto) operator()(Args&&... args) const {
        return Wrapping::wrap(Nested{}(std::forward<Args>(args)...));
      }
    };

    /*Returns the ith parameter as the result*/
    template<typename Nested, size_t i>
    struct Return_ith_arg {
      template<typename ...Args>
      using Sfinae = std::enable_if_t<
        std::is_invocable_v<Nested, Args...>>;

      template<typename ...Args, typename = Sfinae<Args...>>
      constexpr decltype(auto) operator()(Args&&... args) {
        Nested{}(std::forward<Args>(args)...);
        return get_ith_<i>(std::forward<Args>(args)...);
      }
    private:
      template<size_t i, typename First, typename ...Rest>
      constexpr decltype(auto) get_ith_(First&& first, Rest&&... rest) {
        if constexpr (i == 0) {
          return std::forward<First>(first);
        } else {
          return getIth<i - 1>(std::forward<Rest>(rest)...);
        }
      }
    };
  }
  namespace impl {
    template<typename Functor>
    struct Convert_to;
    template<typename Functor>
    struct Less;
    template<typename Functor>
    struct Greater;
    template<typename Functor>
    struct Less_equal;
    template<typename Functor>
    struct Greater_equal;
    template<typename Functor>
    struct Equal;
    template<typename Functor>
    struct Not_equal;
    template<typename Functor>
    struct Assign;
    template<typename Functor>
    struct Add;
    template<typename Functor>
    struct Add_assign;
    template<typename Functor>
    struct Sub;
    template<typename Functor>
    struct Sub_assign;
    template<typename Functor>
    struct Mult;
    template<typename Functor>
    struct Mult_assign;
    template<typename Functor>
    struct Div;
    template<typename Functor>
    struct Div_assign;
    template<typename Functor>
    struct Mod;
    template<typename Functor>
    struct Mod_assign;
    template<typename Functor>
    struct Binary_and;
    template<typename Functor>
    struct Binary_and_assign;
    template<typename Functor>
    struct Binary_or;
    template<typename Functor>
    struct Binary_or_assign;
    template<typename Functor>
    struct Xor;
    template<typename Functor>
    struct Xor_assign;
    template<typename Functor>
    struct Left_shift;
    template<typename Functor>
    struct Left_shift_assign;
    template<typename Functor>
    struct Right_shift;
    template<typename Functor>
    struct Right_shift_assign;
    template<typename Functor>
    struct Binary_not;
    template<typename Functor>
    struct Unary_plus;
    template<typename Functor>
    struct Unary_minus;
    template<typename Functor>
    struct Subscript;
    template<typename Functor>
    struct Indirection;
    template<typename Functor>
    struct Pre_increment;
    template<typename Functor>
    struct Post_increment;
    template<typename Functor>
    struct Pre_decrement;
    template<typename Functor>
    struct Post_decrement;
    template<typename Functor>
    struct Logical_not;
    template<typename Functor>
    struct Logical_and;
    template<typename Functor>
    struct Logical_or;
    template<typename Functor>
    struct Address_of;
    template<typename Functor>
    struct Member_of_pointer;
    template<typename Functor>
    struct Function_call;
    template<typename Functor>
    struct Comma;

    /*An operator with no arguments (e.g. ->)*/
    template<template<typename Functor> typename Holder, typename Default>
    struct Nullary_op {
      template<typename Functor>
      using Ex = Holder<Functor>;

      using Allow = Ex<Default>;
    };
    /*An operator with 1 arg (e.g. [], =)*/
    template<template<typename Functor> typename Holder, typename Default>
    struct Unary_op {
      template<typename Functor>
      using Ex = Holder<Functor>;

      template<typename Arg>
      using Allow = Ex<filters::Right_is<Default, Arg>>;
    };

    /*An operator with 1 arg, but either side (e.g. +)*/
    template<template<typename Functor> typename Holder, typename Default>
    struct Binary_op {
      template<typename Functor>
      using Ex = Holder<Functor>;

      template<typename Right>
      using LeftOf = Ex<filters::Right_is<Default, Right>>;

      template<typename Left>
      using RightOf = Ex<filters::Left_is<Default, Left>>;

      using Self = Ex<filters::Both_same<Default>>;
    };

    /*An operator with a variadic number of args (e.g. ())*/
    template<template<typename Functor> typename Holder, typename Default>
    struct Variadic_op {
      template<typename Functor>
      using Ex = Holder<Functor>;

      template<typename ...Args>
      using Allow = Ex<filters::Variadic_is<Default, Args...>>;
    };
  }
  /*Allows copy construction of the type*/
  struct Copy;
  /*Allows move construction of the type*/
  struct Move;
  /*Allows construction of the type with Types...*/
  template<typename ...Types>
  struct Construct;

  /*Removes the destructor. Destructor is the only operation allows by default*/
  struct NoDestructor;

  /*Allows conversion to To*/
  template<typename To>
  using Convert_to = impl::Convert_to<defaults::Convert_to<To>>;

  /*<*/
  using Less = impl::Binary_op<impl::Less, defaults::Less>;
  /*>*/
  using Greater = impl::Binary_op<impl::Greater, defaults::Greater>;
  /*<=*/
  using Less_equal = impl::Binary_op<impl::Less_equal, defaults::Less_equal>;
  /*>=*/
  using Greater_equal = impl::Binary_op<impl::Greater_equal, defaults::Greater_equal>;
  /*==*/
  using Equal = impl::Binary_op<impl::Equal, defaults::Equal>;
  /*!=*/
  using Not_equal = impl::Binary_op<impl::Not_equal, defaults::Not_equal>;
  /*=*/
  using Assign = impl::Unary_op<impl::Assign, defaults::Assign>;
  /*+*/
  using Add = impl::Binary_op<impl::Add, defaults::Add>;
  /*+=*/
  using Add_assign = impl::Binary_op<impl::Add_assign, defaults::Add_assign>;
  /*-*/
  using Sub = impl::Binary_op<impl::Sub, defaults::Sub>;
  /*-=*/
  using Sub_assign = impl::Binary_op<impl::Sub_assign, defaults::Sub_assign>;
  /***/
  using Mult = impl::Binary_op<impl::Mult, defaults::Mult>;
  /**=*/
  using Mult_assign = impl::Binary_op<impl::Mult_assign, defaults::Mult_assign>;
  using Div = impl::Binary_op<impl::Div, defaults::Div>;
  using Div_assign = impl::Binary_op<impl::Div_assign, defaults::Div_assign>;
  using Mod = impl::Binary_op<impl::Mod, defaults::Mod>;
  using Mod_assign = impl::Binary_op<impl::Mod_assign, defaults::Mod_assign>;
  using Binary_and = impl::Binary_op<impl::Binary_and, defaults::Binary_and>;
  using Binary_and_assign = impl::Binary_op<impl::Binary_and_assign, defaults::Binary_and_assign>;
  using Binary_or = impl::Binary_op<impl::Binary_or, defaults::Binary_or>;
  using Binary_or_assign = impl::Binary_op<impl::Binary_or_assign, defaults::Binary_or_assign>;
  using Xor = impl::Binary_op<impl::Xor, defaults::Xor>;
  using Xor_assign = impl::Binary_op<impl::Xor_assign, defaults::Xor_assign>;
  using Left_shift = impl::Binary_op<impl::Left_shift, defaults::Left_shift>;
  using Left_shift_assign = impl::Binary_op<impl::Left_shift_assign, defaults::Left_shift_assign>;
  using Right_shift = impl::Binary_op<impl::Right_shift, defaults::Right_shift>;
  using Right_shift_assign = impl::Binary_op<impl::Right_shift_assign, defaults::Right_shift_assign>;
  using Binary_not = impl::Nullary_op<impl::Binary_not, defaults::Binary_not>;
  using Unary_plus = impl::Nullary_op<impl::Unary_plus, defaults::Unary_plus>;
  using Unary_minus = impl::Nullary_op<impl::Unary_minus, defaults::Unary_minus>;

  using Subscript = impl::Unary_op<impl::Subscript, defaults::Subscript>;

  using Pre_increment = impl::Nullary_op<impl::Pre_increment, defaults::Pre_increment>;
  using Pre_decrement = impl::Nullary_op<impl::Pre_decrement, defaults::Pre_decrement>;
  using Post_increment = impl::Nullary_op<impl::Post_increment, defaults::Post_increment>;
  using Post_decrement = impl::Nullary_op<impl::Post_decrement, defaults::Post_decrement>;

  using Logical_not = impl::Nullary_op<impl::Logical_not, defaults::Logical_not>;
  using Logical_and = impl::Binary_op<impl::Logical_and, defaults::Logical_and>;
  using Logical_or = impl::Binary_op<impl::Logical_or, defaults::Logical_or>;

  using Indirection = impl::Nullary_op<impl::Indirection, defaults::Indirection>;
  using Address_of = impl::Nullary_op<impl::Address_of, defaults::Address_of>;
  using Member_of_pointer = impl::Nullary_op<impl::Member_of_pointer, defaults::Member_of_pointer>;
  //NYI using PointerToMemberOfPointer = impl::Unary_op<impl::PointerToMemberOfPointer>;

  using Function_call = impl::Variadic_op<impl::Function_call, defaults::Function_call>;
  using Comma = impl::Binary_op<impl::Comma, defaults::Comma>;

  using Orderable = impl::NestedExpandTo<Less::Self, Greater::Self, Less_equal::Self, Greater_equal::Self, Equal::Self, Not_equal::Self>;

  using Regular = impl::NestedExpandTo<Construct<>, Copy, Move, Orderable>;

  using Group = impl::NestedExpandTo<Add::Self, Add_assign::Self, Sub::Self, Sub_assign::Self>;

  using Field = impl::NestedExpandTo<Add::Self, Sub::Self, Mult::Self, Div::Self, Add_assign::Self, Sub_assign::Self, Mult_assign::Self, Div_assign::Self>;

  namespace impl {

    /*SFINAE for allowing a constructor overload*/
    template<typename ArgsConstructable, typename Options>
    using AllowConstructor = std::enable_if_t <contains<ArgsConstructable, Options>>;

    /*helper template struct for disallowing destructors*/
    template<bool>
    struct DisallowDestructorImpl;

    /*if we should not disallow, nothing*/
    template<>
    struct DisallowDestructorImpl<false> {

    };
    /*If we should disallow destruction, delete the destructor*/
    template<>
    struct DisallowDestructorImpl<true> {
      ~DisallowDestructorImpl() = delete;
    };

    /*Helper typedef to choose the appropriate DisallowDestructorImpl specialization*/
    template<typename Options>
    using DisallowDestructor =
      DisallowDestructorImpl<contains<NoDestructor, Options>>;

    /*Helper template struct for allowing copy*/
    template<bool>
    struct AllowCopyImpl;

    /*If we should allow, nothing special*/
    template<>
    struct AllowCopyImpl<true> {
    };
    /*If we shouldn't allow copying, delete copying*/
    template<>
    struct AllowCopyImpl<false> {
      AllowCopyImpl() = default;
      AllowCopyImpl(AllowCopyImpl const&) = delete;
      AllowCopyImpl(AllowCopyImpl&&) = default;

      AllowCopyImpl& operator=(AllowCopyImpl const&) = delete;
      AllowCopyImpl& operator=(AllowCopyImpl&&) = default;
    };
    /*Helper typedef to choose the appropriate AllowCopyImpl specialization*/
    template<typename Options>
    using AllowCopy = AllowCopyImpl<contains<Copy, Options>>;
    /*Helper template struct for allowing move*/
    template<bool>
    struct AllowMoveImpl;
    /*if we should allow, nothing special*/
    template<>
    struct AllowMoveImpl<true> {
    };
    /*If we shouldn't allow moving, delete moving*/
    template<>
    struct AllowMoveImpl<false> {
      AllowMoveImpl() = default;
      AllowMoveImpl(AllowMoveImpl const&) = default;
      AllowMoveImpl(AllowMoveImpl&&) = delete;

      AllowMoveImpl& operator=(AllowMoveImpl const&) = default;
      AllowMoveImpl& operator=(AllowMoveImpl&&) = delete;
    };
    /*Helper typedef to choose the appropriate AllowMoveImpl specialization*/
    template<typename Options>
    using AllowMove = AllowMoveImpl<contains<Move, Options>>;

    /*A helper to filter functors by type, Value is true_type if T is Option<SomethingElse>*/
    template<template<typename> typename Option>
    struct GetFunctorsHelper {
      template<typename T>
      using Value = ToIntegralConstant<isWrappedBy<Option, T>>;
    };
    /*Get a list of all functors of a particular option in options*/
    template<template<typename> typename Option, typename Options>
    using GetFunctors = Transform<
      First,
      Filter<GetFunctorsHelper<Option>::template Value, Options>>;

    template<typename Target, typename Type>
    struct AllowConversionToHelper {
      template<typename T>
      using Value = std::is_same<
        std::invoke_result_t<T, Type>,
        Target>;
    };

    template<typename Target, typename Type, typename Options>
    using AllowConversionTo = First<Filter<AllowConversionToHelper<Target, Type>::template Value, GetFunctors<Convert_to, Options>>>;

    template<typename ...Args>
    struct GetMatchingFunctorHelper {
      template<typename Functor>
      using Value = std::is_invocable<Functor, Args...>;
    };

    template<template<typename> typename Option, typename ...Ts>
    using GetFunctor = First<Filter<GetMatchingFunctorHelper<Ts...>::template Value, GetFunctors<Option, Concat<Strong_type_options<Ts>...>>>>;

    template<typename Type_, typename Options_>
    struct Strong_type :
      private DisallowDestructor<Options_>,
      private AllowCopy<Options_>,
      private AllowMove<Options_> {
    public:
      using Strong_type_tag = void;
      using Type = Type_;
      using Options = Options_;

      template<typename ...Args, typename = AllowConstructor<Construct<Args...>, Options>>
      explicit Strong_type(Args&&... args) :
        val_{ std::forward<Args>(args)... } {

      }

      Strong_type(Strong_type&& t) = default;

      Strong_type(Strong_type const& t) = default;

      Strong_type& operator=(Strong_type&& t) = default;

      Strong_type& operator=(Strong_type const& t) = default;

      template<typename T, typename Functor = GetFunctor<Assign, Strong_type&, T>>
      decltype(auto) operator=(T&& t) {
        return Functor{}(*this, std::forward<T>(t));
      }

      template<typename T, typename Functor = GetFunctor<Assign, Strong_type const&, T>>
      decltype(auto) operator=(T&& t) const {
        return Functor{}(*this, std::forward<T>(t));
      }

      template<typename T, typename Functor = AllowConversionTo<T, Type&, Options>>
      explicit operator T() {
        return Functor{}(val_);
      }

      template<typename T, typename Functor = AllowConversionTo<T, Type const&, Options>>
      explicit operator T()  const {
        return Functor{}(val_);
      }

      template<typename T, typename Functor = GetFunctor<Subscript, Strong_type&, T>>
      decltype(auto) operator[](T&& t) {
        return Functor{}(*this, std::forward<T>(t));
      }

      template<typename T, typename Functor = GetFunctor<Subscript, Strong_type const&, T>>
      decltype(auto) operator[](T&& t) const {
        return Functor{}(*this, std::forward<T>(t));
      }

      template<typename T = Strong_type & , typename Functor = GetFunctor<MemberOfPointer, T>>
      decltype(auto) operator->() {
        return Functor{}(*this);
      }

      template<typename T = Strong_type const&, typename Functor = GetFunctor<MemberOfPointer, T>>
      decltype(auto) operator->() const {
        return Functor{}(*this);
      }

      template<typename ...Args, typename Functor = GetFunctor<FunctionCall, Strong_type&, Args...>>
      decltype(auto) operator()(Args&&... args) {
        return Functor{}(*this, std::forward<Args>(args)...);
      }

      template<typename ...Args, typename Functor = GetFunctor<FunctionCall, Strong_type const&, Args...>>
      decltype(auto) operator()(Args&&... args) const {
        return Functor{}(*this, std::forward<Args>(args)...);
      }

      Type& get() noexcept {
        return val_;
      }

      Type const& get() const noexcept {
        return val_;
      }

      template<typename ...Args>
      static Strong_type wrap(Args&&... args) {
        return Strong_type{ escapeHatch_, std::forward<Args>(args)... };
      }
    private:
      struct EscapeHatch {};

      static constexpr EscapeHatch escapeHatch_{};

      template<typename ...Args>
      Strong_type(EscapeHatch, Args&&... args) :
        val_{ std::forward<Args>(args)... } {

      }

      Type val_;
    };
  }

  template<typename Type, typename ...Options>
  using Strong_type = impl::Strong_type<Type, ExpandEx<impl::ExpandTo, Options...>>;
}

template<typename L, typename R, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Less, L, R>>
decltype(auto) operator<(L&& l, R&& r) {
  return Functor{}(std::forward<L>(l), std::forward<R>(r));
}

template<typename L, typename R, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Greater, L, R>>
decltype(auto) operator>(L&& l, R&& r) {
  return Functor{}(std::forward<L>(l), std::forward<R>(r));
}

template<typename L, typename R, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Less_equal, L, R>>
decltype(auto) operator<=(L&& l, R&& r) {
  return Functor{}(std::forward<L>(l), std::forward<R>(r));
}

template<typename L, typename R, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Greater_equal, L, R>>
decltype(auto) operator>=(L&& l, R&& r) {
  return Functor{}(std::forward<L>(l), std::forward<R>(r));
}

template<typename L, typename R, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Equal, L, R>>
decltype(auto) operator==(L&& l, R&& r) {
  return Functor{}(std::forward<L>(l), std::forward<R>(r));
}

template<typename L, typename R, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Not_equal, L, R>>
decltype(auto) operator!=(L&& l, R&& r) {
  return Functor{}(std::forward<L>(l), std::forward<R>(r));
}

template<typename L, typename R, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Add, L, R>>
decltype(auto) operator+(L&& l, R&& r) {
  return Functor{}(std::forward<L>(l), std::forward<R>(r));
}

template<typename L, typename R, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Add_assign, L, R>>
decltype(auto) operator+=(L&& l, R&& r) {
  return Functor{}(std::forward<L>(l), std::forward<R>(r));
}

template<typename L, typename R, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Sub, L, R>>
decltype(auto) operator-(L&& l, R&& r) {
  return Functor{}(std::forward<L>(l), std::forward<R>(r));
}

template<typename L, typename R, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Sub_assign, L, R>>
decltype(auto) operator-=(L&& l, R&& r) {
  return Functor{}(std::forward<L>(l), std::forward<R>(r));
}

template<typename L, typename R, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Mult, L, R>>
decltype(auto) operator*(L&& l, R&& r) {
  return Functor{}(std::forward<L>(l), std::forward<R>(r));
}

template<typename L, typename R, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Mult_assign, L, R>>
decltype(auto) operator*=(L&& l, R&& r) {
  return Functor{}(std::forward<L>(l), std::forward<R>(r));
}

template<typename L, typename R, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Div, L, R>>
decltype(auto) operator/(L&& l, R&& r) {
  return Functor{}(std::forward<L>(l), std::forward<R>(r));
}

template<typename L, typename R, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Div_assign, L, R>>
decltype(auto) operator/=(L&& l, R&& r) {
  return Functor{}(std::forward<L>(l), std::forward<R>(r));
}

template<typename L, typename R, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Mod, L, R>>
decltype(auto) operator%(L&& l, R&& r) {
  return Functor{}(std::forward<L>(l), std::forward<R>(r));
}

template<typename L, typename R, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Mod_assign, L, R>>
decltype(auto) operator%=(L&& l, R&& r) {
  return Functor{}(std::forward<L>(l), std::forward<R>(r));
}

template<typename L, typename R, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Binary_and, L, R>>
decltype(auto) operator&(L&& l, R&& r) {
  return Functor{}(std::forward<L>(l), std::forward<R>(r));
}

template<typename L, typename R, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Binary_and_assign, L, R>>
decltype(auto) operator&=(L&& l, R&& r) {
  return Functor{}(std::forward<L>(l), std::forward<R>(r));
}

template<typename L, typename R, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Binary_or, L, R>>
decltype(auto) operator|(L&& l, R&& r) {
  return Functor{}(std::forward<L>(l), std::forward<R>(r));
}

template<typename L, typename R, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Binary_or_assign, L, R>>
decltype(auto) operator|=(L&& l, R&& r) {
  return Functor{}(std::forward<L>(l), std::forward<R>(r));
}

template<typename L, typename R, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Xor, L, R>>
decltype(auto) operator^(L&& l, R&& r) {
  return Functor{}(std::forward<L>(l), std::forward<R>(r));
}

template<typename L, typename R, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Xor_assign, L, R>>
decltype(auto) operator^=(L&& l, R&& r) {
  return Functor{}(std::forward<L>(l), std::forward<R>(r));
}

template<typename L, typename R, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Left_shift, L, R>>
decltype(auto) operator<<(L&& l, R&& r) {
  return Functor{}(std::forward<L>(l), std::forward<R>(r));
}

template<typename L, typename R, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Left_shift_assign, L, R>>
decltype(auto) operator<<=(L&& l, R&& r) {
  return Functor{}(std::forward<L>(l), std::forward<R>(r));
}

template<typename L, typename R, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Right_shift, L, R>>
decltype(auto) operator>>(L&& l, R&& r) {
  return Functor{}(std::forward<L>(l), std::forward<R>(r));
}

template<typename L, typename R, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Right_shift_assign, L, R>>
decltype(auto) operator>>=(L&& l, R&& r) {
  return Functor{}(std::forward<L>(l), std::forward<R>(r));
}

template<typename L, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Binary_not, L>>
decltype(auto) operator~(L&& l) {
  return Functor{}(std::forward<L>(l));
}

template<typename L, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Unary_plus, L>>
decltype(auto) operator+(L&& l) {
  return Functor{}(std::forward<L>(l));
}

template<typename L, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Unary_minus, L>>
decltype(auto) operator-(L&& l) {
  return Functor{}(std::forward<L>(l));
}

template<typename L, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Pre_increment, L>>
decltype(auto) operator++(L&& l) {
  return Functor{}(std::forward<L>(l));
}

template<typename L, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Pre_decrement, L>>
decltype(auto) operator--(L&& l) {
  return Functor{}(std::forward<L>(l));
}

template<typename L, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Post_increment, L>>
decltype(auto) operator++(L&& l, int) {
  return Functor{}(std::forward<L>(l));
}

template<typename L, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Post_decrement, L>>
decltype(auto) operator--(L&& l, int) {
  return Functor{}(std::forward<L>(l));
}

template<typename L, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Logical_not, L>>
decltype(auto) operator!(L&& l) {
  return Functor{}(std::forward<L>(l));
}

template<typename L, typename R, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Logical_and, L, R>>
decltype(auto) operator&&(L&& l, R&& r) {
  return Functor{}(std::forward<L>(l), std::forward<R>(r));
}

template<typename L, typename R, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Logical_or, L, R>>
decltype(auto) operator||(L&& l, R&& r) {
  return Functor{}(std::forward<L>(l), std::forward<R>(r));
}

template<typename L, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Indirection, L>>
decltype(auto) operator*(L&& l) {
  return Functor{}(std::forward<L>(l));
}

template<typename L, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Address_of, L>>
decltype(auto) operator&(L&& l) {
  return Functor{}(std::forward<L>(l));
}

template<typename L, typename R, typename Functor = ::dlib::strong_type::impl::GetFunctor<::dlib::strong_type::impl::Comma, L, R>>
decltype(auto) operator,(L&& l, R&& r) {
  return Functor{}(std::forward<L>(l), std::forward<R>(r));
}