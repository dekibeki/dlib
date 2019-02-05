#pragma once

#include <dlib/interface.hpp>
#include <unordered_map>
#include <optional>
#include <functional>

namespace dlib {
  template<typename Instance>
  struct Finder_default_get {
    template<typename ...Args>
    constexpr auto operator()(Instance* instance, Args&&... args) const noexcept {
      return instance->get(std::forward<Args>(args)...);
    }
  };

  template<typename Instance>
  struct Finder_default_contains {
    template<typename ...Args>
    constexpr auto operator()(Instance* instance, Args&&... args) const noexcept {
      return instance->contains(std::forward<Args>(args)...);
    }
  };

  template<typename Key, typename Value, typename Hash, typename Eq, typename Allocator>
  struct Finder_default_get<std::unordered_map<Key, Value, Hash, Eq, Allocator>> {
    using Instance = std::unordered_map<Key, Value, Hash, Eq, Allocator>;

    constexpr std::optional<Value> operator()(const Instance* instance, Key const& key) const noexcept {
      const auto found{ instance->find(key) };

      if (found == instance->end()) {
        return std::nullopt;
      } else {
        return { found->second };
      }
    }
  };

  template<typename Key, typename Value, typename Hash, typename Eq, typename Allocator>
  struct Finder_default_contains<std::unordered_map<Key, Value, Hash, Eq, Allocator>> {
    using Instance = std::unordered_map<Key, Value, Hash, Eq, Allocator>;

    bool operator()(Instance* instance, Key const& key) const noexcept {
      return instance->count(key) != 0;
    }
  };

  namespace finder_interface_impl {
    struct Get_tag;

    template<typename Key, typename Value>
    struct Get_function {
      template<typename Parent_>
      class Type :
        public Interface_function<Parent_, false, std::optional<Value>(*)(Key), Finder_default_get, Get_tag> {
      public:
        using Interface_function<Parent_, false, std::optional<Value>(*)(Key), Finder_default_get, Get_tag>::Interface_function;

        std::optional<Value> get(Key key) noexcept {
          return Interface_function::call_(std::move(key));
        }
      };
    };

    struct Contains_tag;

    template<typename Key>
    struct Contains_function {
      template<typename Parent_>
      class Type :
        public Interface_function<Parent_, false, bool(*)(Key), Finder_default_contains, Contains_tag> {
      public:
        using Interface_function<Parent_, false, bool(*)(Key), Finder_default_contains, Contains_tag>::Interface_function;

        bool contains(Key key) noexcept {
          return Interface_function::call_(std::move(key));
        }
      };
    };
  }

  template<typename T>
  constexpr Arg<finder_interface_impl::Get_tag>::Holder<T> finder_get(T val) noexcept {
    return { std::move(val) };
  }

  template<typename T>
  constexpr Arg<finder_interface_impl::Contains_tag>::Holder<T> finder_contains(T val) noexcept {
    return { std::move(val) };
  }

  template<typename Key, typename Value>
  struct Finder_subinterface {
    template<typename Parent>
    using Type = Subinterface<
      typename finder_interface_impl::Get_function<Key, Value>::template Type<Parent>,
      typename finder_interface_impl::Contains_function<Key>::template Type<Parent>>;
  };

  template<typename Key, typename Value>
  using Finder_interface = Interface<typename Finder_subinterface<Key, Value>::template Type>;
}