#pragma once

#include <utility>
#include <memory>
#include <dlib/meta.hpp>
#include <dlib/args.hpp>

namespace dlib {
  namespace interface_impl {
    template<typename Return, typename ...Args>
    using Function_ptr = Return(*)(Args...);

    template<typename Instance, typename Lambda, typename Return, typename ...Args>
    Return wrapper(void* instance_, Args... args) noexcept {
      Instance* instance = reinterpret_cast<Instance*>(instance_);
      return Lambda{}(instance, std::forward<Args>(args)...);
    }

    template<typename Instance, typename Lambda, typename Return, typename ...Args>
    Return const_wrapper(const void* instance_, Args... args) noexcept {
      const Instance* instance = reinterpret_cast<const Instance*>(instance_);
      return Lambda{}(instance, std::forward<Args>(args)...);
    }
  }

  template<template<typename> typename Default>
  using Make_interface_tag = Default<void>;

  template<typename Parent, bool is_const, typename Function, template<typename> typename Default, typename Tag>
  class Interface_function;
  
  template<typename Parent_, bool is_const_, typename Return_, typename ...Args_, template<typename> typename Default_, typename Tag_>
  class Interface_function<Parent_, is_const_, Return_(*)(Args_...), Default_, Tag_>
  {
  private:
    using Parent = Parent_;
    using Return = Return_;
    using Args_tuple = std::tuple<Args_...>;
    using Instance_ptr = std::conditional_t<is_const_, const void*, void*>;
    using Function = Return(*)(Instance_ptr, Args_...) noexcept;
    template<typename Instance>
    using Default = Default_<Instance>;
    using Tag = Tag_;
  public:
    template<typename Instance, typename ...Overrides>
    constexpr Interface_function(Type_arg<Instance>, Overrides&&... overrides) noexcept :
      wrapper_{ get_override_<Instance>(std::forward<Overrides>(overrides)...) } {
    }
  protected:
    template<typename ...Call_args, bool is_const = is_const_, typename = std::enable_if_t<!is_const>>
    constexpr Return call_(Call_args&&... call_args) noexcept {
      return call_impl_(
        static_cast<void*>(static_cast<Parent*>(this)->get_instance()),
        std::forward<Call_args>(call_args)...);
    }
    
    template<typename ...Call_args, bool is_const = is_const_, typename = std::enable_if_t<is_const>>
    constexpr Return call_(Call_args&&... call_args) const noexcept {
      return call_impl_(
        static_cast<const void*>(static_cast<const Parent*>(this)->get_instance()),
        std::forward<Call_args>(call_args)...);
    }
  private:
    template<typename ...Call_args>
    constexpr Return call_impl_(Instance_ptr instance, Call_args&&... call_args) noexcept {
      return wrapper_(instance, std::forward<Call_args>(call_args)...);
    }
    
    template<typename Instance, typename ...Overrides>
    constexpr static Function get_override_(Overrides&&... overrides) noexcept {
      using Lambda = Get_arg_defaulted<
        Arg<Tag>::template Holder,
        Default<Instance>,
        Overrides...>;
      if constexpr (is_const_) {
        return &interface_impl::const_wrapper<Instance, Lambda, Return, Args_...>;
      } else {
        return &interface_impl::wrapper<Instance, Lambda, Return, Args_...>;
      }
    }
    
    Function wrapper_;
  };

  template<typename ...Functions>
  class Subinterface :
    public Functions... {
  public:
    template<typename Instance, typename ...Overrides>
    constexpr Subinterface(Type_arg<Instance>, Overrides&&... overrides) noexcept :
      Functions{ type_arg<Instance>, std::forward<Overrides>(overrides)... }... {

    }
  };

  template<template<typename> typename ...Subinterfaces>
  class Interface :
    public Subinterfaces<Interface<Subinterfaces...>>... {
  public:

    template<typename Instance, typename ...Overrides>
    Interface(Instance instance, Overrides&&... overrides) :
      Subinterfaces<Interface>( type_arg<Instance>, std::forward<Overrides>(overrides)...)...,
      instance_(std::make_shared<Instance>(std::move(instance))) {

    }

    template<typename Instance, typename ...Overrides>
    Interface(std::shared_ptr<Instance> instance, Overrides&&... overrides) :
      Subinterfaces<Interface>{ type_arg<Instance>, std::forward<Overrides>(overrides)... }...,
      instance_(std::move(instance)) {

    }

    template<template<typename> typename Subinterface>
    Subinterface<Interface>& get_subinterface() noexcept {
      return *static_cast<Subinterface<Interface>*>(this);
    }
    template<template<typename> typename Subinterface>
    Subinterface<Interface> const& get_subinterface() const noexcept {
      return *static_cast<const Subinterface<Interface>*>(this);
    }

    template<typename T = void>
    T* get_instance() noexcept {
      return reinterpret_cast<T*>(instance_.get());
    }
    template<typename T = void>
    const T* get_instance() const noexcept {
      return reinterpret_cast<const T*>(instance_.get());
    }
  private:
    std::shared_ptr<void> instance_;
  };
}