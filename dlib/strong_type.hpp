#pragma once

#include <type_traits>
#include <dlib/utility.hpp>

namespace dlib::strongValue {

  namespace impl {
    template<typename ...>
    struct ExpandTo;

    template<typename ...Args>
    using NestedExpandTo = utility::ChangeContainer<ExpandTo, utility::ExpandEx<ExpandTo, Args...>>;

    template<typename Type, typename Options>
    struct StrongValue;

    template<typename T>
    struct IsStrongValue {
      static constexpr bool value = false;
    };

    template<typename Type, typename Options>
    struct IsStrongValue<StrongValue<Type, Options>> {
      static constexpr bool value = true;
    };

    template<typename T>
    constexpr bool isStrongValue = IsStrongValue<std::decay_t<T>>::value;

    template<typename T>
    struct StrongValueInfo;

    template<typename Type_, typename Options_>
    struct StrongValueInfo<StrongValue<Type_, Options_>> {
      using Type = Type_;
      using Options = Options_;
    };

    template<typename T>
    using StrongValueType = typename StrongValueInfo<std::decay_t<T>>::Type;

    template<typename T>
    using StrongValueOptions = typename StrongValueInfo<std::decay_t<T>>::Options;
    template<typename T>
    constexpr decltype(auto) unwrap(T&& t) noexcept {
      if constexpr(isStrongValue<T>) {
        return t.get();
      } else {
        return std::forward<T>(t);
      }
    }

    template<typename T>
    using UnWrap = decltype(unwrap(std::declval<T>()));

    namespace defaults {

      template<typename To>
      struct Converter {
        template<typename From, typename = decltype(static_cast<To>(std::declval<From>()))>
        To operator()(From&& t) {
          return static_cast<To>(t);
        }
      };

      struct Less {
        template<typename L, typename R, typename = decltype(std::declval<L>() < std::declval<R>())>
            constexpr decltype(auto) operator()(L&& l, R&& r) const {
              return l < r;
            }
      };

      struct Greater {
        template<typename L, typename R, typename = decltype(std::declval<L>() > std::declval<R>())>
                  constexpr decltype(auto) operator()(L&& l, R&& r) const {
                    return l > r;
                  }
      };

      struct LessEqual {
        template<typename L, typename R, typename = decltype(std::declval<L>() <= std::declval<R>())>
        constexpr decltype(auto) operator()(L&& l, R&& r) const {
          return  l <= r;
        }
      };

      struct GreaterEqual {
        template<typename L, typename R, typename = decltype(std::declval<L>() >= std::declval<R>())>
        constexpr decltype(auto) operator()(L&& l, R&& r) const {
          return l >= r;
        }
      };

      struct Equal {
        template<typename L, typename R, typename = decltype(std::declval<L>() == std::declval<R>())>
        constexpr decltype(auto) operator()(L&& l, R&& r) const {
          return l == r;
        }
      };

      struct NotEqual {
        template<typename L, typename R, typename = decltype(std::declval<L>() != std::declval<R>())>
        constexpr decltype(auto) operator()(L&& l, R&& r) const {
          return l != r;
        }
      };

      struct Assign {
        template<typename L, typename R, typename = decltype(std::declval<L>() = std::declval<R>())>
        constexpr decltype(auto) operator()(L&&l, R&& r) const {
          return l = r;
        }
      };

      struct Add {
        template<typename L, typename R, typename = decltype(std::declval<L>() + std::declval<R>())>
        constexpr decltype(auto) operator()(L&& l, R&& r) const {
          return l + r;
        }
      };

      struct AddAssign {
        template<typename L, typename R, typename = decltype(std::declval<L>() += std::declval<R>())>
        constexpr decltype(auto) operator()(L&&l, R&& r) const {
          return l += r;
        }
      };

      struct Sub {
        template<typename L, typename R, typename = decltype(std::declval<L>() - std::declval<R>())>
        constexpr decltype(auto) operator()(L&& l, R&& r) const {
          return l - r;
        }
      };

      struct SubAssign {
        template<typename L, typename R, typename = decltype(std::declval<L>() -= std::declval<R>())>
        constexpr decltype(auto) operator()(L&&l, R&& r) const {
          return l -= r;
        }
      };

