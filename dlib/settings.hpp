#pragma once

#include <vector>

#include "db.h"

namespace settings
{
  namespace sql
  {
    extern const char get_setting[];
  }

  class Settings_base
  {
  public:
    virtual int get_settings(
      const char* name,
      std::vector<std::string>& settings) = 0;
  };

  template<typename Db_type>
  class Settings :
    public Settings_base
  {
  public:
    Settings(
      const char* name,
      spdlog::sink_ptr const& sink,
      std::shared_ptr<cache::Cache_base> const& cache) :
      db_(
        name,
        sink,
        cache)
    {

    }

    int open(
      const char* location)
    {
      return db_.open(
        location);
    }

    virtual int get_settings(
      const char* name,
      std::vector<std::string>& settings) override final
    {
      db::Easy_stmt <
        Db_type,
        sql::get_setting,
        db::Bind_tuple<
          const char*>, //setting name
        db::Return_tuple <
          const char* >> get_setting; //setting data

      if (get_setting.start(
        db_,
        name) != 0)
      {
        return -1;
      }

      const char* data;

      while (get_setting.step(
        data) == db::step_row)
      {
        settings.emplace_back(
          data);
      }

      return 0;
    }

  private:
    db::details::Db_impl<Db_type> db_;
  };
}