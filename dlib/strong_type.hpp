#pragma once

#include <type_traits>
#include <dlib/meta.hpp>

/*

  Used for creating strong types.
  
  All operations including construction are disallowed be default, and must be enabled
  by specifying them in the template args. A tag to make each template a unique type can be given by any type, as these allowed operations are pulled out of the variadic template args.

*/

namespace dlib::strongValue {

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
    struct StrongValue;

    /* Helper class to find if something is a strong value*/
    template<typename T>
    struct IsStrongValue {
      static constexpr bool value = false;
    };

    /* Helper class to find it something is a strong value, partial spec*/
    template<typename Type, typename Options>
    struct IsStrongValue<StrongValue<Type, Options>> {
      static constexpr bool value = true;
    };

    /*Finds if something is a strong value*/
    template<typename T>
    constexpr bool isStrongValue = IsStrongValue<std::decay_t<T>>::value;

    /*Declaration of a class to pull type and options out of a strong value*/
    template<typename T>
    struct StrongValueInfo;

    /*Partial spec to pull type and options out of a strong value*/
    template<typename Type_, typename Options_>
    struct StrongValueInfo<StrongValue<Type_, Options_>> {
      using Type = Type_;
      using Options = Options_;
    };

    /* Helper typedef to get type of a strong value*/
    template<typename T>
    using StrongValueType = typename StrongValueInfo<std::decay_t<T>>::Type;

    /* Helper typedef to get the options of a strong value*/
    template<typename T>
    using StrongValueOptions = typename StrongValueInfo<std::decay_t<T>>::Options;

    /*Unwraps a strong value if it is one, otherwise returns t*/
    template<typename T>
    constexpr decltype(auto) unwrap(T&& t) noexcept {
      if constexpr(isStrongValue<T>) {
        return t.get();
      } else {
        return std::forward<T>(t);
      }
    }

    /*Gets the unwrapped (the type if its a strong value, otherwise T) type of T*/
    template<typename T>
    using UnWrap = decltype(unwrap(std::declval<T>()));