      struct Mult {
        template<typename L, typename R, typename = decltype(std::declval<L>() * std::declval<R>())>
        constexpr decltype(auto) operator()(L&& l, R&& r) const {
          return l * r;
        }
      };

      struct MultAssign {
        template<typename L, typename R, typename = decltype(std::declval<L>() *= std::declval<R>())>
        constexpr decltype(auto) operator()(L&&l, R&& r) const {
          return l *= r;
        }
      };

      struct Div {
        template<typename L, typename R, typename = decltype(std::declval<L>() / std::declval<R>())>
        constexpr decltype(auto) operator()(L&& l, R&& r) const {
          return l / r;
        }
      };

      struct DivAssign {
        template<typename L, typename R, typename = decltype(std::declval<L>() /= std::declval<R>())>
        constexpr decltype(auto) operator()(L&&l, R&& r) const {
          return l /= r;
        }
      };

      struct Mod {
        template<typename L, typename R, typename = decltype(std::declval<L>() % std::declval<R>())>
        constexpr decltype(auto) operator()(L&& l, R&& r) const {
          return l % r;
        }
      };

      struct ModAssign {
        template<typename L, typename R, typename = decltype(std::declval<L>() %= std::declval<R>())>
        constexpr decltype(auto) operator()(L&&l, R&& r) const {
          return l %= r;
        }
      };

      struct BinaryAnd {
        template<typename L, typename R, typename = decltype(std::declval<L>() & std::declval<R>())>
        constexpr decltype(auto) operator()(L&& l, R&& r) const {
          return l & r;
        }
      };

      struct BinaryAndAssign {
        template<typename L, typename R, typename = decltype(std::declval<L>() &= std::declval<R>())>
        constexpr decltype(auto) operator()(L&&l, R&& r) const {
          return l &= r;
        }
      };

      struct BinaryOr {
        template<typename L, typename R, typename = decltype(std::declval<L>() | std::declval<R>())>
        constexpr decltype(auto) operator()(L&& l, R&& r) const {
          return l | r;
        }
      };

      struct BinaryOrAssign {
        template<typename L, typename R, typename = decltype(std::declval<L>() |= std::declval<R>())>
        constexpr decltype(auto) operator()(L&&l, R&& r) const {
          return l |= r;
        }
      };

      struct Xor {
        template<typename L, typename R, typename = decltype(std::declval<L>() ^ std::declval<R>())>
        constexpr decltype(auto) operator()(L&& l, R&& r) const {
          return l ^ r;
        }
      };

      struct XorAssign {
        template<typename L, typename R, typename = decltype(std::declval<L>() ^= std::declval<R>())>
        constexpr decltype(auto) operator()(L&&l, R&& r) const {
          return l ^= r;
        }
      };

      struct LeftShift {
        template<typename L, typename R, typename = decltype(std::declval<L>() << std::declval<R>())>
        constexpr decltype(auto) operator()(L&& l, R&& r) const {
          return l << r;
        }
      };

      struct LeftShiftAssign {
        template<typename L, typename R, typename = decltype(std::declval<L>() <<= std::declval<R>())>
        constexpr decltype(auto) operator()(L&&l, R&& r) const {
          return l <<= r;
        }
      };

      struct RightShift {
        template<typename L, typename R, typename = decltype(std::declval<L>() >> std::declval<R>())>
        constexpr decltype(auto) operator()(L&& l, R&& r) const {
          return l >> r;
        }
      };

      struct RightShiftAssign {
        template<typename L, typename R, typename = decltype(std::declval<L>() >>= std::declval<R>())>
        constexpr decltype(auto) operator()(L&&l, R&& r) const {
          return l >>= r;
        }
      };

      struct BinaryNot {
        template<typename L, typename = decltype(~std::declval<L>())>
        constexpr decltype(auto) operator()(L&&l) const {
          return ~l;
        }
      };

      struct UnaryPlus {
        template<typename L, typename = decltype(+std::declval<L>())>
        constexpr decltype(auto) operator()(L&&l) const {
          return +l;
        }
      };

