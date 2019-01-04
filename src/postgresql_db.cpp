#include "postgresql_db.h"

int db::details::postgresql::prepare(
  Db_base& me_,
  const char* name,
  const char* sql,
  Stmt_& stmt)
{
  stmt = name;

  Db_impl<Postgresql_db>& me = static_cast<Db_impl<Postgresql_db>&>(
    me_);

  Scoped_pgres res;

  if((res = PQprepare(
    me.connection_,
    name,
    sql,
    0,
    nullptr)) == nullptr
    || PQresultStatus(
      res) != PGRES_COMMAND_OK)
  {
    me_.log_.critical(
      "Could not prepare statement {}, ({}): {}",
      name,
      sql,
      PQresultErrorMessage(
        res));

    return -1;
  }
  else
  {
    return 0;
  }
}

void db::details::postgresql::finalize(
  Db_base&,
  Stmt_&)
{
  //do nothing, postgresql can't finalize stmts without some shifty sql stuff
}

void db::details::postgresql::reset(
  Db_base&,
  Stmt_&)
{
  //do nothing, postgresql doesn't need to reset
}

db::details::postgresql::Stmt::Stmt() :
  binds_(),
  row_number_(
    -1)
{

}

int db::details::postgresql::Stmt::bind_static(
  int id,
  const char* v)
{
  binds_[id] = v;

  return 0;
}

int db::details::postgresql::Stmt::bind_static(
  int id,
  const char* v,
  size_t n)
{
  binds_[id].assign(
    v,
    n);

  return 0;
}

int db::details::postgresql::Stmt::bind_static(
  int id,
  const unsigned char* v)
{
  binds_[id] = reinterpret_cast<const char*>(
    v);

  return 0;
}

int db::details::postgresql::Stmt::bind_static(
  int id,
  const unsigned char* v,
  size_t n)
{
  binds_[id].assign(
    reinterpret_cast<const char*>(
      v),
    n);

  return 0;
}

int db::details::postgresql::Stmt::bind(
  int id,
  const char* v)
{
  binds_[id] = v;

  return 0;
}

int db::details::postgresql::Stmt::bind(
  int id,
  const char* v,
  size_t n)
{
  binds_[id].assign(
    v,
    n);

  return 0;
}

int db::details::postgresql::Stmt::bind(
  int id,
  const unsigned char* v)
{
  binds_[id] = reinterpret_cast<const char*>(
    v);

  return 0;
}

int db::details::postgresql::Stmt::bind(
  int id,
  const unsigned char* v,
  size_t n)
{
  binds_[id].assign(
    reinterpret_cast<const char*>(
      v),
    n);

  return 0;
}

int db::details::postgresql::Stmt::bind(
  int id,
  uint8_t v)
{
  binds_[id] = std::to_string(
    v);

  return 0;
}

int db::details::postgresql::Stmt::bind(
  int id,
  int8_t v)
{
  binds_[id] = std::to_string(
    v);

  return 0;
}

int db::details::postgresql::Stmt::bind(
  int id,
  uint16_t v)
{
  binds_[id] = std::to_string(
    v);

  return 0;
}

int db::details::postgresql::Stmt::bind(
  int id,
  int16_t v)
{
  binds_[id] = std::to_string(
    v);

  return 0;
}

int db::details::postgresql::Stmt::bind(
  int id,
  uint32_t v)
{
  binds_[id] = std::to_string(
    v);

  return 0;
}

int db::details::postgresql::Stmt::bind(
  int id,
  int32_t v)
{
  binds_[id] = std::to_string(
    v);

  return 0;
}

int db::details::postgresql::Stmt::bind(
  int id,
  uint64_t v)
{
  binds_[id] = std::to_string(
    v);

  return 0;
}

int db::details::postgresql::Stmt::bind(
  int id,
  int64_t v)
{
  binds_[id] = std::to_string(
    v);

  return 0;
}

db::Step_value db::details::postgresql::Stmt::step(
  )
{
  if (result_ == nullptr)
  {
    size_t max_id;

    if (binds_.empty())
    {
      max_id = 0;
    }
    else
    {
      max_id = binds_.rbegin()->first;
    }

    std::vector<const char*> bind_list(
      max_id,
      "");

    auto bind_end = binds_.end();

    for (auto bind_iter = binds_.begin();
    bind_iter != bind_end;
      ++bind_iter)
    {
      bind_list[bind_iter->first - 1] = bind_iter->second.c_str();
    }

    result_ = PQexecPrepared(
      this->db_->connection_,
      this->name_.c_str(),
      static_cast<int>(
        bind_list.size()),
      bind_list.data(),
      nullptr,
      nullptr,
      0);

    if (result_ == nullptr
      || PQresultStatus(
        result_) == PGRES_FATAL_ERROR)
    {
      this->db_->log_.critical(
        "PQexecPrepared failed: {}",
        PQresultErrorMessage(
          result_));

      return step_error;
    }

    tuple_count_ = PQntuples(
      result_);
  }

  if (row_number_ + 1 >= tuple_count_)
  {
    return step_done;
  }

  ++row_number_;

  return step_row;
}