    namespace defaults {
      /*
        These call the operators on the given args
      */
      namespace operators {
        /*Default conversion operator functor*/
        template<typename To>
        struct Converter {
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
        struct LessEqual {
          template<typename L, typename R, typename = decltype(std::declval<L>() <= std::declval<R>())>
          constexpr decltype(auto) operator()(L&& l, R&& r) const {
            return  l <= r;
          }
        };
        /*Default >= operator functor*/
        struct GreaterEqual {
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
        struct NotEqual {
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
        struct AddAssign {
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
        struct SubAssign {
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
        struct MultAssign {
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
        struct DivAssign {
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
        struct ModAssign {
          template<typename L, typename R, typename = decltype(std::declval<L>() %= std::declval<R>())>
          constexpr decltype(auto) operator()(L&&l, R&& r) const {
            return l %= r;
          }
        };
        /*Default & operator functor*/
        struct BinaryAnd {
          template<typename L, typename R, typename = decltype(std::declval<L>() & std::declval<R>())>
          constexpr decltype(auto) operator()(L&& l, R&& r) const {
            return l & r;
          }
        };
        /*Default &= operator functor*/
        struct BinaryAndAssign {
          template<typename L, typename R, typename = decltype(std::declval<L>() &= std::declval<R>())>
          constexpr decltype(auto) operator()(L&&l, R&& r) const {
            return l &= r;
          }
        };
        /*Default | operator functor*/
        struct BinaryOr {
          template<typename L, typename R, typename = decltype(std::declval<L>() | std::declval<R>())>
          constexpr decltype(auto) operator()(L&& l, R&& r) const {
            return l | r;
          }
        };
        /*Default |= operator functor*/
        struct BinaryOrAssign {
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
        struct XorAssign {
          template<typename L, typename R, typename = decltype(std::declval<L>() ^= std::declval<R>())>
          constexpr decltype(auto) operator()(L&&l, R&& r) const {
            return l ^= r;
          }
        };
        /*Default << operator functor*/
        struct LeftShift {
          template<typename L, typename R, typename = decltype(std::declval<L>() << std::declval<R>())>
          constexpr decltype(auto) operator()(L&& l, R&& r) const {
            return l << r;
          }
        };
        /*Default <<= operator functor*/
        struct LeftShiftAssign {
          template<typename L, typename R, typename = decltype(std::declval<L>() <<= std::declval<R>())>
          constexpr decltype(auto) operator()(L&&l, R&& r) const {
            return l <<= r;
          }
        };
        /*Default >> operator functor*/
        struct RightShift {
          template<typename L, typename R, typename = decltype(std::declval<L>() >> std::declval<R>())>
          constexpr decltype(auto) operator()(L&& l, R&& r) const {
            return l >> r;
          }
        };
        /*Default >>= operator functor*/
        struct RightShiftAssign {
          template<typename L, typename R, typename = decltype(std::declval<L>() >>= std::declval<R>())>
          constexpr decltype(auto) operator()(L&&l, R&& r) const {
            return l >>= r;
          }
        };
        /*Default ~ operator functor*/
        struct BinaryNot {
          template<typename L, typename = decltype(~std::declval<L>())>
          constexpr decltype(auto) operator()(L&&l) const {
            return ~l;
          }
        };
        /*Default unary + (e.g. +strongType) operator functor*/
        struct UnaryPlus {
          template<typename L, typename = decltype(+std::declval<L>())>
          constexpr decltype(auto) operator()(L&&l) const {
            return +l;
          }
        };
        /*Default unary - (e.g. -strongType) operator functor*/
        struct UnaryMinus {
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
        struct PreIncrement {
          template<typename L, typename = decltype(++std::declval<L>())>
          constexpr decltype(auto) operator()(L&&l) const {
            return ++l;
          }
        };
        /*Default pre -- (e.g. --strongType) operator functor*/
        struct PreDecrement {
          template<typename L, typename = decltype(--std::declval<L>())>
          constexpr decltype(auto) operator()(L&&l) const {
            return --l;
          }
        };
        /*Default post ++ (e.g. strongType++) operator functor*/
        struct PostIncrement {
          template<typename L, typename = decltype(std::declval<L>()++)>
          constexpr decltype(auto) operator()(L&&l) const {
            return l++;
          }
        };
        /*Default post -- (e.g. strongType--) operator functor*/
        struct PostDecrement {
          template<typename L, typename = decltype(std::declval<L>()--)>
          constexpr decltype(auto) operator()(L&&l) const {
            return l--;
          }
        };
        /*Default ! operator functor*/
        struct LogicalNot {
          template<typename L, typename = decltype(!std::declval<L>())>
          constexpr decltype(auto) operator()(L&& l) const {
            return !l;
          }
        };
        /*Default && operator functor*/
        struct LogicalAnd {
          template<typename L, typename R, typename = decltype(std::declval<L>() && std::declval<R>())>
          constexpr decltype(auto) operator()(L&& l, R&& r) const {
            return l && r;
          }
        };
        /*Default || operator functor*/
        struct LogicalOr {
          template<typename L, typename R, typename = decltype(std::declval<L>() || std::declval<R>())>
          constexpr decltype(auto) operator()(L&& l, R&& r) const noexcept(std::declval<L>() || std::declval<R>()) {
            return l || r;
          }
        };
        /*Default & operator functor*/
        struct AddressOf {
          template<typename L, typename = decltype(&std::declval<L>())>
          constexpr decltype(auto) operator()(L&& l) const {
            return &l;
          }
        };
        /*Default -> operator functor*/
        struct MemberOfPointer {
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
        struct FunctionCall {
          template<typename L, typename ...Rest, typename = std::enable_if_t<std::is_invocable_v<L, Rest...>>>
          constexpr decltype(auto) operator()(L&& l, Rest&&... rest) const {
            return l(std::forward<Rest>(rest)...);
          }
        };
        /*Default , operator functor*/
        struct Comma {
          template<typename L, typename R, typename = decltype(std::declval<L>(), std::declval<R>())>
          constexpr decltype(auto) operator()(L&& l, R&& r) {
            return l, r;
          }
        };
      }
      /*
        These allow or disallow operators based on type signatures.
        e.g. An int + char is allowed in C++, but not having an appropriate filter disallows it
      */
      namespace filters {
        /*Allows operations with a specific type on the right of the operator*/
        template<typename Operator, typename Right>
        struct EnableOperatorIfRightIs {
        private:
          template<typename L, typename R>
          static constexpr bool enabled_ = isStrongValue<L>
            && std::is_same_v<std::decay_t<R>, Right>
            && std::is_invocable_v<Operator, UnWrap<L>, UnWrap<R>>;
        public:
          template<typename L, typename R, typename = std::enable_if_t<enabled_<L, R>>>
          constexpr decltype(auto) operator()(L&& l, R&& r) const {
            return Operator{}(unwrap(std::forward<L>(l)), unwrap(std::forward<R>(r)));
          }
        };
        /*Allows operations with a specific type on the left of the operator*/
        template<typename Operator, typename Left>
        struct EnableOperatorIfLeftIs {
        private:
          template<typename L, typename R>
          static constexpr bool enabled_ = isStrongValue<R>
            && std::is_same_v<std::decay_t<L>, Left>
            && std::is_invocable_v<Operator, UnWrap<L>, UnWrap<R>>;
        public:
          template<typename L, typename R, typename = std::enable_if_t<enabled_<L, R>>>
          constexpr decltype(auto) operator()(L&& l, R&& r) const {
            return Operator{}(unwrap(std::forward<L>(l)), unwrap(std::forward<R>(r)));
          }
        };
        /*Allows operations if both types are the same (same type as self)*/
        template<typename Operator>
        struct EnableOperatorIfBothSame {
        private:
          template<typename L, typename R>
          static constexpr bool enabled_ = isStrongValue<L>
            && std::is_same_v<std::decay_t<L>, std::decay_t<R>>
            && std::is_invocable_v<Operator, UnWrap<L>, UnWrap<R>>;
        public:
          template<typename L, typename R, typename = std::enable_if_t<enabled_<L, R>>>
          constexpr decltype(auto) operator()(L&& l, R&& r) const {
            return Operator{}(unwrap(std::forward<L>(l)), unwrap(std::forward<R>(r)));
          }
        };
        /*Allows operations with a strong value*/
        template<typename Operator>
        struct EnableOperatorIfStrongValue {
        private:
          template<typename L>
          static constexpr bool enabled_ = isStrongValue<L>
            && std::is_invocable_v<Operator, UnWrap<L>>;
        public:
          template<typename L, typename = std::enable_if_t<enabled_<L>>>
          constexpr decltype(auto) operator()(L&& l) const {
            return Operator{}(unwrap(std::forward<L>(l)));
          }
        };
        /*Allow operations with specific types*/
        template<typename Operator, typename ...Args>
        struct EnableOperatorIfVariadicIs {
        private:
          template<typename L, typename ...GivenArgs>
          static constexpr bool enabled_ = isStrongValue<L>
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
        struct NoWrappingImpl {
        private:
          template<typename ...Args>
          static constexpr bool enabled_ =
            std::is_invocable_v<Nested, Args...>;
        public:
          template<typename ...Args, typename = std::enable_if_t<enabled_<Args...>>>
          constexpr decltype(auto) operator()(Args&&... args) const {
            return Nested{}(std::forward<Args>(args)...);
          }
        };
        /*Returns a value as is*/
        using NoWrapping = NoWrappingImpl<Placeholder>;

