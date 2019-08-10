#include <dlib/postgresql.hpp>

#include <libpq-fe.h>

namespace {
  enum class Postgresql_error : int {
    ok = 0,
    error,
    is_null
  };

  struct Postgresql_error_category final
    : public std::error_category {
    virtual const char* name() const noexcept final {
      return "postgresql";
    }
    virtual std::string message(int i) const noexcept final {
      return "unknown";
    }
  };

  const Postgresql_error_category postgresql_error_category;

  std::error_code make_error_code(Postgresql_error err) noexcept {
    return std::error_code{ static_cast<int>(err), postgresql_error_category };
  }
}

namespace std {
  template<>
  struct is_error_code_enum<::Postgresql_error> :
    public ::std::true_type {

  };
}

void dlib::postgresql_impl::Result_destructor::operator()(void* result) const noexcept {
  PQclear(static_cast<PGresult*>(result));
}

dlib::postgresql_impl::Results::Results(void* result) noexcept :
  on_{ 0 },
  max_{ PQntuples(static_cast<PGresult*>(result)) },
  results_{ result } {

}

dlib::Result<void> dlib::postgresql_impl::Results::get_column(size_t id, int64_t& returning) noexcept {
  PGresult* result = static_cast<PGresult*>(results_.get());
  if (PQgetisnull(result, on_, static_cast<int>(id))) {
    return Postgresql_error::is_null;
  }
  returning = std::stoll(PQgetvalue(result, on_, static_cast<int>(id)));
  return success;
}

dlib::Result<void> dlib::postgresql_impl::Results::get_column(size_t id, int32_t& returning) noexcept {
  PGresult* result = static_cast<PGresult*>(results_.get());
  if (PQgetisnull(result, on_, static_cast<int>(id))) {
    return Postgresql_error::is_null;
  }
  returning = std::stol(PQgetvalue(result, on_, static_cast<int>(id)));
  return success;
}

dlib::Result<void> dlib::postgresql_impl::Results::get_column(size_t id, std::string& returning) noexcept {
  PGresult* result = static_cast<PGresult*>(results_.get());
  if (PQgetisnull(result, on_, static_cast<int>(id))) {
    return Postgresql_error::is_null;
  }
  returning = returning.assign(PQgetvalue(result, on_, static_cast<int>(id)), PQgetlength(result, on_, static_cast<int>(id)));
  return success;
}

dlib::Result<void> dlib::postgresql_impl::Results::get_column(size_t id, std::string_view& returning) noexcept {
  PGresult* result = static_cast<PGresult*>(results_.get());
  if (PQgetisnull(result, on_, static_cast<int>(id))) {
    return Postgresql_error::is_null;
  }
  returning = std::string_view{
    PQgetvalue(result, on_, static_cast<int>(id)),
    static_cast<std::size_t>(PQgetlength(result, on_, static_cast<int>(id))) };
  return success;
}

dlib::Result<void> dlib::postgresql_impl::Results::get_column(size_t id, const char*& returning) noexcept {
  PGresult* result = static_cast<PGresult*>(results_.get());
  if (PQgetisnull(result, on_, static_cast<int>(id))) {
    return Postgresql_error::is_null;
  }
  returning = PQgetvalue(result, on_, static_cast<int>(id));
  return success;
}

dlib::Result<void> dlib::postgresql_impl::Results::get_column(size_t id, Blob& returning) noexcept {
  PGresult* result = static_cast<PGresult*>(results_.get());
  if (PQgetisnull(result, on_, static_cast<int>(id))) {
    return Postgresql_error::is_null;
  }
  returning = Blob{ 
    reinterpret_cast<std::byte*>(PQgetvalue(result, on_, static_cast<int>(id))), 
    static_cast<std::size_t>(PQgetlength(result, on_, static_cast<int>(id))) };
  return success;
}

dlib::Result<void> dlib::postgresql_impl::Results::get_column(size_t id, std::chrono::system_clock::time_point& returning) noexcept {
  PGresult* result = static_cast<PGresult*>(results_.get());
  if (PQgetisnull(result, on_, static_cast<int>(id))) {
    return Postgresql_error::is_null;
  }

  std::stringstream sstream;
  
  sstream << PQgetvalue(result, on_, static_cast<int>(id));
  date::from_stream(sstream, "%F %T", returning);

  return dlib::success;
}

dlib::Result<void> dlib::postgresql_impl::Results::get_column(size_t id, std::chrono::system_clock::duration& returning) noexcept {
  PGresult* result = static_cast<PGresult*>(results_.get());
  if (PQgetisnull(result, on_, static_cast<int>(id))) {
    return Postgresql_error::is_null;
  }

  std::stringstream sstream;

  sstream << PQgetvalue(result, on_, static_cast<int>(id));

  date::from_stream(sstream, "%5H:%M:%S", returning);

  return dlib::success;
}

bool dlib::postgresql_impl::Results::is_null_(size_t id) noexcept {
  return PQgetisnull(static_cast<PGresult*>(results_.get()), on_, id) != 0;
}

