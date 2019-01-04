#pragma once
#include <mutex>
#include <atomic>
#include <dlib/pointer_to.hpp>

namespace dlib {
  template<typename Concurrency>
  using Mutex = typename Concurrency::Mutex;

  template<typename Concurrency, typename T>
  using Atomic = typename Concurrency::template Atomic<T>;

  template<typename Concurrency, typename T>
  T atomic_load(const Atomic<Concurrency, T>* v) {
    return Concurrency::atomic_load(v);
  }

  template<typename Concurrency, typename T>
  void atomic_store(Atomic<Concurrency, T>* p, T v) {
    return Concurrency::atomic_store(p, v);
  }

  template<typename Concurrency, typename T, typename Tag = void>
  using Thread_local = typename Concurrency::template Thread_local<T, Tag>;

  struct Std_concurrency {
    using Mutex = std::mutex;

    template<typename T>
    using Atomic = std::atomic<T>;

    template<typename Value>
    static Value atomic_load(const Value* v) {
      return std::atomic_load(v);
    }

    template<typename Value>
    static void atomic_store(Value* p, Value v) {
      return std::atomic_store(p, v);
    }

    template<typename T, typename>
    struct Thread_local {
      static thread_local T t;
      operator T&() noexcept {
        return t;
      }
      operator T const&() const noexcept {
        return t;
      }
    };
  };

  namespace impl {
    struct Null_mutex {
      constexpr void lock() const noexcept {

      }

      constexpr void unlock() const noexcept {

      }

      bool try_lock() const noexcept {
        return true;
      }
    };
  }

  struct Null_concurrency {
    using Mutex = impl::Null_mutex;

    template<typename T>
    using Atomic = T;

    template<typename Value>
    static Value atomic_load(const Value* p) {
      return *p;
    }

    template<typename Value>
    static void atomic_store(Value* p, Value v) {
      *p = v;
    }

    template<typename T,typename>
    struct Thread_local {
      T t;
      operator T&() noexcept {
        return t;
      }
      operator T const&() const noexcept {
        return t;
      }
    };
  };

  template<typename>
  struct Concurrency_is;

  template<typename T, typename Concurrency>
  class Guarded;

  namespace concurrency_impl {
    template<typename T, typename Concurrency>
    class Locked {
    public:
      friend class ::dlib::Guarded<T, Concurrency>;

      constexpr T* operator->() const noexcept {
        return ptr_;
      }
      constexpr T& operator*() const noexcept {
        return *ptr_;
      }
    private:
      Locked(Mutex<Concurrency>& mutex, T* ptr) noexcept :
        lock_(mutex),
        ptr_(ptr) {

      }
      std::lock_guard<Concurrency> lock_;
      T* ptr_;
    };
  }

  template<typename T, typename Concurrency = Std_concurrency>
  class Guarded {
  public:
    using Concurrency_arg = Concurrency;
    using Value = T;

    Value* operator->() noexcept {
      return &value_;
    }
    const Value* operator->() const noexcept {
      return &value_;
    }

    Value& operator*() noexcept {
      return value_;
    }
    Value const& operator*() const noexcept {
      return value_;
    }

    template<typename = decltype(mutex_.lock())>
    void lock() noexcept {
      return this->mutex_.lock();
    }
    template<typename = decltype(mutex_.try_lock())>
    bool try_lock() noexcept {
      return this->mutex_.try_lock();
    }
    template<typename = decltype(mutex_.unlock())>
    void unlock() noexcept {
      return this->mutex_.unlock();
    }
  private:
    Mutex<Concurrency_arg> mutex_;
    Holder<Value> value_;
  };
}