        /*Wrap the output in a strongvalue the same as the ith type*/
        template<size_t i>
        struct WrapAsIthArgImpl {

          template<typename Nested>
          struct Impl {
          private:
            template<typename ...Args>
            static constexpr bool enabled_ =
              std::is_invocable_v<Nested, Args...>;
          public:
            template<typename ...Args, typename = std::enable_if_t<enabled_<Args...>>>
            constexpr decltype(auto) operator()(Args&&... args) const {
              return GetEx<i, std::decay_t<Args>...>::wrap(Nested{}(std::forward<Args>(args)...));
            }
          };
        };
        /*Wrap the output in a strongvalue the same as the ith type*/
        template<size_t i>
        using WrapAsIthArg = typename WrapAsIthArgImpl<i>::template Impl<Placeholder>;

        /*Wrap the result as a Wrapping*/
        template<typename Wrapping>
        struct StaticWrappingImpl {
          template<typename Nested>
          struct Impl {
          private:
            template<typename ...Args>
            static constexpr bool enabled_ =
              std::is_invocable_v<Nested, Args...>;
          public:
            template<typename ...Args, typename = std::enable_if_t<enabled_<Args...>>>
            constexpr decltype(auto) operator()(Args&&... args) const {
              return Wrapping::wrap(Nested{}(std::forward<Args>(args)...));
            }
          };
        };
        /*Wrap the result as a Wrapping*/
        template<typename Wrapping>
        using StaticWrapping = typename StaticWrappingImpl<Wrapping>::template Impl<Placeholder>;

        /*Returns the ith parameter as the result*/
        template<size_t i>
        struct ReturnIthArgImpl {
          template<typename Nested>
          struct Impl {
          private:
            template<typename ...Args>
            static constexpr bool enabled_ =
              std::is_invocable_v<Nested, Args...>;
          public:
            template<typename ...Args, typename = std::enable_if_t<enabled_<Args...>>>
            constexpr decltype(auto) operator()(Args&&... args) {
              Nested{}(std::forward<Args>(args)...);
              return getIth<i>(std::forward<Args>(args)...);
            }

