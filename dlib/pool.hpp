#pragma once

#include <vector>
#include <memory>
#include <dlib/args.hpp>
#include <dlib/concurrency.hpp>
#include <dlib/outcome.hpp>
#include <dlib/pointer_to.hpp>
#include <dlib/unique_value.hpp>

namespace dlib {

  template<typename Pool>
  class Return_to_pool_destroyer {
  public:
    using Pool_holder = typename Pool::Holder;
    using Pool_type = typename Pool::Type;

    Return_to_pool_destroyer(Pool_holder pool) :
      pool_(std::move(pool)) {

    }

    void operator()(Pool_type& type) const noexcept {
      pool_->give_back(std::move(type));
    }
  private:
    Pool_holder pool_;
  };

  template<typename Pool_>
  struct Pooled :
    public Pool_::Pooled_info {
    using Pool = Pool_;
    using Type = typename Pool::Type;
    using Info = typename Pool::Pooled_info;
    Type value;

    Pooled(Type value_, Info info = Info()) :
      Info(info),
      value(std::move(value_)) {

    }
  };

  template<typename Pool_>
  using Returning_pointer = Unique_value<Pooled<Pool_>, Return_to_pool_destroyer<Pool_>>;

  namespace pool_impl {
    template<typename Type_, typename Concurrency_, typename Pointer_>
    class Pool_base :
      public Get_pointer_from<Pointer_>
    {
    public:
      using Type = Type_;
      using Concurrency_arg = Concurrency_;
      using Pointer_arg = Pointer_;
      struct Pooled_info {};
    protected:
      Result<Type> get_() noexcept {
        auto lock = get_lock_();
        return locked_get_();
      }

      template<typename Constructor>
      Result<Type> get_(Constructor&& constructor) {
        {
          auto lock = get_lock_();
          auto got = locked_get_();
          if (got) {
            return got;
          }
        }
        //we call constructor out of the lock
        return constructor();
      }

      Result<Type> locked_get_() noexcept {
        if (this->holding_.empty()) {
          //TODO: fix this, make a nice error_message
          return Errors::empty;
        } else {
          Type value = std::move(holding_.back());
          holding_.pop_back();
          return value;
        }
      }

      void give_back_(Type&& value) noexcept {
        auto lock = get_lock_();
        locked_give_back_(std::move(value));
      }

      void locked_give_back_(Type&& value) noexcept {
        holding_.emplace_back(std::move(value));
      }

      [[nodiscard]]
      auto get_lock_() noexcept {
        return std::lock_guard{ mutex_ };
      }
    private:
      Mutex<Concurrency_> mutex_;
      std::vector<Type_> holding_;
    };

    template<typename Type_, typename Concurrency_, typename Pointer_>
    class Pool final :
      public Pool_base<Type_, Concurrency_, Pointer_> {
    public:
      using Base = Pool_base<Type_, Concurrency_, Pointer_>
      using Pooled = ::dlib::Pooled<Pool>;
      using Pointer_to = Get_pointer_to<typename Base::Pointer_arg, Pool>;
      using Holder = Get_holder<Pointer_arg, Pool>;

      template<typename Constructor>
      Result<Pooled> get(Constructor&& constructor) {
        DLIB_TRY(value, (this->get_(std::forward<Constructor>(constructor))));
        return Pooled( value );
      }

      Result<Pooled> get() {
        DLIB_TRY(value, (this->get_()));
        return Pooled( value );
      }

      void give_back(Pooled&& pooled) {
        this->give_back_(std::move(pooled.value));
      }
    };

    template<typename Type_, typename Concurrency_, typename Pointer_>
    class Versioned_pool final :
      public Pool_base<Type_, Concurrency_, Pointer_> {
    public:
      using Version = intmax_t;
      struct Pooled_info {
        Version version;
      };
      using Pointer_to = Get_pointer_to<Pointer_arg, Versioned_pool>;
      using Holder = Get_holder<Pointer_arg, Versioned_pool>;
      using Pooled = Pooled<Versioned_pool>;

      Versioned_pool() :
        version_( 0 ) {

      }

      template<typename Constructor>
      Result<Pooled> get(Constructor&& constructor) {
        DLIB_TRY(value, (this->get_(std::forward<Constructor>(constructor))));
      }

      Result<Pooled> get() noexcept {
        DLIB_TRY(value, (this->get_()));
        return Pooled(value, Pooled_info{ atomic_load(&version_) });
      }

      void give_back(Pooled&& pooled) noexcept {
        auto lock = get_lock_();
        if (version_ != static_cast<Pooled_info&>(pooled).version) {
          /* we move it so the previous pooled.value is valid, and it is destroyed here by going out of scope */
          Type destroying = std::move(pooled.value);
        } else {
          locked_give_back_(std::move(pooled.value));
        }
      }

      void increment_version() noexcept {
        auto lock = get_lock_();
        ++version_;
      }
    private:
      Version version_;
    };
}

  template<typename Type, typename ...Args>
  using Pool = pool_impl::Pool<Type, 
    Get_arg_defaulted<Concurrency_is, Std_concurrency, Args...>,
    Get_arg_defaulted<Pointer_is, Raw_pointer, Args...>>;

  template<typename Type, typename ...Args>
  using Versioned_pool = pool_impl::Versioned_pool<Type, 
    Get_arg_defaulted<Concurrency_is, Std_concurrency, Args...>,
    Get_arg_defaulted<Pointer_is, Raw_pointer, Args...>>;
}