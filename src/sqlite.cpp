#include <dlib/sqlite.hpp>

#include <system_error>

#include <sqlite3.h>

namespace {

  enum class Sqlite3_error : int {
    ok = 0,
    error = 1,
    internal = 2,
    perm = 3,
    abort = 4,
    busy = 5,
    locked = 6,
    nomem = 7,
    readonly = 8,
    interrupt = 9,
    ioerr = 10,
    corrupt = 11,
    notfound = 12,
    full = 13,
    cantopen = 14,
    protocol = 15,
    empty = 16,
    schema = 17,
    toobig = 18,
    constraint = 19,
    mismatch = 20,
    misuse = 21,
    nolfs = 22,
    auth = 23,
    format = 24,
    range = 25,
    notadb = 26,
    notice = 27,
    warning = 28,
  };

  struct Sqlite3_error_category final :
    public std::error_category {

    virtual const char* name() const noexcept final {
      return "sqlite3";
    }
    virtual std::string message(int i) const noexcept final {
      return sqlite3_errstr(i);
    }
  };

  const Sqlite3_error_category sqlite3_error_category;

  std::error_code make_error_code(Sqlite3_error err) noexcept {
    return std::error_code{ static_cast<int>(err), sqlite3_error_category };
  }
}

namespace std {
  template<>
  struct is_error_code_enum<::Sqlite3_error> :
    public ::std::true_type {

  };
}

/* FREE FUNCTIONS */

dlib::Result<int> dlib::sqlite_impl::name_to_index(void* stmt, std::string_view name) noexcept {
  std::string null_terminated_name{ name };
  if (int res = sqlite3_bind_parameter_index(static_cast<sqlite3_stmt*>(stmt), null_terminated_name.c_str()); res == 0) {
    return Sqlite3_error::notfound;
  } else {
    return res;
  }
}

/* IMPL */

dlib::Result<dlib::sqlite_impl::Impl::Driver> dlib::sqlite_impl::Impl::open(std::string_view location) noexcept {
  std::string null_terminated_location{ location };

  sqlite3* db{ nullptr };

  if (int res = sqlite3_open_v2(null_terminated_location.c_str(), &db, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, nullptr); res != SQLITE_OK) {
    return static_cast<Sqlite3_error>(res);
  } else {
    return Driver{ db };
  }
}

dlib::Result<void> dlib::sqlite_impl::Impl::close(Driver& driver) noexcept {
  if (int res = sqlite3_close_v2(static_cast<sqlite3*>(driver)); res != SQLITE_OK) {
    return static_cast<Sqlite3_error>(res);
  } else {
    driver = nullptr;
    return success();
  }
}

dlib::Result<void> dlib::sqlite_impl::Impl::begin(Driver& driver) noexcept {
  if (int res = sqlite3_exec(static_cast<sqlite3*>(driver), "BEGIN;", nullptr, nullptr, nullptr); res != SQLITE_OK) {
    return static_cast<Sqlite3_error>(res);
  } else {
    return success();
  }
}

dlib::Result<void> dlib::sqlite_impl::Impl::commit(Driver& driver) noexcept {
  if (int res = sqlite3_exec(static_cast<sqlite3*>(driver), "COMMIT;", nullptr, nullptr, nullptr); res != SQLITE_OK) {
    return static_cast<Sqlite3_error>(res);
  } else {
    return success();
  }
}

dlib::Result<void> dlib::sqlite_impl::Impl::rollback(Driver& driver) noexcept {
  if (int res = sqlite3_exec(static_cast<sqlite3*>(driver), "ROLLBACK;", nullptr, nullptr, nullptr); res != SQLITE_OK) {
    return static_cast<Sqlite3_error>(res);
  } else {
    return success();
  }
}

dlib::Result<dlib::sqlite_impl::Impl::Stmt> dlib::sqlite_impl::Impl::prepare(Driver& driver, std::string_view sql) noexcept {
  sqlite3_stmt* stmt{ nullptr };
  if (int res = sqlite3_prepare_v3(static_cast<sqlite3*>(driver), sql.data(), sql.size(), 0, &stmt, nullptr); res != SQLITE_OK) {
    return static_cast<Sqlite3_error>(res);
  } else {
    return Stmt{ stmt };
  }
}

dlib::Result<void> dlib::sqlite_impl::Impl::reset(Driver& driver, Stmt& stmt) noexcept {
  sqlite3_reset(static_cast<sqlite3_stmt*>(stmt));
  return success();
}

dlib::Result<void> dlib::sqlite_impl::Impl::finalize(Driver& driver, Stmt& stmt) noexcept {
  sqlite3_finalize(static_cast<sqlite3_stmt*>(stmt));
  stmt = nullptr;
  return success();
}

dlib::Result<dlib::Stmt_step> dlib::sqlite_impl::Impl::step(Driver& driver, Stmt& stmt) noexcept {
  if (int res = sqlite3_step(static_cast<sqlite3_stmt*>(stmt)); res == SQLITE_ROW) {
    return dlib::Stmt_step::Data;
  } else if (res == SQLITE_DONE) {
    return dlib::Stmt_step::Done;
  } else {
    return static_cast<Sqlite3_error>(res);
  }
}