            template<size_t i, typename First, typename ...Rest>
            constexpr decltype(auto) getIth(First&& first, Rest&&... rest) {
              if constexpr (i == 0) {
                return std::forward<First>(first);
              } else {
                return getIth<i - 1>(std::forward<Rest>(rest)...);
              }
            }
          };
        };

        /*Returns the ith parameter as the result*/
        template<size_t i>
        using ReturnIthArg = typename ReturnIthArgImpl<i>::template Impl<Placeholder>;
      }
      /*An operator with no arguments (e.g. ->)*/
      template<typename Operator, typename DefaultWrapping>
      struct NullaryOp {
        template<typename Functor>
        struct Ex;

        template<typename Wrapping = DefaultWrapping>
        using Using = Ex<ChangeContainedEx<Wrapping, filters::EnableOperatorIfStrongValue<Operator>>>;
      };
      /*An operator with 1 arg (e.g. [])*/
      template<typename Operator, typename DefaultWrapping>
      struct UnaryOp {
        template<typename Functor>
        struct Ex;

        template<typename Arg, typename Wrapping = DefaultWrapping>
        using Using = Ex<ChangeContainedEx<Wrapping, filters::EnableOperatorIfLeftIs<Operator, Arg>>>;
      };

      /*An operator with 1 arg, but either side (e.g. +)*/
      template<typename Operator, typename DefaultWrapping>
      struct BinaryOp {
        template<typename Functor>
        struct Ex;

        template<typename Right, typename Wrapping = DefaultWrapping>
        using LeftOf = Ex<ChangeContainedEx<Wrapping, filters::EnableOperatorIfRightIs<Operator, Right>>>;

        template<typename Left, typename Wrapping = DefaultWrapping>
        using RightOf = Ex<ChangeContainedEx<Wrapping, filters::EnableOperatorIfLeftIs<Operator, Left>>>;

        template<typename Wrapping = DefaultWrapping>
        using Self = Ex<ChangeContainedEx<Wrapping, filters::EnableOperatorIfBothSame<Operator>>>;
      };

      /*An operator with a variadic number of args (e.g. ())*/
      template<typename Operator>
      struct VariadicOp {
        template<typename Functor>
        struct Ex;

        template<typename ...Args>
        using Using = Ex <filters::EnableOperatorIfVariadicIs<Operator, Args...>>;
      };
    }
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

  /*Allows conversion using Functor*/
  template<typename Functor>
  struct ConvertToEx;

  /*Allows conversion to To*/
  template<typename To>
  using ConvertTo = ConvertToEx<impl::defaults::operators::Converter<To>>;

  /*Don't wrap the result, return it as is*/
  using NoWrapping = impl::defaults::return_wrapping::NoWrapping;
  /*Wrap the result as T*/
  template<typename T>
  using WrapAs = impl::defaults::return_wrapping::StaticWrapping<T>;
  /*Wrap the result as the ith type*/
  template<size_t i>
  using WrapAsIthArg = impl::defaults::return_wrapping::WrapAsIthArg<i>;
  /*Wrap the result as the first type*/
  using WrapAsFirstArg = WrapAsIthArg<0>;
  /*Wrap the result as the second type*/
  using WrapAsSecondArg = WrapAsIthArg<1>;
  /*Return the ith parameter*/
  template<size_t i>
  using ReturnAsIthArg = impl::defaults::return_wrapping::ReturnIthArg<i>;
  /*Return the first parameter*/
  using ReturnAsFirstArg = ReturnAsIthArg<0>;
  /*Return the second parameter*/
  using ReturnAsSecondArg = ReturnAsIthArg<1>;

