#pragma once

#include "db.h"
#include <sqlite3.h>

namespace db
{
  struct Sqlite_db;

  namespace details
  {
    namespace sqlite
    {
      typedef sqlite3_stmt* Stmt_;

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
        public Stmt_base<Db_impl<Sqlite_db>, Stmt_, reset>
      {
      public:

        friend class Db_impl<Sqlite_db>;

        int bind_static(
          int id,
          const char*) const;

        int bind_static(
          int id,
          const char*,
          size_t) const;

        int bind_static(
          int id,
          const unsigned char*) const;

        int bind_static(
          int id,
          const unsigned char*,
          size_t) const;

        int bind(
          int id,
          const char*) const;

        int bind(
          int id,
          const char*,
          size_t) const;

        int bind(
          int id,
          const unsigned char*) const;

        int bind(
          int id,
          const unsigned char*,
          size_t) const;

        int bind(
          int id,
          uint8_t) const;

        int bind(
          int id,
          int8_t) const;

        int bind(
          int id,
          uint16_t) const;

        int bind(
          int id,
          int16_t) const;

        int bind(
          int id,
          uint32_t) const;

        int bind(
          int id,
          int32_t) const;

        int bind(
          int id,
          uint64_t) const;

        int bind(
          int id,
          int64_t) const;

        Step_value step(
          ) const;

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
    class Db_impl<Sqlite_db> :
      public Db_impl_base<sqlite::Stmt_,sqlite::prepare,sqlite::finalize>
    {
    public:

      sqlite3* connection_;

      typedef sqlite_int64 Id;

      typedef sqlite::Stmt Stmt;

      friend class sqlite::Stmt;

      Db_impl(
        std::string const& log_name,
        spdlog::sink_ptr const& log_sink,
        cache::Cache_base::Ptr const& cache);

      int open(
        const char* location);

      int stmt_get(
        const char* stmt,
        Stmt& out);

      ~Db_impl();
    };
  }
}