dlib::Result<void> dlib::sqlite_impl::Impl::bind_(Driver&, Stmt& stmt, int index, nullptr_t) noexcept {
  if (int res = sqlite3_bind_null(static_cast<sqlite3_stmt*>(stmt), index); res != SQLITE_OK) {
    return static_cast<Sqlite3_error>(res);
  } else {
    return success();
  }
}

dlib::Result<void> dlib::sqlite_impl::Impl::bind_(Driver&, Stmt& stmt, int index, const char* str) noexcept {
  if (int res = sqlite3_bind_text(static_cast<sqlite3_stmt*>(stmt), index, str, -1, SQLITE_TRANSIENT); res != SQLITE_OK) {
    return static_cast<Sqlite3_error>(res);
  } else {
    return success();
  }
}

dlib::Result<void> dlib::sqlite_impl::Impl::bind_(Driver&, Stmt& stmt, int index, std::string const& str) noexcept {
  if (int res = sqlite3_bind_text(static_cast<sqlite3_stmt*>(stmt), index, str.data(), str.size(), SQLITE_TRANSIENT); res != SQLITE_OK) {
    return static_cast<Sqlite3_error>(res);
  } else {
    return success();
  }
}

dlib::Result<void> dlib::sqlite_impl::Impl::bind_(Driver&, Stmt& stmt, int index, std::string_view str) noexcept {
  if (int res = sqlite3_bind_text(static_cast<sqlite3_stmt*>(stmt), index, str.data(), str.size(), SQLITE_TRANSIENT); res != SQLITE_OK) {
    return static_cast<Sqlite3_error>(res);
  } else {
    return success();
  }
}

dlib::Result<void> dlib::sqlite_impl::Impl::bind_(Driver& driver, Stmt& stmt, int index, int32_t value) noexcept {
  if (int res = sqlite3_bind_int(static_cast<sqlite3_stmt*>(stmt), index, value); res != SQLITE_OK) {
    return static_cast<Sqlite3_error>(res);
  } else {
    return success();
  }
}

dlib::Result<void> dlib::sqlite_impl::Impl::bind_(Driver& driver, Stmt& stmt, int index, int64_t value) noexcept {
  if (int res = sqlite3_bind_int64(static_cast<sqlite3_stmt*>(stmt), index, value); res != SQLITE_OK) {
    return static_cast<Sqlite3_error>(res);
  } else {
    return success();
  }
}

dlib::Result<void> dlib::sqlite_impl::Impl::bind_(Driver& driver, Stmt& stmt, int index, double value) noexcept {
  if (int res = sqlite3_bind_double(static_cast<sqlite3_stmt*>(stmt), index, value); res != SQLITE_OK) {
    return static_cast<Sqlite3_error>(res);
  } else {
    return success();
  }
}

dlib::Result<void> dlib::sqlite_impl::Impl::bind_(Driver&, Stmt& stmt, int index, Array_view<std::byte> value) noexcept {
  if (int res = sqlite3_bind_blob(static_cast<sqlite3_stmt*>(stmt), index, value.data(), value.size(), SQLITE_TRANSIENT); res != SQLITE_OK) {
    return static_cast<Sqlite3_error>(res);
  } else {
    return success();
  }
}

dlib::Result<std::string_view> dlib::sqlite_impl::Impl::get_column(Driver&, Stmt& stmt, int index, Column_type<std::string_view>) noexcept {
  const unsigned char* text_ptr{ sqlite3_column_text(static_cast<sqlite3_stmt*>(stmt), index) };
  std::size_t text_length{ static_cast<std::size_t>(sqlite3_column_bytes(static_cast<sqlite3_stmt*>(stmt), index)) };

  if (text_length == 0 || text_ptr == nullptr) {
    return Sqlite3_error::nomem;
  } else {
    //TODO
    //DIRTY CAST
    return std::string_view{ reinterpret_cast<const char*>(text_ptr), text_length };
  }
}

dlib::Result<int32_t> dlib::sqlite_impl::Impl::get_column(Driver&, Stmt& stmt, int index, Column_type<int32_t>) noexcept {
  return sqlite3_column_int(static_cast<sqlite3_stmt*>(stmt), index);
}

dlib::Result<int64_t> dlib::sqlite_impl::Impl::get_column(Driver&, Stmt& stmt, int index, Column_type<int64_t>) noexcept {
  return sqlite3_column_int64(static_cast<sqlite3_stmt*>(stmt), index);
}

dlib::Result<double> dlib::sqlite_impl::Impl::get_column(Driver&, Stmt& stmt, int index, Column_type<double>) noexcept {
  return sqlite3_column_double(static_cast<sqlite3_stmt*>(stmt), index);
}

dlib::Result<dlib::Array_view<const std::byte>> dlib::sqlite_impl::Impl::get_column(Driver&, Stmt& stmt, int index, Column_type<Array_view<const std::byte>>) noexcept {
  const void* text_ptr{ sqlite3_column_blob(static_cast<sqlite3_stmt*>(stmt), index) };
  std::size_t text_length{ static_cast<std::size_t>(sqlite3_column_bytes(static_cast<sqlite3_stmt*>(stmt), index)) };

  if (text_length == 0 || text_ptr == nullptr) {
    return Sqlite3_error::nomem;
  } else {
    //TODO
    //DIRTY CAST
    return Array_view{ reinterpret_cast<const std::byte*>(text_ptr), text_length };
  }
}