  /*<*/
  using Less = impl::defaults::BinaryOp<impl::defaults::operators::Less, NoWrapping>;
  /*>*/
  using Greater = impl::defaults::BinaryOp<impl::defaults::operators::Greater, NoWrapping>;
  /*<=*/
  using LessEqual = impl::defaults::BinaryOp<impl::defaults::operators::LessEqual, NoWrapping>;
  /*>=*/
  using GreaterEqual = impl::defaults::BinaryOp<impl::defaults::operators::GreaterEqual, NoWrapping>;
  /*==*/
  using Equal = impl::defaults::BinaryOp<impl::defaults::operators::Equal, NoWrapping>;
  /*!=*/
  using NotEqual = impl::defaults::BinaryOp<impl::defaults::operators::NotEqual, NoWrapping>;
  /*=*/
  using Assign = impl::defaults::UnaryOp<impl::defaults::operators::Assign, WrapAsFirstArg>;
  /*+*/
  using Add = impl::defaults::BinaryOp<impl::defaults::operators::Add, WrapAsFirstArg>;
  /*+=*/
  using AddAssign = impl::defaults::BinaryOp<impl::defaults::operators::AddAssign, ReturnAsFirstArg>;
  /*-*/
  using Sub = impl::defaults::BinaryOp<impl::defaults::operators::Sub, WrapAsFirstArg>;
  /*-=*/
  using SubAssign = impl::defaults::BinaryOp<impl::defaults::operators::SubAssign, ReturnAsFirstArg>;
  /**/
  using Mult = impl::defaults::BinaryOp<impl::defaults::operators::Mult, WrapAsFirstArg>;
  using MultAssign = impl::defaults::BinaryOp<impl::defaults::operators::MultAssign, ReturnAsFirstArg>;
  using Div = impl::defaults::BinaryOp<impl::defaults::operators::Div, WrapAsFirstArg>;
  using DivAssign = impl::defaults::BinaryOp<impl::defaults::operators::DivAssign, ReturnAsFirstArg>;
  using Mod = impl::defaults::BinaryOp<impl::defaults::operators::Mod, WrapAsFirstArg>;
  using ModAssign = impl::defaults::BinaryOp<impl::defaults::operators::ModAssign, ReturnAsFirstArg>;
  using BinaryAnd = impl::defaults::BinaryOp<impl::defaults::operators::BinaryAnd, WrapAsFirstArg>;
  using BinaryAndAssign = impl::defaults::BinaryOp<impl::defaults::operators::BinaryAndAssign, ReturnAsFirstArg>;
  using BinaryOr = impl::defaults::BinaryOp<impl::defaults::operators::BinaryOr, WrapAsFirstArg>;
  using BinaryOrAssign = impl::defaults::BinaryOp<impl::defaults::operators::BinaryOrAssign, ReturnAsFirstArg>;
  using Xor = impl::defaults::BinaryOp<impl::defaults::operators::Xor, WrapAsFirstArg>;
  using XorAssign = impl::defaults::BinaryOp<impl::defaults::operators::XorAssign, ReturnAsFirstArg>;
  using LeftShift = impl::defaults::BinaryOp<impl::defaults::operators::LeftShift, WrapAsFirstArg>;
  using LeftShiftAssign = impl::defaults::BinaryOp<impl::defaults::operators::LeftShiftAssign, ReturnAsFirstArg>;
  using RightShift = impl::defaults::BinaryOp<impl::defaults::operators::RightShift, WrapAsFirstArg>;
  using RightShiftAssign = impl::defaults::BinaryOp<impl::defaults::operators::RightShiftAssign, ReturnAsFirstArg>;
  using BinaryNot = impl::defaults::NullaryOp<impl::defaults::operators::BinaryNot, WrapAsFirstArg>;
  using UnaryPlus = impl::defaults::NullaryOp<impl::defaults::operators::UnaryPlus, WrapAsFirstArg>;
  using UnaryMinus = impl::defaults::NullaryOp<impl::defaults::operators::UnaryMinus, WrapAsFirstArg>;

  using Subscript = impl::defaults::UnaryOp<impl::defaults::operators::Subscript, NoWrapping>;

  using PreIncrement = impl::defaults::NullaryOp<impl::defaults::operators::PreIncrement, ReturnAsFirstArg>;
  using PreDecrement = impl::defaults::NullaryOp<impl::defaults::operators::PreDecrement, ReturnAsFirstArg>;
  using PostIncrement = impl::defaults::NullaryOp<impl::defaults::operators::PostIncrement, WrapAsFirstArg>;
  using PostDecrement = impl::defaults::NullaryOp<impl::defaults::operators::PostDecrement, WrapAsFirstArg>;

  using LogicalNot = impl::defaults::NullaryOp<impl::defaults::operators::LogicalNot, NoWrapping>;
  using LogicalAnd = impl::defaults::BinaryOp<impl::defaults::operators::LogicalAnd, NoWrapping>;
  using LogicalOr = impl::defaults::BinaryOp<impl::defaults::operators::LogicalOr, NoWrapping>;

  using Indirection = impl::defaults::NullaryOp<impl::defaults::operators::Indirection, NoWrapping>;
  using AddressOf = impl::defaults::NullaryOp<impl::defaults::operators::AddressOf, NoWrapping>;
  using MemberOfPointer = impl::defaults::NullaryOp<impl::defaults::operators::MemberOfPointer, NoWrapping>;
  //NYI using PointerToMemberOfPointer = impl::defaults::UnaryOp<impl::defaults::PointerToMemberOfPointer>;

