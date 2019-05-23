#pragma once

#include <dlib/db.hpp>
#include <dlib/args.hpp>

namespace dlib {

  namespace stmt_impl {
    template<typename ...Columns_>
    struct Columns {};

    template<typename ...Binds_>
    struct Binds {};
  }

  template<typename Query_string, typename Columns, typename Args>
  struct Stmt;

  template<typename Query_string, template<typename...> typename Column_list, typename ...Columns, template<typename...> typename Args_list, typename ...Args>
  struct Stmt<Query_string, Column_list<Columns...>, Args_list<Args...>> :
    private Query_string {
  public:
    template<typename ...Ignored>
    constexpr Stmt(Query_string query_string, Ignored&& ...) noexcept :
      Query_string{ std::move(query_string) } {

    }

    auto query_string() const noexcept {
      return base()();
    }

    template<typename ...Db_args, typename Callback>
    Result<void> execute(Db_ex<Db_args...>& db, Callback&& callback, Args const& ... args) const noexcept {
      return db.template execute<Columns...>(query_string(), std::forward<Callback>(callback), args...);
    }

    template<typename ...Db_args, typename Callback>
    Result<void> operator()(Db_ex<Db_args...>& db, Callback&& callback, Args const& ... args) const noexcept {
      return execute(db, std::forward<Callback>(callback), args...);
    }
  private:
    constexpr Query_string const& base() const noexcept {
      return static_cast<Query_string const&>(*this);
    }
  };

  template<typename ...Columns>
  constexpr stmt_impl::Columns<Columns...> columns{};

  template<typename ...Binds>
  constexpr stmt_impl::Binds<Binds...> binds{};

  template<typename Query_string, typename ...Args>
  Stmt(Query_string, Args...)->Stmt<Query_string,
    Get_arg_defaulted<stmt_impl::Columns, stmt_impl::Columns<>, Args...>,
    Get_arg_defaulted<stmt_impl::Binds, stmt_impl::Binds<>, Args...>>;
}