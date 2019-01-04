#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include <unordered_map>
#include <optional>
#include <cassert>

#include <dlib/args.hpp>
#include <dlib/outcome.hpp>
#include <dlib/pointer_to.hpp>
#include <dlib/finder_interface.hpp>
#include <dlib/concurrency.hpp>

namespace dlib {
  namespace cache_impl {

    template<typename Key_, typename Value_, typename Concurrency_, typename Pointer_>
    class Cache :
      protected Get_pointer_from<Pointer_> {
    public:
      using Key = Key_;
      using Value = Value_;
      using Pointer_arg = Pointer_;
      using Pointer_to = Get_pointer_to<Pointer_arg, Cache>;
      using Holder = Get_holder<Pointer_arg, Cache>;
      using Concurrency_arg = Concurrency_;
      using Mutex = Mutex<Concurrency_arg>;

      using Cache_line = std::shared_ptr<const Value>;
      using Source = Finder_interface<Key, std::shared_ptr<const Value>>;

      Cache() = default;
      Cache(Cache const& cache) noexcept {
        *this = cache;
      }
      Cache(Cache&& cache) noexcept {
        *this = std::move(cache);
      }
      Cache& operator=(Cache const& cache) noexcept {
        std::scoped_lock lock{ mutex_, cache.mutex_ };
        lines_ = cache.lines_;
        sources_ = cache.sources_;
        return *this;
      }
      Cache& operator=(Cache&& cache) noexcept {
        std::scoped_lock lock{ mutex_, cache.mutex_ };
        lines_ = std::move(cache.lines_);
        sources_ = std::move(cache.sources_);
        return *this;
      }

      void add_source(Source source) {
        std::lock_guard{ mutex_ };
        sources_.emplace_back(std::move(source));
      }

      template<typename Instance, typename ...Overrides>
      void add_source(Instance instance, Overrides&&... overrides) {
        std::lock_guard{ mutex_ };
        sources_.emplace_back(Source{ std::move(instance), std::forward<Overrides>(overrides)... });
      }

      Result<Cache_line> read(Key const& key) noexcept {
        //no lock as this is a proxy
        return deep_read(key);
      }

      /*Try and read current, if that fails, read through*/
      Result<Cache_line> deep_read(Key const& key) noexcept {
        std::lock_guard{ mutex_ };
        auto current = read_(key);
        if (current) {
          return std::move(current);
        }

        auto through = read_backing_(key);
        if (through) {
          return set_(key, std::move(through.value()));
        } else {
          return std::errc::bad_address;
        }
      }

      /*Try and get the current version of a key*/
      Result<Cache_line> shallow_read(Key const& key) noexcept {
        std::lock_guard{ mutex_ };
        return read_(key);
      }

      /*Try and update a single key, if that fails, try and read current*/
      Result<Cache_line> read_through(Key const& key) noexcept {
        std::lock_guard{ mutex_ };
        auto through = read_backing_(key);
        if (through) {
          return set_(key, std::move(through.value()));
        }

        return read_(key);
      }

      /*Set the cached value to be this*/
      Result<Cache_line> set(Key const& key, Value value) noexcept {
        std::lock_guard{ mutex_ };
        return set_(key, std::make_shared<const Value>(std::move(value)));
      }

      /*Set the cached value to be this*/
      Result<Cache_line> set(Key const& key, std::shared_ptr<const Value> value) noexcept {
        std::lock_guard{ mutex_ };
        return set_(key, std::move(value));
      }

      /*Construct the cached value using args*/
      template<typename ...Args>
      Result<Cache_line> set(Key const& key, Args&&... args) noexcept {
        std::lock_guard{ mutex_ };
        return set_(key, std::make_shared<const Value>(std::forward<Args>(args)...));
      }

      /*remove a key*/
      void flush(Key const& key) noexcept {
        std::lock_guard{ mutex_ };
        const auto found = lines_.find(key);

        if (found != lines_.end()) {
          std::shared_ptr<const Value> setting(nullptr);
          found->second = std::move(setting);
        }
      }

      /*remove all keys*/
      void flush() noexcept {
        std::lock_guard{ mutex_ };
        std::shared_ptr<const Value> setting(nullptr);

        for (auto&& line : lines_) {
          line.second = std::move(setting);
        }
      }
    
      static Cache make() noexcept {
        return Cache{};
      }
    private:
      Result<Cache_line> read_(Key const& key) const noexcept {
        const auto found = lines_.find(key);
        if (found == lines_.end()) {
          return std::errc::bad_address;
        } else {
          auto ptr = found->second;
          if (ptr) {
            return ptr;
          } else {
            return std::errc::bad_address;
          }
        }
      }

      Result<std::shared_ptr<const Value>> read_backing_(Key const& key) noexcept {
        for (auto& updater : sources_) {
          auto updated{ updater.get(key) };
          if (updated) {
            return std::move(updated);
          }
        }
        return std::errc::bad_address;
      }

      Cache_line set_(Key const& key, Cache_line ptr) noexcept {
        const auto found = lines_.find(key);

        if (found == lines_.end()) {
          const auto emplaced = lines_.emplace(key, std::move(ptr)).first;
          return emplaced->second;
        } else {
          found->second = std::move(ptr);
          return found->second;
        }
      }
      Mutex mutex_;
      std::unordered_map<Key, Cache_line> lines_;
      std::vector<Source> sources_;
    };
  }

  template<typename Key, typename Value, typename ...Args>
  using Cache = cache_impl::Cache<Key, Value,
    Get_arg_defaulted<Concurrency_is, Std_concurrency, Args...>,
    Get_arg_defaulted<Pointer_is, Raw_pointer, Args...>>;
}