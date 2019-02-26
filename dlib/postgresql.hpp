#pragma once

#include <dlib/db.hpp>
#include <unordered_map>

namespace dlib {
  namespace postgresql_impl {
    struct Driver_destructor {
      void operator()(void*) const noexcept;
    };

    struct Stmt_destructor {
      void operator()(void*) const noexcept;
    };

    struct Driver_impl {
      Driver_impl() noexcept;
      std::unique_ptr<void, Driver_destructor> connection;
    };

    struct Stmt_impl {
      Stmt_impl() noexcept;

      std::string sql;
      std::unordered_map<int, std::string> index_binds;
      std::unordered_map<std::string, std::string> named_binds;

      std::unique_ptr<void, Stmt_destructor> result;
    };

    struct Impl {
      using Driver = Driver_impl;
      using Stmt = Stmt_impl;

      static Result<Driver> open(std::string_view location) noexcept;
      static Result<void> begin(Driver& driver) noexcept;
      static Result<void> commit(Driver& driver) noexcept;
      static Result<void> rollback(Driver& driver) noexcept;
      static Result<Stmt> prepare(Driver& driver, std::string_view sql) noexcept;
      static Result<void> reset(Driver& driver, Stmt& stmt) noexcept;
      static Result<void> finalize(Driver& driver, Stmt& stmt) noexcept;
      static Result<Stmt_step> step(Driver& driver, Stmt& stmt) noexcept;
    };
  }
  using Postgresql_impl = postgresql_impl::Impl;
  template<typename Pointer>
  using Postgresql_ex = Db_ex<Postgresql_impl, Pointer>;
  using Postgresql = Db<Postgresql_impl>;
}