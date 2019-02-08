#pragma once

#include <memory>
#include <type_traits>
#include <utility>
#include <dlib/outcome.hpp>

namespace dlib {
  namespace pointer_to_impl {
    struct Make {};

    constexpr Make make;
  }

  struct Raw_pointer final {
    template<typename T>
    using Pointer_to = T*;

    class Pointer_from {
    public:
      template<typename T>
      static Pointer_to<T> get_pointer_to(T* t) noexcept {
        return Pointer_to<T>(t);
      }
    };

    template<typename T>
    struct Holder final {
    public:
      Holder() = delete;
      Holder(T t) noexcept :
        holding_{ std::move(t) } {

      }

      Holder(Holder&&) = default;
      Holder(Holder const& me) = default;

      Holder& operator=(Holder&&) = default;
      Holder& operator=(Holder const&) = default;
      
      constexpr Pointer_to<T> get_pointer_to() noexcept {
        return Pointer_to<T>(&holding_);
      }

      constexpr Pointer_to<const T> get_pointer_to() const noexcept {
        return Pointer_to<const T>(&holding_);
      }

      constexpr T* operator->() noexcept {
        return &holding_;
      }

      constexpr const T* operator->() const noexcept {
        return &holding_;
      }

      constexpr T& operator*() noexcept {
        return holding_;
      }

      constexpr T const& operator*() const noexcept {
        return holding_;
      }
    private:
      T holding_;
    };
  };

  struct Shared_pointer final {
    template<typename T>
    using Pointer_to = std::shared_ptr<T>;

    class Pointer_from :
      private std::enable_shared_from_this<Pointer_from> {
    public:
      template<typename T>
      Pointer_to<T> get_pointer_to(T* t) noexcept {
        return std::shared_ptr<T>(shared_from_this(),t);
      }
    };
  
    template<typename T>
    struct Holder final {
    public:
      Holder(T t) :
        holding_(std::make_shared<T>(std::move(t))) {

      }
      Holder(Holder const&) = default;
      Holder(Holder&&) = default;

      Holder& operator=(Holder&&) = default;
      Holder& operator=(Holder const&) = default;

      constexpr Pointer_to<T> get_pointer_to() noexcept {
        return Pointer_to<T>(holding_);
      }

      constexpr Pointer_to<const T> get_pointer_to() const noexcept {
        return Pointer_to<const T>(holding_);
      }

      T* operator->() noexcept {
        return holding_.operator->();
      }

      const T* operator->() const noexcept {
        return holding_.operator->();
      }

      T& operator*() noexcept {
        return holder_.operator*();
      }

      T const& operator*() const noexcept {
        return holder_.operator*();
      }
    private:   
      std::shared_ptr<T> holding_;
    };
  };

  struct Unique_pointer final {
    template<typename T>
    using Pointer_to = std::unique_ptr<T>;

    template<typename T>
    struct Holder final {
    public:
      Holder(T t) :
        holding_(std::make_unique<T>(std::move(t))) {

      }

      constexpr Pointer_to<T>& get_pointer_to() noexcept {
        return holding_;
      }
      constexpr Pointer_to<const T>& get_poiter_to() noexcept {
        return holding_;
      }

      T* operator->() noexcept {
        return holding_.operator->();
      }

      const T* operator->() const noexcept {
        return holding_.operator->();
      }

      T& operator*() noexcept {
        return holding_.operator*();
      }

      T const& operator*() const noexcept {
        return holding_.operator*();
      }
    private:
      std::unique_ptr<T> holding_;
    };
  };

  template<typename>
  struct Pointer_is {};

  template<typename Pointer_arg, typename T>
  using Get_pointer_to = typename Pointer_arg::template Pointer_to<T>;

  template<typename Pointer_arg>
  using Get_pointer_from = typename Pointer_arg::Pointer_from;

  template<typename Pointer_arg, typename T>
  using Get_holder = typename Pointer_arg::template Holder<T>;

  template<typename T>
  using Pointer = typename T::Pointer_to;

  template<typename T>
  using Holder = typename T::Holder;

  template<typename T>
  struct Make {
    constexpr Make() noexcept {}

    template<typename ...Args>
      constexpr Result<Holder<T>> operator()(Args&&... args) const noexcept {
      if constexpr (is_result<decltype(T::make(std::declval<Args>()...))>) {
        DLIB_TRY(making, (T::make(std::forward<Args>(args)...)));
        return Holder<T>{std::move(making)};
      } else {
        return Holder<T>{T::make(std::forward<Args>(args)...)};
      }
    }
  };

  template<typename T>
  constexpr Make<T> make{};
}