  using FunctionCall = impl::defaults::VariadicOp<impl::defaults::operators::FunctionCall>;
  using Comma = impl::defaults::BinaryOp<impl::defaults::operators::Comma, ReturnAsSecondArg>;

  using Orderable = impl::NestedExpandTo<Less::Self<>, Greater::Self<>, LessEqual::Self<>, GreaterEqual::Self<>, Equal::Self<>, NotEqual::Self<>>;

  using Regular = impl::NestedExpandTo<Construct<>, Copy, Move, Orderable>;

  using Group = impl::NestedExpandTo<Add::Self<>, AddAssign::Self<>, Sub::Self<>, SubAssign::Self<>>;

  using Field = impl::NestedExpandTo<Add::Self<>, Sub::Self<>, Mult::Self<>, Div::Self<>, AddAssign::Self<>, SubAssign::Self<>, MultAssign::Self<>, DivAssign::Self<>>;

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
    using AllowConversionTo = First<Filter<AllowConversionToHelper<Target, Type>::template Value, GetFunctors<ConvertToEx, Options>>>;

    template<typename StrongValueArg>
    struct GetStrongValueInfo {
      using Type = StrongValueArg;
      using Options = List<>;
    };

    template<typename Type_, typename Options_>
    struct GetStrongValueInfo<StrongValue<Type_, Options_>> {
      using Type = Type_;
      using Options = Options_;
    };

    template<typename StrongValueArg>
    using GetType = typename GetStrongValueInfo<std::decay_t<StrongValueArg>>::Type;

    template<typename StrongValueArg>
    using GetOptions = typename GetStrongValueInfo<std::decay_t<StrongValueArg>>::Options;

    template<typename ...Args>
    struct GetMatchingFunctorHelper {
      template<typename Functor>
      using Value = std::is_invocable<Functor, Args...>;
    };

    template<template<typename> typename Option, typename ...Ts>
    using GetFunctor = First<Filter<GetMatchingFunctorHelper<Ts...>::template Value, GetFunctors<Option, Concat<GetOptions<Ts>...>>>>;

    template<typename Type, typename Options>
    struct StrongValue :
      private DisallowDestructor<Options>,
      private AllowCopy<Options>,
      private AllowMove<Options> {
    public:
      template<typename ...Args, typename = AllowConstructor<Construct<Args...>, Options>>
      explicit StrongValue(Args&&... args) :
        val_{ std::forward<Args>(args)... } {

      }

      StrongValue(StrongValue&& t) = default;

      StrongValue(StrongValue const& t) = default;

      StrongValue& operator=(StrongValue&& t) = default;

      StrongValue& operator=(StrongValue const& t) = default;

      template<typename T, typename Functor = GetFunctor<Assign::Ex, StrongValue&, T>>
      decltype(auto) operator=(T&& t) {
        return Functor{}(*this, std::forward<T>(t));
      }

      template<typename T, typename Functor = GetFunctor<Assign::Ex, StrongValue const&, T>>
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

      template<typename T, typename Functor = GetFunctor<Subscript::Ex, StrongValue&, T>>
      decltype(auto) operator[](T&& t) {
        return Functor{}(*this, std::forward<T>(t));
      }

      template<typename T, typename Functor = GetFunctor<Subscript::Ex, StrongValue const&, T>>
      decltype(auto) operator[](T&& t) const {
        return Functor{}(*this, std::forward<T>(t));
      }

      template<typename T = StrongValue & , typename Functor = GetFunctor<MemberOfPointer::Ex, T>>
      decltype(auto) operator->() {
        return Functor{}(*this);
      }

      template<typename T = StrongValue const&, typename Functor = GetFunctor<MemberOfPointer::Ex, T>>
      decltype(auto) operator->() const {
        return Functor{}(*this);
      }

      template<typename ...Args, typename Functor = GetFunctor<FunctionCall::Ex, StrongValue&, Args...>>
      decltype(auto) operator()(Args&&... args) {
        return Functor{}(*this, std::forward<Args>(args)...);
      }

      template<typename ...Args, typename Functor = GetFunctor<FunctionCall::Ex, StrongValue const&, Args...>>
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
      static StrongValue wrap(Args&&... args) {
        return StrongValue{ escapeHatch_, std::forward<Args>(args)... };
      }
    private:
      struct EscapeHatch {};

      static constexpr EscapeHatch escapeHatch_{};

      template<typename ...Args>
      StrongValue(EscapeHatch, Args&&... args) :
        val_{ std::forward<Args>(args)... } {

      }

      Type val_;
    };

