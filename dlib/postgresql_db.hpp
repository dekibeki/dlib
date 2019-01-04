#pragma once

#include "db.h"
#include "scoped_resource.h"
#include <map>
#include <libpq-fe.h>

namespace db
{
  struct Postgresql_db;
  namespace details
  {
    namespace postgresql
    {
      typedef Scoped_res<PGresult*, nullptr, void, PQclear> Scoped_pgres;
      typedef Scoped_res<PGconn*, nullptr, void, PQfinish> Scoped_pgconn;

      typedef const char* Stmt_;

      int prepare(
        Db_base&,
        const char* name,
        const char* sql,
        Stmt_&);

      void finalize(
        Db_base&,
        Stmt_&);

      void reset(
        Db_base&,
        Stmt_&);

      class Stmt :
        public Stmt_base<Db_impl<Postgresql_db>, Stmt_, reset>
      {
      public:
        std::map<int, std::string> binds_;

        Scoped_pgres result_;

        int tuple_count_;
        int row_number_;

        Stmt();

        int bind_static(
          int id,
          const char*);

        int bind_static(
          int id,
          const char*,
          size_t);

        int bind_static(
          int id,
          const unsigned char*);

        int bind_static(
          int id,
          const unsigned char*,
          size_t);

        int bind(
          int id,
          const char*);

        int bind(
          int id,
          const char*,
          size_t);

        int bind(
          int id,
          const unsigned char*);

        int bind(
          int id,
          const unsigned char*,
          size_t);

        int bind(
          int id,
          uint8_t);

        int bind(
          int id,
          int8_t);

        int bind(
          int id,
          uint16_t);

        int bind(
          int id,
          int16_t);

        int bind(
          int id,
          uint32_t);

        int bind(
          int id,
          int32_t);

        int bind(
          int id,
          uint64_t);

        int bind(
          int id,
          int64_t);

        Step_value step(
          );

        int get_column(
          int id,
          uint8_t&) const;

        int get_column(
          int id,
          int8_t&) const;

        int get_column(
          int id,
          uint16_t&) const;

        int get_column(
          int id,
          int16_t&) const;

        int get_column(
          int id,
          uint32_t&) const;

        int get_column(
          int id,
          int32_t&) const;

        int get_column(
          int id,
          uint64_t&) const;

        int get_column(
          int id,
          int64_t&) const;

        int get_column(
          int id,
          const char*& out) const;

        int get_column(
          int id,
          const unsigned char*& out) const;
      };
    }

    template<>
    class Db_impl<Postgresql_db> :
      public Db_impl_base<postgresql::Stmt_, postgresql::prepare,postgresql::finalize>
    {
    public:
      postgresql::Scoped_pgconn connection_;
      typedef pg_int64 Id;
      typedef postgresql::Stmt Stmt;

      friend class postgresql::Stmt;

      Db_impl(
        std::string const& log_name,
        spdlog::sink_ptr const& sink,
        cache::Cache_base::Ptr const& cache);

      int open(
        const char* db_location);

      int stmt_get(
        const char* stmt,
        Stmt& out);

      int begin();

      int commit();

      int rollback();
    };
  }
}