      struct UnaryMinus {
        template<typename L, typename = decltype(-std::declval<L>())>
        constexpr decltype(auto) operator()(L&&l) const {
          return -l;
        }
      };

      struct Subscript {
        template<typename L, typename R, typename = decltype(std::declval<L>()[std::declval<R>()])>
        constexpr decltype(auto) operator()(L&& l, R && r) const {
          return l[r];
        }
      };

      struct Indirection {
        template<typename L, typename = decltype(*std::declval<L>())>
        constexpr decltype(auto) operator()(L&& l) const {
          return *l;
        }
      };

      struct PreIncrement {
        template<typename L, typename = decltype(++std::declval<L>())>
        constexpr decltype(auto) operator()(L&&l) const {
          return ++l;
        }
      };

      struct PreDecrement {
        template<typename L, typename = decltype(--std::declval<L>())>
        constexpr decltype(auto) operator()(L&&l) const {
          return --l;
        }
      };

      struct PostIncrement {
        template<typename L, typename = decltype(std::declval<L>()++)>
        constexpr decltype(auto) operator()(L&&l) const {
          return l++;
        }
      };

      struct PostDecrement {
        template<typename L, typename = decltype(std::declval<L>()--)>
        constexpr decltype(auto) operator()(L&&l) const {
          return l--;
        }
      };

      struct LogicalNot {
        template<typename L, typename = decltype(!std::declval<L>())>
        constexpr decltype(auto) operator()(L&& l) const {
          return !l;
        }
      };

      struct LogicalAnd {
        template<typename L, typename R, typename = decltype(std::declval<L>() && std::declval<R>())>
        constexpr decltype(auto) operator()(L&& l, R&& r) const {
          return l && r;
        }
      };

      struct LogicalOr {
        template<typename L, typename R, typename = decltype(std::declval<L>() || std::declval<R>())>
        constexpr decltype(auto) operator()(L&& l, R&& r) const noexcept(std::declval<L>() || std::declval<R>()) {
          return l || r;
        }
      };

      struct AddressOf {
        template<typename L, typename = decltype(&std::declval<L>())>
        constexpr decltype(auto) operator()(L&& l) const {
          return &l;
        }
      };

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

      struct FunctionCall {
        template<typename L, typename ...Rest, typename = std::enable_if_t<std::is_invocable_v<L, Rest...>>>
        constexpr decltype(auto) operator()(L&& l, Rest&&... rest) const {
          return l(std::forward<Rest>(rest)...);
        }
      };

      struct Comma {
        template<typename L, typename R, typename = decltype(std::declval<L>(), std::declval<R>())>
        constexpr decltype(auto) operator()(L&& l, R&& r) {
          return l, r;
        }
      };

      template<typename Operator, typename Right>
      struct LeftOfImpl {
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

      template<typename Operator, typename Left>
      struct RightOfImpl {
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

      template<typename Operator>
      struct SelfImpl {
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

      template<typename Operator>
      struct NullaryImpl {
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

      template<typename Operator, typename ...Args>
      struct VariadicImpl {
      private:
        template<typename L, typename ...GivenArgs>
        static constexpr bool enabled_ = isStrongValue<L>
          && std::is_same_v<utility::List<std::decay_t<Args>...>, utility::List<std::decay_t<GivenArgs>...>>
          && std::is_invocable_v<Operator, Args...>;
      public:
        template<typename L, typename ...GivenArgs, typename = std::enable_if_t<enabled_<L, GivenArgs...>>>
        constexpr decltype(auto) operator()(L&& l, GivenArgs&&... rest) const {
          return Operator{}(std::forward<L>(l), std::forward<GivenArgs>(rest)...);
        }
      };
      template<typename Nested>
      struct NullWrappingImpl {
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

      using NullWrapping = NullWrappingImpl<utility::Placeholder>;

      template<size_t i>
      struct IthWrappingImpl {
        template<typename Nested>
        struct Impl {
          template<typename ...Args>
          constexpr decltype(auto) operator()(Args&&... args) const {
            return utility::GetEx<i, std::decay_t<Args>...>::wrap(Nested{}(std::forward<Args>(args)...));
          }
        };
      };