int db::details::postgresql::Stmt::get_column(
  int id,
  uint8_t& v) const
{
  v = std::stoi(
    PQgetvalue(
      result_,
      row_number_,
      id));

  return 0;
}

int db::details::postgresql::Stmt::get_column(
  int id,
  int8_t& v) const
{
  v = std::stoi(
    PQgetvalue(
      result_,
      row_number_,
      id));

  return 0;
}

int db::details::postgresql::Stmt::get_column(
  int id,
  uint16_t& v) const
{
  v = std::stoi(
    PQgetvalue(
      result_,
      row_number_,
      id));

  return 0;
}

int db::details::postgresql::Stmt::get_column(
  int id,
  int16_t& v) const
{
  v = std::stoi(
    PQgetvalue(
      result_,
      row_number_,
      id));

  return 0;
}

int db::details::postgresql::Stmt::get_column(
  int id,
  uint32_t& v) const
{
  v = std::stoi(
    PQgetvalue(
      result_,
      row_number_,
      id));

  return 0;
}

int db::details::postgresql::Stmt::get_column(
  int id,
  int32_t& v) const
{
  v = std::stoi(
    PQgetvalue(
      result_,
      row_number_,
      id));

  return 0;
}

int db::details::postgresql::Stmt::get_column(
  int id,
  uint64_t& v) const
{
  v = std::stoi(
    PQgetvalue(
      result_,
      row_number_,
      id));

  return 0;
}

int db::details::postgresql::Stmt::get_column(
  int id,
  int64_t& v) const
{
  v = std::stoi(
    PQgetvalue(
      result_,
      row_number_,
      id));

  return 0;
}

int db::details::postgresql::Stmt::get_column(
  int id,
  const char*& v) const
{
  v = PQgetvalue(
    result_,
    row_number_,
    id);

  return 0;
}

int db::details::postgresql::Stmt::get_column(
  int id,
  const unsigned char*& v) const
{
  reinterpret_cast<const char*&>(
    v) = PQgetvalue(
      result_,
      row_number_,
      id);

  return 0;
}

db::details::Db_impl<db::Postgresql_db>::Db_impl(
  std::string const& log_name,
  spdlog::sink_ptr const& sink,
  cache::Cache_base::Ptr const& cache) :
  Db_impl_base(
    log_name,
    sink,
    cache)
{

}

int db::details::Db_impl<db::Postgresql_db>::open(
  const char* db_location)
{
  connection_ = PQconnectdb(
    db_location);

  if (PQstatus(
    connection_) != CONNECTION_OK)
  {
    log_.critical(
      "Could not connection to postgresql, {}",
      PQerrorMessage(
        connection_));

    return -1;
  }
  else
  {
    return 0;
  }
}

int db::details::Db_impl<db::Postgresql_db>::stmt_get(
  const char* name,
  Stmt& out)
{
  postgresql::Stmt_ stmt;

  if (Db_impl_base::stmt_get(
    name,
    stmt) != 0)
  {
    log_.critical(
      "Could not get stmt {}",
      name);

    return -1;
  }

  out.db_ = this;
  out.name_ = name;
  if (out.result_ != nullptr)
  {
    PQclear(
      out.result_);

    out.result_ = nullptr;
  }

  return 0;
}

int db::details::Db_impl<db::Postgresql_db>::begin()
{
  postgresql::Scoped_pgres result = PQexec(
    connection_,
    "BEGIN;");

  if (result == nullptr
    || PQresultStatus(
      result) != PGRES_COMMAND_OK)
  {
    return -1;
  }

  return 0;
}

int db::details::Db_impl<db::Postgresql_db>::commit()
{
  postgresql::Scoped_pgres result = PQexec(
    connection_,
    "COMMIT;");

  if (result == nullptr
    || PQresultStatus(
      result) != PGRES_COMMAND_OK)
  {
    return -1;
  }

  return 0;
}

int db::details::Db_impl<db::Postgresql_db>::rollback()
{
  postgresql::Scoped_pgres result = PQexec(
    connection_,
    "ROLLBACK;");

  if (result == nullptr
    || PQresultStatus(
      result) != PGRES_COMMAND_OK)
  {
    return -1;
  }

  return 0;
}