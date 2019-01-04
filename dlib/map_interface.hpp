#pragma once

#include <dlib/args.hpp>
#include <dlib/meta.hpp>
#include <dlib/interface.hpp>
#include <dlib/outcome.hpp>
#include <functional>

namespace dlib {

  namespace map_interface_impl {
    struct Get_tag;
    struct Contains_tag;

    struct Set_tag;
    struct Erase_tag;
    
    template<typename Key, typename Value>
    using Get_function = Result<Value> (*)(void*, Key const&) noexcept;
    template<typename Key, typename Value>
    using Set_function = Result<Value> (*)(void*, Key const&, Value) noexcept;
    template<typename Key>
    using Erase_function = Result<void> (*)(void*, Key const&) noexcept;
    template<typename Key>
    using Contains_function = Result<bool> (*)(const void*, Key const&) noexcept;

    template<typename Instance, typename Key, typename Value>
    Result<Value> default_get(void* ptr, Key const& key) noexcept {
      return reinterpret_cast<Instance*>(ptr)->get(key);
    };

    template<typename Instance, typename Key, typename Value>
    Result<Value> default_set(void* ptr, Key const& key, Value value) noexcept {
      return reinterpret_cast<Instance*>(ptr)->set(key, std::move(value));
    }

    template<typename Instance, typename Key>
    Result<void> default_erase(void* ptr, Key const& key) noexcept {      
      if constexpr (std::is_same_v<void, decltype(reinterpret_cast<Instance*>(ptr)->erase(key))>) {
        reinterpret_cast<Instance*>(ptr)->erase(key);
        return success();
      } else {
        return reinterpret_cast<Instance*>(ptr)->erase(key);
      }
    }

    template<typename Instance, typename Key>
    Result<bool> default_contains(const void* ptr, Key const& key) noexcept {
      return reinterpret_cast<const Instance*>(ptr)->contains(key);
    }

    template<typename Parent, typename Key, typename Value>
    class Map_subinterface_impl {
    public:
      using Set_function = Set_function<Key, Value>;
      using Erase_function = Erase_function<Key>;

      using Get = Arg<Get_function, Get_tag>;
      using Set = Arg<Set_function, Set_tag>;
      using Erase = Arg<Erase_function, Erase_tag>;
      using Contains = Arg<Contains_function, Contains_tag>;

      Result<Value> get(Key const& key) noexcept {
        return get_(get_ptr(), key);
      }

      Result<Value> set(Key const& key, Value value) noexcept {
        return set_(get_ptr(), key, std::move(value));
      }

      Result<void> erase(Key const& key) noexcept {
        return erase_(get_ptr(), key);
      }

      Result<bool> contains(Key const& key) const noexcept {
        return contains_(get_ptr(), key);
      }

      void* get_ptr() noexcept {
        return static_cast<Parent*>(this)->get_ptr();
      }

      const void* get_ptr() const noexcept {
        return static_cast<const Parent*>(this)->get_ptr();
      }
    protected:
      template<typename Instance, typename... Overrides>
      constexpr Map_subinterface_impl(Type_arg<Instance>,Overrides&&... overrides) noexcept {
        if constexpr (contains_arg<Get, Overrides...>) {
          get_ = get_arg<Get>(std::forward<Overrides>(overrides)...).val;
        } else {
          get_ = &default_get<Instance, Key, Value>;
        }

        if constexpr (contains_arg<Set, Overrides...>) {
          set_ = get_arg<Set>(std::forward<Overrides>(overrides)...).val;
        } else {
          set_ = &default_set<Instance, Key, Value>;
        }

        if constexpr (contains_arg<Erase, Overrides...>) {
          erase_ = get_arg<Erase>(std::forward<Overrides>(overrides)...).val;
        } else {
          erase_ = &default_erase<Instance, Key>;
        }

        if constexpr (contains_arg<Contains, Overrides...>) {
          contains_ = get_arg<Contains>(std::forward<Overrides>(overrides)...).val;
        } else {
          contains_ = &default_contains<Instance, Key>;
        }
      }
    private:
      Get_function get_;
      Set_function set_;
      Erase_function erase_;
      Contains_function contains_;
    };
  }

  
  template<typename Key, typename Value>
  struct Map_subinterface {
    template<typename Parent>
    using Type = map_interface_impl::Map_subinterface_impl<Parent, Key, Value>;
  };
  template<typename Key, typename Value>
  using Map_interface = Interface<Map_subinterface<Key, Value>::template Type>;
}