      template<size_t i>
      using IthWrapping = typename IthWrappingImpl<i>::template Impl<utility::Placeholder>;

      template<typename Wrapping>
      struct StaticWrappingImpl {
        template<typename Nested>
        struct Impl {
          template<typename ...Args>
          constexpr decltype(auto) operator()(Args&&... args) const {
            return Wrapping::wrap(Nested{}(std::forward<Args>(args)...));
          }
        };
      };

      template<typename Wrapping>
      using StaticWrapping = typename StaticWrappingImpl<Wrapping>::template Impl<utility::Placeholder>;

      template<size_t i>
      struct IthReferenceWrappingImpl {
        template<typename Nested>
        struct Impl {
          template<typename ...Args>
          constexpr decltype(auto) operator()(Args&&... args) {
            Nested{}(std::forward<Args>(args)...);
            return getIth<i>(std::forward<Args>(args)...);
          }

          template<size_t i, typename First, typename ...Rest>
          constexpr decltype(auto) getIth(First&& first, Rest&&... rest) {
            if constexpr(i == 0) {
              return std::forward<First>(first);
            } else {
              return getIth<i - 1>(std::forward<Rest>(rest)...);
            }
          }
        };
      };

      template<size_t i>
      using IthReferenceWrapping = typename IthReferenceWrappingImpl<i>::template Impl<utility::Placeholder>;

      template<typename Operator, typename DefaultWrapping>
      struct NullaryOp {
        template<typename Functor>
        struct Ex;

        template<typename Wrapping = DefaultWrapping>
        using Using = Ex<utility::ChangeContainedEx<Wrapping, NullaryImpl<Operator>>>;
      };

      template<typename Operator, typename DefaultWrapping>
      struct UnaryOp {
        template<typename Functor>
        struct Ex;

        template<typename Arg, typename Wrapping = DefaultWrapping>
        using Using = Ex<utility::ChangeContainedEx<Wrapping, RightOfImpl<Operator, Arg>>>;
      };

      template<typename Operator, typename DefaultWrapping>
      struct BinaryOp {
        template<typename Functor>
        struct Ex;

        template<typename Right, typename Wrapping = DefaultWrapping>
        using LeftOf = Ex<utility::ChangeContainedEx<Wrapping, LeftOfImpl<Operator, Right>>>;

        template<typename Left, typename Wrapping = DefaultWrapping>
        using RightOf = Ex<utility::ChangeContainedEx<Wrapping, RightOfImpl<Operator, Left>>>;

        template<typename Wrapping = DefaultWrapping>
        using Self = Ex<utility::ChangeContainedEx<Wrapping, SelfImpl<Operator>>>;
      };

      template<typename Operator>
      struct VariadicOp {
        template<typename Functor>
        struct Ex;

        template<typename ...Args>
        using Using = Ex <VariadicImpl<Operator, Args...>>;
      };
    }
  }

  struct Copy;
  struct Move;
  template<typename ...Types>
  struct Construct;

  struct NoDestructor;

  template<typename Functor>
  struct ConvertToEx;

  template<typename To>
  using ConvertTo = ConvertToEx<impl::defaults::Converter<To>>;

  using NoWrap = impl::defaults::NullWrapping;
  template<typename T>
  using WrapAs = impl::defaults::StaticWrapping<T>;
  template<size_t i>
  using WrapAsIth = impl::defaults::IthWrapping<i>;
  using WrapAsFirst = WrapAsIth<0>;
  using WrapAsSecond = WrapAsIth<1>;
  template<size_t i>
  using WrapAsIthReference = impl::defaults::IthReferenceWrapping<i>;
  using WrapAsFirstReference = WrapAsIthReference<0>;
  using WrapAsSecondReference = WrapAsIthReference<1>;

