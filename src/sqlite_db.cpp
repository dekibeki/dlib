#include "sqlite_db.h"

#include <cstdio>

int db::details::sqlite::prepare(
  Db_base& db,
  const char* name,
  const char* sql,
  Stmt_& stmt)
{
  if (sqlite3_prepare_v2(
    static_cast<Db_impl<Sqlite_db>&>(
      db).connection_,
    sql,
    -1,
    &stmt,
    nullptr) != SQLITE_OK)
  {
    return -1;
  }
  else
  {
    return 0;
  }
}

void db::details::sqlite::finalize(
  Db_base&,
  Stmt_& stmt)
{
  sqlite3_finalize(
    stmt);
}

void db::details::sqlite::reset(
  Db_base&,
  Stmt_& stmt)
{
  sqlite3_reset(
    stmt);
}

db::details::Db_impl<db::Sqlite_db>::Db_impl(
  std::string const& log_name,
  spdlog::sink_ptr const& log_sink,
  cache::Cache_base::Ptr const& cache) :
  Db_impl_base(
    log_name,
    log_sink,
    cache)
{

}

int db::details::Db_impl<db::Sqlite_db>::open(
  const char* db_location)
{
  int res;

  if ((res = sqlite3_open_v2(
    db_location,
    &connection_,
    SQLITE_OPEN_READWRITE,
    nullptr)) != SQLITE_OK)
  {
    log_.critical(
      "could not open sqlite db: {} - {}",
      res,
      sqlite3_errmsg(
        connection_));

    return -1;
  }

  return 0;
}

int db::details::Db_impl<db::Sqlite_db>::stmt_get(
  const char* name,
  Stmt& out)
{
  sqlite::Stmt_ stmt;

  if (Db_impl_base::stmt_get(
    name,
    stmt) != 0)
  {
    log_.critical(
      "Could not get {} sql",
      name);

    return -1;
  }

  out.db_ = this;
  out.name_ = name;
  out.stmt_ = stmt;

  return 0;
}

int db::details::sqlite::Stmt::bind_static(
  int id,
  const char* str) const
{
  int res;

  if ((res = sqlite3_bind_text(
    stmt_,
    id,
    str,
    -1,
    SQLITE_STATIC)) == SQLITE_OK)
  {
    return 0;
  }

  db_->log_.critical(
    "Could not bind {} to stmt {} at index {}: {} - {}",
    str,
    name_,
    id,
    res,
    sqlite3_errmsg(
      db_->connection_));

  return -1;
}

int db::details::sqlite::Stmt::bind_static(
  int id,
  const char* str,
  size_t n) const
{
  int res;

  if ((res = sqlite3_bind_text(
    stmt_,
    id,
    str,
    static_cast<int>(
      n),
    SQLITE_STATIC)) == SQLITE_OK)
  {
    return 0;
  }

  db_->log_.critical(
    "Could not bind {} (length {}) to stmt {} at index {}: {} - {}",
    str,
    name_,
    n,
    id,
    res,
    sqlite3_errmsg(
      db_->connection_));

  return -1;
}

int db::details::sqlite::Stmt::bind_static(
  int id,
  const unsigned char* str) const
{
  int res;

  if ((res = sqlite3_bind_text(
    stmt_,
    id,
    reinterpret_cast<const char*>(
      str),
    -1,
    SQLITE_STATIC)) == SQLITE_OK)
  {
    return 0;
  }

  db_->log_.critical(
    "Could not bind {} to stmt {} at index {}: {} - {}",
    str,
    name_,
    id,
    res,
    sqlite3_errmsg(
      db_->connection_));

  return -1;
}

int db::details::sqlite::Stmt::bind_static(
  int id,
  const unsigned char* str,
  size_t n) const
{
  int res;

  if ((res = sqlite3_bind_text(
    stmt_,
    id,
    reinterpret_cast<const char*>(
      str),
    static_cast<int>(
      n),
    SQLITE_STATIC)) == SQLITE_OK)
  {
    return 0;
  }

  db_->log_.critical(
    "Could not bind {} (length {}) to stmt {} at index {}: {} - {}",
    str,
    n,
    name_,
    id,
    res,
    sqlite3_errmsg(
      db_->connection_));

  return -1;
}

