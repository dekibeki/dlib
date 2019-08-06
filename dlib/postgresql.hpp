#pragma once

#include <vector>
#include <deque>
#include <dlib/db.hpp>
#include <unordered_map>

#include <date/tz.h>

namespace dlib {
  namespace postgresql_impl { 
    struct Result_destructor {
    public:
      void operator()(void*) const noexcept;
    };

    struct Results {
    public:
      Results(void*) noexcept;
      Result<void> get_column(size_t id, int64_t&) noexcept;
      Result<void> get_column(size_t id, int32_t&) noexcept;
      Result<void> get_column(size_t id, std::chrono::system_clock::time_point&) noexcept;
      template<typename T>
      Result<void> get_column(size_t id, Nullable<T>& returning) noexcept {
        if (is_null_(id)) {
          returning = std::nullopt;
          return success;
        }

        returning = T{};
        return get_column(id, *returning);
      }
      template<typename Cb>
      Result<void> run_callbacks(Cb&& cb) noexcept {
        for (on_ = 0; on_ < max_; ++on_) {
          DLIB_TRY((cb(*this)));
        }
        return success;
      }
    private:
      bool is_null_(size_t id) noexcept;
      int on_;
      int max_;
      std::unique_ptr<void, Result_destructor> results_;
    };
  }
  struct Postgresql_driver {
  public:
    Postgresql_driver() noexcept;

    Result<void> open(std::string_view location) noexcept;
    Result<void> open(std::string const& location) noexcept;
    Result<void> open(const char* location) noexcept;

    Result<void> close() noexcept;

    Result<void> begin() noexcept;
    Result<void> commit() noexcept;
    Result<void> rollback() noexcept;

    template<typename Cb, typename ...Args>
    Result<void> execute(std::string_view sql, Cb&& cb, Args const& ... args) noexcept {
      std::string zero_terminated{ sql };
      return execute(zero_terminated.c_str(), std::forward<Cb>(cb), args...);
    }
    template<typename Cb, typename ...Args>
    Result<void> execute(std::string const& sql, Cb&& cb, Args const& ... args) noexcept {
      return execute(sql.c_str(), std::forward<Cb>(cb), args...);
    }
    template<typename Cb, typename ...Args>
    Result<void> execute(const char* sql, Cb&& cb, Args const& ... args) noexcept {
      Binding_temps temps;
      std::vector<const char*> args_vec;
      bind_args_(args_vec, temps, args...);

      DLIB_TRY(results, (exec_(sql, args_vec)));
      return results.run_callbacks(std::forward<Cb>(cb));
    }
    
  private:
    using Binding_temps = std::deque<std::string>;

    template<typename T>
    static const char* bind_arg_(Binding_temps& temps, T const& t) noexcept {
      temps.emplace_back(std::to_string(t));
      return temps.back().c_str();
    }
    static const char* bind_arg_(Binding_temps&, const char*) noexcept;
    static const char* bind_arg_(Binding_temps&, std::string const&) noexcept;
    static const char* bind_arg_(Binding_temps&, std::string_view) noexcept;
    static const char* bind_arg_(Binding_temps&, Blob const&) noexcept;
    static const char* bind_arg_(Binding_temps&, std::chrono::system_clock::time_point const&) noexcept;

    void bind_args_(std::vector<const char*>& args, Binding_temps&) noexcept;

    Result<postgresql_impl::Results> exec_(const char* sql, std::vector<const char*> const& args) noexcept;

    template<typename First, typename ...Rest>
    void bind_args_(std::vector<const char*>& args, Binding_temps& temps, First const& first, Rest const& ... rest) noexcept {
      args.emplace_back(bind_arg_(temps, first));
      bind_args_(args, temps, rest...);
    }

    void* connection_;
  };

  using Postgresql_db = Db<Postgresql_driver>;
}