  using Less = impl::defaults::BinaryOp<impl::defaults::Less, NoWrap>;
  using Greater = impl::defaults::BinaryOp<impl::defaults::Greater, NoWrap>;
  using LessEqual = impl::defaults::BinaryOp<impl::defaults::LessEqual, NoWrap>;
  using GreaterEqual = impl::defaults::BinaryOp<impl::defaults::GreaterEqual, NoWrap>;
  using Equal = impl::defaults::BinaryOp<impl::defaults::Equal, NoWrap>;
  using NotEqual = impl::defaults::BinaryOp<impl::defaults::NotEqual, NoWrap>;

  using Assign = impl::defaults::UnaryOp<impl::defaults::Assign, WrapAsFirst>;

  using Add = impl::defaults::BinaryOp<impl::defaults::Add, WrapAsFirst>;
  using AddAssign = impl::defaults::BinaryOp<impl::defaults::AddAssign, WrapAsFirstReference>;
  using Sub = impl::defaults::BinaryOp<impl::defaults::Sub, WrapAsFirst>;
  using SubAssign = impl::defaults::BinaryOp<impl::defaults::SubAssign, WrapAsFirstReference>;
  using Mult = impl::defaults::BinaryOp<impl::defaults::Mult, WrapAsFirst>;
  using MultAssign = impl::defaults::BinaryOp<impl::defaults::MultAssign, WrapAsFirstReference>;
  using Div = impl::defaults::BinaryOp<impl::defaults::Div, WrapAsFirst>;
  using DivAssign = impl::defaults::BinaryOp<impl::defaults::DivAssign, WrapAsFirstReference>;
  using Mod = impl::defaults::BinaryOp<impl::defaults::Mod, WrapAsFirst>;
  using ModAssign = impl::defaults::BinaryOp<impl::defaults::ModAssign, WrapAsFirstReference>;
  using BinaryAnd = impl::defaults::BinaryOp<impl::defaults::BinaryAnd, WrapAsFirst>;
  using BinaryAndAssign = impl::defaults::BinaryOp<impl::defaults::BinaryAndAssign, WrapAsFirstReference>;
  using BinaryOr = impl::defaults::BinaryOp<impl::defaults::BinaryOr, WrapAsFirst>;
  using BinaryOrAssign = impl::defaults::BinaryOp<impl::defaults::BinaryOrAssign, WrapAsFirstReference>;
  using Xor = impl::defaults::BinaryOp<impl::defaults::Xor, WrapAsFirst>;
  using XorAssign = impl::defaults::BinaryOp<impl::defaults::XorAssign, WrapAsFirstReference>;
  using LeftShift = impl::defaults::BinaryOp<impl::defaults::LeftShift, WrapAsFirst>;
  using LeftShiftAssign = impl::defaults::BinaryOp<impl::defaults::LeftShiftAssign, WrapAsFirstReference>;
  using RightShift = impl::defaults::BinaryOp<impl::defaults::RightShift, WrapAsFirst>;
  using RightShiftAssign = impl::defaults::BinaryOp<impl::defaults::RightShiftAssign, WrapAsFirstReference>;
  using BinaryNot = impl::defaults::NullaryOp<impl::defaults::BinaryNot, WrapAsFirst>;
  using UnaryPlus = impl::defaults::NullaryOp<impl::defaults::UnaryPlus, WrapAsFirst>;
  using UnaryMinus = impl::defaults::NullaryOp<impl::defaults::UnaryMinus, WrapAsFirst>;

  using Subscript = impl::defaults::UnaryOp<impl::defaults::Subscript, NoWrap>;

  using PreIncrement = impl::defaults::NullaryOp<impl::defaults::PreIncrement, WrapAsFirstReference>;
  using PreDecrement = impl::defaults::NullaryOp<impl::defaults::PreDecrement, WrapAsFirstReference>;
  using PostIncrement = impl::defaults::NullaryOp<impl::defaults::PostIncrement, WrapAsFirst>;
  using PostDecrement = impl::defaults::NullaryOp<impl::defaults::PostDecrement, WrapAsFirst>;

  using LogicalNot = impl::defaults::NullaryOp<impl::defaults::LogicalNot, NoWrap>;
  using LogicalAnd = impl::defaults::BinaryOp<impl::defaults::LogicalAnd, NoWrap>;
  using LogicalOr = impl::defaults::BinaryOp<impl::defaults::LogicalOr, NoWrap>;