int db::details::sqlite::Stmt::bind(
  int id,
  const char* str) const
{
  int res;

  if ((res = sqlite3_bind_text(
    stmt_,
    id,
    str,
    -1,
    SQLITE_TRANSIENT)) == SQLITE_OK)
  {
    return 0;
  }

  db_->log_.critical(
    "Could not bind {} to stmt {} at index {}: {} - {}",
    str,
    name_,
    id,
    res,
    sqlite3_errmsg(
      db_->connection_));

  return -1;
}

int db::details::sqlite::Stmt::bind(
  int id,
  const char* str,
  size_t n) const
{
  int res;

  if ((res = sqlite3_bind_text(
    stmt_,
    id,
    str,
    static_cast<int>(
      n),
    SQLITE_TRANSIENT)) == SQLITE_OK)
  {
    return 0;
  }

  db_->log_.critical(
    "Could not bind {} (length {}) to stmt {} at index {}: {} - {}",
    str,
    n,
    name_,
    id,
    res,
    sqlite3_errmsg(
      db_->connection_));

  return -1;
}

int db::details::sqlite::Stmt::bind(
  int id,
  const unsigned char* str) const
{
  int res;

  if ((res = sqlite3_bind_text(
    stmt_,
    id,
    reinterpret_cast<const char*>(
      str),
    -1,
    SQLITE_TRANSIENT)) == SQLITE_OK)
  {
    return 0;
  }

  db_->log_.critical(
    "Could not bind {} to stmt {} at index {}: {} - {}",
    str,
    name_,
    id,
    res,
    sqlite3_errmsg(
      db_->connection_));

  return -1;
}

int db::details::sqlite::Stmt::bind(
  int id,
  const unsigned char* str,
  size_t n) const
{
  int res;

  if ((res = sqlite3_bind_text(
    stmt_,
    id,
    reinterpret_cast<const char*>(
      str),
    static_cast<int>(
      n),
    SQLITE_TRANSIENT)) == SQLITE_OK)
  {
    return 0;
  }

  db_->log_.critical(
    "Could not bind {} (length {}) to stmt {} at index {}: {} - {}",
    str,
    n,
    name_,
    id,
    res,
    sqlite3_errmsg(
      db_->connection_));

  return -1;
}

int db::details::sqlite::Stmt::bind(
  int id,
  uint8_t in) const
{
  int res;

  if ((res = sqlite3_bind_int(
    stmt_,
    id,
    in)) == SQLITE_OK)
  {
    return 0;
  }

  db_->log_.critical(
    "Could not bind {} ({}) to stmt {} at index {}: {} - {}",
    (size_t)in,
    in,
    name_,
    id,
    res,
    sqlite3_errmsg(
      db_->connection_));

  return -1;
}

int db::details::sqlite::Stmt::bind(
  int id,
  int8_t in) const
{
  int res;

  if ((res = sqlite3_bind_int(
    stmt_,
    id,
    in)) == SQLITE_OK)
  {
    return 0;
  }

  db_->log_.critical(
    "Could not bind {} ({}) to stmt {} at index {}: {} - {}",
    (int)in,
    in,
    name_,
    id,
    res,
    sqlite3_errmsg(
      db_->connection_));

  return -1;
}

int db::details::sqlite::Stmt::bind(
  int id,
  uint16_t in) const
{
  int res;

  if ((res = sqlite3_bind_int(
    stmt_,
    id,
    in)) == SQLITE_OK)
  {
    return 0;
  }

  db_->log_.critical(
    "Could not bind {} to stmt {} at index {}: {} - {}",
    in,
    name_,
    id,
    res,
    sqlite3_errmsg(
      db_->connection_));

  return -1;
}

int db::details::sqlite::Stmt::bind(
  int id,
  int16_t in) const
{
  int res;

  if ((res = sqlite3_bind_int(
    stmt_,
    id,
    in)) == SQLITE_OK)
  {
    return 0;
  }

  db_->log_.critical(
    "Could not bind {} to stmt {} at index {}: {} - {}",
    in,
    name_,
    id,
    res,
    sqlite3_errmsg(
      db_->connection_));

  return -1;
}