    template<typename L, typename R, typename Functor = GetFunctor<Less::Ex, L, R>>
    decltype(auto) operator<(L&& l, R&& r) {
      return Functor{}(std::forward<L>(l), std::forward<R>(r));
    }

    template<typename L, typename R, typename Functor = GetFunctor<Greater::Ex, L, R>>
    decltype(auto) operator>(L&& l, R&& r) {
      return Functor{}(std::forward<L>(l), std::forward<R>(r));
    }

    template<typename L, typename R, typename Functor = GetFunctor<LessEqual::Ex, L, R>>
    decltype(auto) operator<=(L&& l, R&& r) {
      return Functor{}(std::forward<L>(l), std::forward<R>(r));
    }

    template<typename L, typename R, typename Functor = GetFunctor<GreaterEqual::Ex, L, R>>
    decltype(auto) operator>=(L&& l, R&& r) {
      return Functor{}(std::forward<L>(l), std::forward<R>(r));
    }

    template<typename L, typename R, typename Functor = GetFunctor<Equal::Ex, L, R>>
    decltype(auto) operator==(L&& l, R&& r) {
      return Functor{}(std::forward<L>(l), std::forward<R>(r));
    }

    template<typename L, typename R, typename Functor = GetFunctor<NotEqual::Ex, L, R>>
    decltype(auto) operator!=(L&& l, R&& r) {
      return Functor{}(std::forward<L>(l), std::forward<R>(r));
    }

    template<typename L, typename R, typename Functor = GetFunctor<Add::Ex, L, R>>
    decltype(auto) operator+(L&& l, R&& r) {
      return Functor{}(std::forward<L>(l), std::forward<R>(r));
    }

    template<typename L, typename R, typename Functor = GetFunctor<AddAssign::Ex, L, R>>
    decltype(auto) operator+=(L&& l, R&& r) {
      return Functor{}(std::forward<L>(l), std::forward<R>(r));
    }

    template<typename L, typename R, typename Functor = GetFunctor<Sub::Ex, L, R>>
    decltype(auto) operator-(L&& l, R&& r) {
      return Functor{}(std::forward<L>(l), std::forward<R>(r));
    }

    template<typename L, typename R, typename Functor = GetFunctor<SubAssign::Ex, L, R>>
    decltype(auto) operator-=(L&& l, R&& r) {
      return Functor{}(std::forward<L>(l), std::forward<R>(r));
    }

    template<typename L, typename R, typename Functor = GetFunctor<Mult::Ex, L, R>>
    decltype(auto) operator*(L&& l, R&& r) {
      return Functor{}(std::forward<L>(l), std::forward<R>(r));
    }

    template<typename L, typename R, typename Functor = GetFunctor<MultAssign::Ex, L, R>>
    decltype(auto) operator*=(L&& l, R&& r) {
      return Functor{}(std::forward<L>(l), std::forward<R>(r));
    }

    template<typename L, typename R, typename Functor = GetFunctor<Div::Ex, L, R>>
    decltype(auto) operator/(L&& l, R&& r) {
      return Functor{}(std::forward<L>(l), std::forward<R>(r));
    }

    template<typename L, typename R, typename Functor = GetFunctor<DivAssign::Ex, L, R>>
    decltype(auto) operator/=(L&& l, R&& r) {
      return Functor{}(std::forward<L>(l), std::forward<R>(r));
    }

    template<typename L, typename R, typename Functor = GetFunctor<Mod::Ex, L, R>>
    decltype(auto) operator%(L&& l, R&& r) {
      return Functor{}(std::forward<L>(l), std::forward<R>(r));
    }

    template<typename L, typename R, typename Functor = GetFunctor<ModAssign::Ex, L, R>>
    decltype(auto) operator%=(L&& l, R&& r) {
      return Functor{}(std::forward<L>(l), std::forward<R>(r));
    }

    template<typename L, typename R, typename Functor = GetFunctor<BinaryAnd::Ex, L, R>>
    decltype(auto) operator&(L&& l, R&& r) {
      return Functor{}(std::forward<L>(l), std::forward<R>(r));
    }

    template<typename L, typename R, typename Functor = GetFunctor<BinaryAndAssign::Ex, L, R>>
    decltype(auto) operator&=(L&& l, R&& r) {
      return Functor{}(std::forward<L>(l), std::forward<R>(r));
    }

    template<typename L, typename R, typename Functor = GetFunctor<BinaryOr::Ex, L, R>>
    decltype(auto) operator|(L&& l, R&& r) {
      return Functor{}(std::forward<L>(l), std::forward<R>(r));
    }