  using Indirection = impl::defaults::NullaryOp<impl::defaults::Indirection, NoWrap>;
  using AddressOf = impl::defaults::NullaryOp<impl::defaults::AddressOf, NoWrap>;
  using MemberOfPointer = impl::defaults::NullaryOp<impl::defaults::MemberOfPointer, NoWrap>;
  //NYI using PointerToMemberOfPointer = impl::defaults::UnaryOp<impl::defaults::PointerToMemberOfPointer>;

  using FunctionCall = impl::defaults::VariadicOp<impl::defaults::FunctionCall>;
  using Comma = impl::defaults::BinaryOp<impl::defaults::Comma, WrapAsSecondReference>;

  using Orderable = impl::NestedExpandTo<Less::Self<>, Greater::Self<>, LessEqual::Self<>, GreaterEqual::Self<>, Equal::Self<>, NotEqual::Self<>>;

  using Regular = impl::NestedExpandTo<Construct<>, Copy, Move, Orderable>;  

  namespace impl {

    template<typename ArgsConstructable, typename Options>
    using AllowConstructor = std::enable_if_t <utility::contains<ArgsConstructable, Options>>;

    template<bool>
    struct DisallowDestructorImpl;

    template<>
    struct DisallowDestructorImpl<false> {

    };

    template<>
    struct DisallowDestructorImpl<true> {
      ~DisallowDestructorImpl() = delete;
    };

    template<typename Options>
    using DisallowDestructor =
      DisallowDestructorImpl<utility::contains<NoDestructor, Options>>;

    template<bool>
    struct AllowCopyImpl;

    template<>
    struct AllowCopyImpl<true> {
    };

    template<>
    struct AllowCopyImpl<false> {
      AllowCopyImpl() = default;
      AllowCopyImpl(AllowCopyImpl const&) = delete;
      AllowCopyImpl(AllowCopyImpl&&) = default;

      AllowCopyImpl& operator=(AllowCopyImpl const&) = delete;
      AllowCopyImpl& operator=(AllowCopyImpl&&) = default;
    };

    template<typename Options>
    using AllowCopy = AllowCopyImpl<utility::contains<Copy, Options>>;

    template<bool>
    struct AllowMoveImpl;

    template<>
    struct AllowMoveImpl<true> {
    };

    template<>
    struct AllowMoveImpl<false> {
      AllowMoveImpl() = default;
      AllowMoveImpl(AllowMoveImpl const&) = default;
      AllowMoveImpl(AllowMoveImpl&&) = delete;

      AllowMoveImpl& operator=(AllowMoveImpl const&) = default;
      AllowMoveImpl& operator=(AllowMoveImpl&&) = delete;
    };

    template<typename Options>
    using AllowMove = AllowMoveImpl<utility::contains<Move, Options>>;

    template<template<typename> typename Option>
    struct GetFunctorsHelper {
      template<typename T>
      using Value = utility::ToIntegralConstant<utility::isWrappedBy<Option, T>>;
    };

    template<template<typename> typename Option, typename Options>
    using GetFunctors = utility::Transform<
      utility::First,
      utility::Filter<GetFunctorsHelper<Option>::template Value, Options>>;

    template<typename Target, typename Type>
    struct AllowConversionToHelper {
      template<typename T>
      using Value = std::is_same<
        std::invoke_result_t<T, Type>,
        Target>;
    };

    template<typename Target, typename Type, typename Options>
    using AllowConversionTo = utility::First<utility::Filter<AllowConversionToHelper<Target, Type>::template Value, GetFunctors<ConvertToEx, Options>>>;

    template<typename StrongValueArg>
    struct GetStrongValueInfo {
      using Type = StrongValueArg;
      using Options = utility::List<>;
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
    using GetFunctor = utility::First<utility::Filter<GetMatchingFunctorHelper<Ts...>::template Value, GetFunctors<Option, utility::Concat<GetOptions<Ts>...>>>>;

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
  using StrongValue = impl::StrongValue<Type, utility::ExpandEx<impl::ExpandTo, Options...>>;
}