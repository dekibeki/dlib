#pragma once

#include <string_view>

#include <dlib/db.hpp>
#include <dlib/meta.hpp>

namespace dlib {

  template<typename Columns, typename Args>
  struct Statement;
  
  template<template<typename...> typename Column_list, typename ...Columns, template<typename...> typename Args_list, typename ...Args>
  struct Statement<Column_list<Columns...>, Args_list<Args...>>
  {
  public:

    template<typename Name>
    constexpr Statement(Name&& name) noexcept :
      name_(as_string_view(std::forward<Name>(name))) {

    }

    constexpr std::string_view name() const noexcept {
      return name_;
    }

    template<typename ...Db_args, typename Callback>
    Result<void> execute(Db_ex<Db_args...>& db, Callback&& callback, Args const&... args) const noexcept{
      return db.execute<Columns...>(name(), std::forward<Callback>(callback), args...);
    }

    template<typename ...Db_args, typename Callback>
    Result<void> operator()(Db_ex<Db_args...>& db, Callback&& callback, Args const&... args) const noexcept {
      return execute(db, std::forward<Callback>(callback), args...);
    }
  private:
    std::string_view name_;
  };
}