    template<typename L, typename R, typename Functor = GetFunctor<BinaryOrAssign::Ex, L, R>>
    decltype(auto) operator|=(L&& l, R&& r) {
      return Functor{}(std::forward<L>(l), std::forward<R>(r));
    }

    template<typename L, typename R, typename Functor = GetFunctor<Xor::Ex, L, R>>
    decltype(auto) operator^(L&& l, R&& r) {
      return Functor{}(std::forward<L>(l), std::forward<R>(r));
    }

    template<typename L, typename R, typename Functor = GetFunctor<XorAssign::Ex, L, R>>
    decltype(auto) operator^=(L&& l, R&& r) {
      return Functor{}(std::forward<L>(l), std::forward<R>(r));
    }

    template<typename L, typename R, typename Functor = GetFunctor<LeftShift::Ex, L, R>>
    decltype(auto) operator<<(L&& l, R&& r) {
      return Functor{}(std::forward<L>(l), std::forward<R>(r));
    }

    template<typename L, typename R, typename Functor = GetFunctor<LeftShiftAssign::Ex, L, R>>
    decltype(auto) operator<<=(L&& l, R&& r) {
      return Functor{}(std::forward<L>(l), std::forward<R>(r));
    }

    template<typename L, typename R, typename Functor = GetFunctor<RightShift::Ex, L, R>>
    decltype(auto) operator>>(L&& l, R&& r) {
      return Functor{}(std::forward<L>(l), std::forward<R>(r));
    }

    template<typename L, typename R, typename Functor = GetFunctor<RightShiftAssign::Ex, L, R>>
    decltype(auto) operator>>=(L&& l, R&& r) {
      return Functor{}(std::forward<L>(l), std::forward<R>(r));
    }

    template<typename L, typename Functor = GetFunctor<BinaryNot::Ex, L>>
    decltype(auto) operator~(L&& l) {
      return Functor{}(std::forward<L>(l));
    }

    template<typename L, typename Functor = GetFunctor<UnaryPlus::Ex, L>>
    decltype(auto) operator+(L&& l) {
      return Functor{}(std::forward<L>(l));
    }

    template<typename L, typename Functor = GetFunctor<UnaryMinus::Ex, L>>
    decltype(auto) operator-(L&& l) {
      return Functor{}(std::forward<L>(l));
    }

    template<typename L, typename Functor = GetFunctor<PreIncrement::Ex, L>>
    decltype(auto) operator++(L&& l) {
      return Functor{}(std::forward<L>(l));
    }

    template<typename L, typename Functor = GetFunctor<PreDecrement::Ex, L>>
    decltype(auto) operator--(L&& l) {
      return Functor{}(std::forward<L>(l));
    }

    template<typename L, typename Functor = GetFunctor<PostIncrement::Ex, L>>
    decltype(auto) operator++(L&& l, int) {
      return Functor{}(std::forward<L>(l));
    }

    template<typename L, typename Functor = GetFunctor<PostDecrement::Ex, L>>
    decltype(auto) operator--(L&& l, int) {
      return Functor{}(std::forward<L>(l));
    }

    template<typename L, typename Functor = GetFunctor<LogicalNot::Ex, L>>
    decltype(auto) operator!(L&& l) {
      return Functor{}(std::forward<L>(l));
    }

    template<typename L, typename R, typename Functor = GetFunctor<LogicalAnd::Ex, L, R>>
    decltype(auto) operator&&(L&& l, R&& r) {
      return Functor{}(std::forward<L>(l), std::forward<R>(r));
    }

    template<typename L, typename R, typename Functor = GetFunctor<LogicalOr::Ex, L, R>>
    decltype(auto) operator||(L&& l, R&& r) {
      return Functor{}(std::forward<L>(l), std::forward<R>(r));
    }

    template<typename L, typename Functor = GetFunctor<Indirection::Ex, L>>
    decltype(auto) operator*(L&& l) {
      return Functor{}(std::forward<L>(l));
    }

    template<typename L, typename Functor = GetFunctor<AddressOf::Ex, L>>
    decltype(auto) operator&(L&& l) {
      return Functor{}(std::forward<L>(l));
    }

    template<typename L, typename R, typename Functor = GetFunctor<Comma::Ex, L, R>>
    decltype(auto) operator,(L&& l, R&& r) {
      return Functor{}(std::forward<L>(l), std::forward<R>(r));
    }
  }

  template<typename Type, typename ...Options>
  using StrongValue = impl::StrongValue<Type, ExpandEx<impl::ExpandTo, Options...>>;
}