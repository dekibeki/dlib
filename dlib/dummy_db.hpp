#pragma once

#include <dlib/db.hpp>

namespace dlib {
  namespace dummy_db_impl {
    struct Result_set {
      template<typename T>
      dlib::Result<void> get_column(size_t id, T& returning) noexcept {
        returning = T{};
        return dlib::success;
      }
    };
  }

  struct Dummy_db_driver {
    dlib::Result<void> open(std::string_view location) noexcept {
      return dlib::success;
    }
    dlib::Result<void> close() noexcept {
      return dlib::success;
    }
    dlib::Result<void> begin() noexcept {
      return dlib::success;
    }
    dlib::Result<void> commit() noexcept {
      return dlib::success;
    }
    dlib::Result<void> rollback() noexcept {
      return dlib::success;
    }
    template<typename Cb, typename ...Args>
    dlib::Result<void> execute(std::string_view sql, Cb&& cb, Args const& ... args) noexcept {
      dummy_db_impl::Result_set results;
      return cb(results);
    }
  };

  using Dummy_db = Db<Dummy_db_driver>;
}