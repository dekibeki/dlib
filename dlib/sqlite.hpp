#pragma once

#include <string>
#include <string_view>

#include <dlib/arrays.hpp>
#include <dlib/db.hpp>

namespace dlib
{
  namespace sqlite_impl
  {
    Result<int> name_to_index(void* stmt, std::string_view name) noexcept;

    struct Impl {
    public:
      struct Driver {
        constexpr Driver() noexcept :
          db{ nullptr } {

        }
        Driver(Driver const&) = delete;
        constexpr Driver(Driver&& other) noexcept :
          db{ std::move(other.db) } {
          other.db = nullptr;
        }

        Driver& operator=(Driver const&) = delete;
        constexpr Driver& operator=(Driver&& other) noexcept {
          db = other.db;
          other.db = nullptr;
          return *this;
        }
        void* db;
      };
      struct Stmt {
        bool done_initial;
        std::vector<void*> stmts;
        Stmt() noexcept;
      };

      static Result<Driver> open(std::string_view location) noexcept;
      static Result<void> close(Driver& driver) noexcept;
      static Result<void> begin(Driver& driver) noexcept;
      static Result<void> commit(Driver& driver) noexcept;
      static Result<void> rollback(Driver& driver) noexcept;
      static Result<Stmt> prepare(Driver& driver, std::string_view sql) noexcept;
      static Result<void> reset(Driver& driver, Stmt& stmt) noexcept;
      static Result<void> finalize(Driver& driver, Stmt& stmt) noexcept;
      static Result<Stmt_step> step(Driver& driver, Stmt& stmt) noexcept;

      template<typename ...Args>
      static Result<void> bind(Driver& driver, Stmt& stmt, int index, Args&&... args) noexcept {
        return bind_(driver, stmt, index + 1, std::forward<Args>(args)...);
      }

      template<typename ...Args>
      static Result<void> bind(Driver& driver, Stmt& stmt, std::string_view name, Args&&... args) noexcept {
        DLIB_TRY(index, (name_to_index(stmt, name)));
        return bind_(driver, stmt, index, std::forward<Args>(args)...);
      }

      static Result<std::string_view> get_column(Driver&, Stmt&, int index, Column_type<std::string_view>) noexcept;
      static Result<int32_t> get_column(Driver&, Stmt&, int index, Column_type<int32_t>) noexcept;
      static Result<int64_t> get_column(Driver&, Stmt&, int index, Column_type<int64_t>) noexcept;
      static Result<double> get_column(Driver&, Stmt&, int index, Column_type<double>) noexcept;
      static Result<Array_view<const std::byte>> get_column(Driver&, Stmt&, int index, Column_type<Array_view<const std::byte>>) noexcept;
    private:
      static Result<void> bind_(Driver&, Stmt&, int index, nullptr_t) noexcept;
      static Result<void> bind_(Driver&, Stmt&, int index, const char*) noexcept;
      static Result<void> bind_(Driver&, Stmt&, int index, std::string const&) noexcept;
      static Result<void> bind_(Driver&, Stmt&, int index, std::string_view) noexcept;
      static Result<void> bind_(Driver&, Stmt&, int index, int32_t value) noexcept;
      static Result<void> bind_(Driver&, Stmt&, int index, int64_t value) noexcept;
      static Result<void> bind_(Driver&, Stmt&, int index, double value) noexcept;
      static Result<void> bind_(Driver&, Stmt&, int index, Array_view<std::byte> value) noexcept;
    };
  }
  using Sqlite_impl = sqlite_impl::Impl;
  using Sqlite = Db<Sqlite_impl>;
}