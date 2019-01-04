#pragma once

#include <utility>
#include <memory>
#include <dlib/meta.hpp>

namespace dlib {
  template<template<typename> typename ...Interfaces>
  class Interface :
    public Interfaces<Interface<Interfaces...>>... {
  public:
    template<typename Instance, typename ...Overrides>
    Interface(Instance instance, Overrides&&... overrides) :
      Interfaces<Interface<Interfaces...>>( Type_arg<Instance>{}, std::forward<Overrides>(overrides)...)...,
      ptr_(std::make_shared<Instance>(std::move(instance))) {

    }

    template<typename Instance, typename ...Overrides>
    Interface(std::shared_ptr<Instance> instance, Overrides&&... overrides) :
      Interfaces<Interface<Interfaces...>>{ Type_arg<Instance>{}, std::forward<Overrides>(overrides)... }...,
      ptr_(std::move(instance)) {

    }

    template<typename SubInterface>
    SubInterface& get_subinterface() noexcept {
      return *static_cast<SubInterface*>(this);
    }
    template<typename SubInterface>
    SubInterface const& get_subinterface() const noexcept {
      return *static_cast<const SubInterface*>(this);
    }

    template<typename T = void>
    T* get_ptr() noexcept {
      return reinterpret_cast<T*>(ptr_.get());
    }
    template<typename T = void>
    const T* get_ptr() const noexcept {
      return reinterpret_cast<const T*>(ptr_.get());
    }
  private:
    std::shared_ptr<void> ptr_;
  };
}