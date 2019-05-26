#pragma once

#include <array>
#include <string_view>

#include <dlib/outcome.hpp>
#include <dlib/db.hpp>

namespace dlib_tests {
  namespace impl {
    constexpr std::array build_schema_queries{
      std::string_view{ "CREATE TABLE Test(id INTEGER NOT NULL, other INTEGER NOT NULL);"},
      std::string_view{ "ALTER TABLE Test ADD CONSTRAINT pk_test PRIMARY KEY(id);"} };

    constexpr std::string_view insert_query{
      "INSERT INTO Test(id,other) VALUES (1,1),(2,2);" };

    constexpr std::string_view select_max_id_query{
      "SELECT MAX(id) FROM Test;" };

    template<typename Impl>
    dlib::Result<void> build_schema(dlib::Db<Impl>& db, std::string_view test_location) {
      DLIB_TRY((db.open(test_location)));
      auto transaction = [](dlib::Db<Impl>& db) -> dlib::Result<void> {
        for (std::string_view query : build_schema_queries) {
          DLIB_TRY((db.execute(query, []() {})));
        }
        return dlib::success;
      };
      DLIB_TRY((db.transaction(transaction)));
      return dlib::success;
    }
  }
  
  template<typename Impl>
  dlib::Result<void> test_build_schema(std::string_view test_location) {
    dlib::Db<Impl> db;
    return impl::build_schema(db, test_location);
  }

  template<typename Impl>
  dlib::Result<void> test_insert_and_select_statement(std::string_view test_location) {
    dlib::Db<Impl> db;
    DLIB_TRY((impl::build_schema(db, test_location)));
    int value;
    DLIB_TRY((db.execute(impl::insert_query, []() {})));
    DLIB_TRY((db.template execute<int>(impl::select_max_id_query, [&value](int i) { value = i; })));
    if (value != 2) {
      return dlib::Errors::not_found;
    }
    return dlib::success;
  }
}