dlib::Postgresql_driver::Postgresql_driver() noexcept :
  connection_{ nullptr } {

}

dlib::Result<void> dlib::Postgresql_driver::open(std::string_view location) noexcept {
  std::string zero_terminated{ location };
  return open(zero_terminated.c_str());
}

dlib::Result<void> dlib::Postgresql_driver::open(std::string const& location) noexcept {
  return open(location.c_str());
}

dlib::Result<void> dlib::Postgresql_driver::open(const char* location) noexcept {
  PGconn* connection = PQconnectdb(location);
  if (connection == nullptr) {
    return Postgresql_error::error;
  } else if (PQstatus(connection) != CONNECTION_OK) {
    PQfinish(connection);
    return Postgresql_error::error;
  } else {
    connection_ = connection;
    return dlib::success;
  }
}

dlib::Result<void> dlib::Postgresql_driver::close() noexcept {
  PQfinish(static_cast<PGconn*>(connection_));
  connection_ = nullptr;
  return success;
}

dlib::Result<void> dlib::Postgresql_driver::begin() noexcept {
  PGresult* res = PQexec(static_cast<PGconn*>(connection_), "BEGIN;");
  if (res == nullptr || PQresultStatus(res) != PGRES_COMMAND_OK) {
    PQclear(res);
    return Postgresql_error::error;
  } else {
    PQclear(res);
    return success;
  }
}

dlib::Result<void> dlib::Postgresql_driver::commit() noexcept {
  PGresult* res = PQexec(static_cast<PGconn*>(connection_), "COMMIT;");
  if (res == nullptr || PQresultStatus(res) != PGRES_COMMAND_OK) {
    PQclear(res);
    return Postgresql_error::error;
  } else {
    PQclear(res);
    return success;
  }
}

dlib::Result<void> dlib::Postgresql_driver::rollback() noexcept {
  PGresult* res = PQexec(static_cast<PGconn*>(connection_), "ROLLBACK;");
  if (res == nullptr || PQresultStatus(res) != PGRES_COMMAND_OK) {
    PQclear(res);
    return Postgresql_error::error;
  } else {
    PQclear(res);
    return success;
  }
}

const char* dlib::Postgresql_driver::bind_arg_(Binding_temps&, const char* str) noexcept {
  return str;
}

const char* dlib::Postgresql_driver::bind_arg_(Binding_temps&, std::string const& str) noexcept {
  return str.c_str();
}

const char* dlib::Postgresql_driver::bind_arg_(Binding_temps& temps, std::string_view str) noexcept {
  temps.emplace_back(str);
  return temps.back().c_str();
}

namespace {
  char to_hex_digit(std::byte b) noexcept {
    switch (std::to_integer<uint8_t>(b)) {
    case 0: return '0';
    case 1: return '1';
    case 2: return '2';
    case 3: return '3';
    case 4: return '4';
    case 5: return '5';
    case 6: return '6';
    case 7: return '7';
    case 8: return '8';
    case 9: return '9';
    case 10: return 'A';
    case 11: return 'B';
    case 12: return 'C';
    case 13: return 'D';
    case 14: return 'E';
    case 15: return 'F';
    default:
      return '\0';
    }
  }
}

const char* dlib::Postgresql_driver::bind_arg_(Binding_temps& temps, Blob const& blob) noexcept {
  temps.emplace_back("\\x");
  temps.back().reserve(2 + blob.size());
  for (std::byte byte : blob) {
    temps.back().push_back(to_hex_digit((byte >> 4) & std::byte{ 0x0F }));
    temps.back().push_back(to_hex_digit(byte & std::byte{ 0x0F }));
  }
  return temps.back().c_str();
}

const char* dlib::Postgresql_driver::bind_arg_(Binding_temps& temps, std::chrono::system_clock::time_point const& time) noexcept {
  std::stringstream sstream;

  date::to_stream(sstream, "%F %T", time);

  temps.emplace_back(sstream.str());
  return temps.back().c_str();
}

const char* dlib::Postgresql_driver::bind_arg_(Binding_temps& temps, std::chrono::system_clock::duration const& duration) noexcept {
  std::stringstream sstream;

  date::to_stream(sstream, "%T", duration);

  temps.emplace_back(sstream.str());
  return temps.back().c_str();
}

void dlib::Postgresql_driver::bind_args_(std::vector<const char*>&, Binding_temps&) noexcept {

}

dlib::Result<dlib::postgresql_impl::Results> dlib::Postgresql_driver::exec_(const char* sql, std::vector<const char*> const& args) noexcept {
  PGresult* result = PQexecParams(
    static_cast<PGconn*>(connection_),
    sql,
    static_cast<int>(args.size()),
    nullptr,
    args.data(),
    nullptr,
    nullptr,
    0);

  if (result == nullptr
    || PQresultStatus(
      result) == PGRES_FATAL_ERROR) {
    return Postgresql_error::error;
  }

  return postgresql_impl::Results{ result };
}