int db::details::sqlite::Stmt::bind(
  int id,
  uint32_t in) const
{
  int res;

  if ((res = sqlite3_bind_int(
    stmt_,
    id,
    in)) == SQLITE_OK)
  {
    return 0;
  }

  db_->log_.critical(
    "Could not bind {} to stmt {} at index {}: {} - {}",
    in,
    name_,
    id,
    res,
    sqlite3_errmsg(
      db_->connection_));

  return -1;
}

int db::details::sqlite::Stmt::bind(
  int id,
  int32_t in) const
{
  int res;

  if ((res = sqlite3_bind_int(
    stmt_,
    id,
    in)) == SQLITE_OK)
  {
    return 0;
  }

  db_->log_.critical(
    "Could not bind {} to stmt {} at index {}: {} - {}",
    in,
    name_,
    id,
    res,
    sqlite3_errmsg(
      db_->connection_));

  return -1;
}

int db::details::sqlite::Stmt::bind(
  int id,
  uint64_t in) const
{
  int res;
  if ((res = sqlite3_bind_int64(
    stmt_,
    id,
    in)) == SQLITE_OK)
  {
    return 0;
  }

  db_->log_.critical(
    "Could not bind {} to stmt {} at index {}: {} - {}",
    in,
    name_,
    id,
    res,
    sqlite3_errmsg(
      db_->connection_));

  return -1;
}

int db::details::sqlite::Stmt::bind(
  int id,
  int64_t in) const
{
  int res;

  if ((res = sqlite3_bind_int64(
    stmt_,
    id,
    in)) == SQLITE_OK)
  {
    return 0;
  }

  db_->log_.critical(
    "Could not bind {} to stmt {} at index {}: {} - {}",
    in,
    name_,
    id,
    res,
    sqlite3_errmsg(
      db_->connection_));

  return -1;
}

db::Step_value db::details::sqlite::Stmt::step(
  ) const
{
  int res;
  switch (res = sqlite3_step(
    stmt_))
  {
  case SQLITE_BUSY:
  case SQLITE_LOCKED:
    return step_busy;
  case SQLITE_DONE:
    return step_done;
  case SQLITE_ROW:
    return step_row;
  default:
    db_->log_.critical(
      "Step error: {} - {}",
      res,
      sqlite3_errmsg(
        db_->connection_));
    return step_error;
  }
}

int db::details::sqlite::Stmt::get_column(
  int id,
  uint8_t& out) const
{
  out = sqlite3_column_int(
    stmt_,
    id);

  return 0;
}

int db::details::sqlite::Stmt::get_column(
  int id,
  int8_t& out) const
{
  out = sqlite3_column_int(
    stmt_,
    id);

  return 0;
}

int db::details::sqlite::Stmt::get_column(
  int id,
  uint16_t& out) const
{
  out = sqlite3_column_int(
    stmt_,
    id);

  return 0;
}

int db::details::sqlite::Stmt::get_column(
  int id,
  int16_t& out) const
{
  out = sqlite3_column_int(
    stmt_,
    id);

  return 0;
}

int db::details::sqlite::Stmt::get_column(
  int id,
  uint32_t& out) const
{
  out = sqlite3_column_int(
    stmt_,
    id);

  return 0;
}

int db::details::sqlite::Stmt::get_column(
  int id,
  int32_t& out) const
{
  out = sqlite3_column_int(
    stmt_,
    id);

  return 0;
}

int db::details::sqlite::Stmt::get_column(
  int id,
  uint64_t& out) const
{
  out = sqlite3_column_int(
    stmt_,
    id);

  return 0;
}

int db::details::sqlite::Stmt::get_column(
  int id,
  int64_t& out) const
{
  out = sqlite3_column_int(
    stmt_,
    id);

  return 0;
}

int db::details::sqlite::Stmt::get_column(
  int id,
  const char*& out) const
{
  reinterpret_cast<const unsigned char*&>(out) = sqlite3_column_text(
    stmt_,
    id);

  return 0;
}

int db::details::sqlite::Stmt::get_column(
  int id,
  const unsigned char*& out) const
{
  out = sqlite3_column_text(
    stmt_,
    id);

  return 0;
}

db::details::Db_impl<db::Sqlite_db>::~Db_impl(
  )
{
  sqlite3_close_v2(
    connection_);
}
