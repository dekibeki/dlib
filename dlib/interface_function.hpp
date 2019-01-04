#pragma once

#include <dlib/outcome.hpp>
#include <dlib/meta.hpp>
#include <type_traits>
#include <functional>

namespace dlib {
  namespace interface_function_impl {
    template<typename Return, typename ...Args>
    using Function_ptr = Return(*)(Args...);

    template<typename Instance, typename Lambda, typename Return, typename ...Args>
    Return wrapper(void* instance_, Args... args) noexcept {
      Instance* instance = reinterpret_cast<Instance*>(instance_);
      return Lambda{}(instance, std::move(args)...);
    }

    template<typename Instance, typename Lambda, typename Return, typename ...Args>
    Return wrapper(const void* instance_, Args... args) noexcept {
      const Instance* instance = reinterpret_cast<const Instance*>(instance_);
      return Lambda{}(instance, std::move(args)...);
    }
  }

  template<bool is_const, typename Return_, typename ...Args_>
  class Interface_function final {
  public:
    using Return = Result<Return_>;
    using Args_tuple = std::tuple<Args_...>;
  private:
    using Instance_ptr = std::conditional_t<is_const, const void*, void*>;
  public:
    using Function = Return(*)(Instance_ptr, Args_...) noexcept;

    template<typename Instance, typename T>
    Interface_function(Type_arg<Instance>, T) noexcept {
      static_assert(!std::is_pointer_v<T>, "wrap functions in FunctionArgs");
      wrapper_ = &interface_function_impl::wrapper<Instance, T, Return, Args_...>;
    }

    template<typename = std::enable_if_t<is_const>>
    Return operator()(Instance_ptr instance, Args_... args) const noexcept {
      return wrapper_(instance, std::move(args)...);
    }

    Return operator()(Instance_ptr instance, Args_... args) noexcept {
      return wrapper_(instance, std::move(args)...);
    }
  private:
    interface_function_impl::Function_ptr<Return, Instance_ptr, Args_...> wrapper_;
  };

}