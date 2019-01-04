#pragma once

#include <dlib/args.hpp>
#include <dlib/interface.hpp>
#include <dlib/interface_function.hpp>
#include <dlib/outcome.hpp>
#include <unordered_map>

namespace dlib {

  template<typename T>
  struct Finder_get {
    Finder_get(T t) :
      val{ std::move(t) } {

    }
    T val;
  };

  template<typename T>
  struct Finder_contains {
    Finder_contains(T t) :
      val{ std::move(t) } {

    }
    T val;
  };

  template<typename T>
  struct Finder_default_get {
    using Me = T;

    template<typename ...Args>
    auto operator()(Me* ptr, Args&&... args) const noexcept {
      return ptr->get(std::forward<Args>(args)...);
    }
  };

  template<typename T>
  struct Finder_default_contains {
    using Me = T;

    template<typename ...Args>
    auto operator()(Me* ptr, Args&&... args) const noexcept {
      return ptr->contains(std::forward<Args>(args)...);
    }
  };

  template<typename Key, typename Value, typename Hash, typename Eq, typename Allocator>
  struct Finder_default_get<std::unordered_map<Key, Value, Hash, Eq, Allocator>> {
    using Me = std::unordered_map<Key, Value, Hash, Eq, Allocator>;

    Result<Value> operator()(Me* ptr,Key const& key) const noexcept {
      const auto found = ptr->find(key);
      if (found == ptr->end()) {
        return std::errc::bad_address;
      } else {
        return found->second;
      }
    }
  };

  template<typename Key, typename Value, typename Hash, typename Eq, typename Allocator>
  struct Finder_default_contains<std::unordered_map<Key, Value, Hash, Eq, Allocator>> {
    using Me = std::unordered_map<Key, Value, Hash, Eq, Allocator>;

    auto operator()(Me* ptr, Key const& key) const noexcept {
      return ptr->count(key) != 0;
    }
  };

  namespace finder_interface_impl {
    struct Get_tag;
    template<typename Key, typename Value>
    using Get = Interface_function<false, Value, Key const&>;
    template<typename Key, typename Value>
    using Get_function = typename Get<Key, Value>::Function;

    struct Contains_tag;
    template<typename Key>
    using Contains = Interface_function<false, bool, Key const&>;
    template<typename Key>
    using Contains_function = typename Contains<Key>::Function;

    template<typename Parent, typename Key, typename Value>
    class Finder_subinterface_impl {
    public:
      using Get_function = Get_function<Key, Value>;
      using Contains_function = Contains_function<Key>;

      Result<Value> get(Key const& key) noexcept {
        return get_(get_ptr(), key);
      }

      Result<bool> contains(Key const& key) noexcept {
        return contains_(get_ptr(), key);
      }

      void* get_ptr() noexcept {
        return static_cast<Parent*>(this)->get_ptr();
      }
      const void* get_ptr() const noexcept {
        return static_cast<const Parent*>(this)->get_ptr();
      }
    protected:
      template<typename Instance, typename ...Overrides>
      constexpr Finder_subinterface_impl(Type_arg<Instance>, Overrides&&... overrides) noexcept :
        get_{ type_arg<Instance>, get_get<Instance>(std::forward<Overrides>(overrides)...) },
        contains_{ type_arg<Instance>, get_contains<Instance>(std::forward<Overrides>(overrides)...) } {
      }

      template<typename Instance, typename ...Overrides>
      constexpr auto get_get(Overrides&&... overrides) noexcept {
        if constexpr (contains_varg<Finder_get, Overrides...>) {
          return get_varg<Finder_get>(std::forward<Overrides>(overrides)...).val;
        } else {
          return Finder_default_get<Instance>{};
        }
      }
      template<typename Instance, typename ...Overrides>
      constexpr auto get_contains(Overrides&&... overrides) noexcept {
        if constexpr (contains_varg<Finder_contains, Overrides...>) {
          return get_varg<Finder_contains>(std::forward<Overrides>(overrides)...).val;
        } else {
          return Finder_default_contains<Instance>{};
        }
      }
    private:
      finder_interface_impl::Get<Key, Value> get_;
      finder_interface_impl::Contains<Key> contains_;
    };
  }

  template<typename Key, typename Value>
  struct Finder_subinterface {
    template<typename Parent>
    using Type = finder_interface_impl::Finder_subinterface_impl<Parent, Key, Value>;
  };

  template<typename Key, typename Value>
  using Finder_interface = Interface<Finder_subinterface<Key, Value>::template Type>;
}