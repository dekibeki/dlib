#pragma once

#include <tuple>
#include <utility>
#include <iterator>
#include <utility>
#include <dlib/unordered_vector.hpp>
#include <dlib/meta.hpp>

namespace dlib {

  namespace soa_impl {
    template<typename ...Args>
    using Vector_impl = Unordered_vector<Args...>;

    using size_type = Vector_impl<int>::size_type;
    using difference_type = Vector_impl<int>::difference_type;

    template<typename ...Members>
    class Soa_proxy_reference {
    public:
      Soa_proxy_reference(Members&... members) :
        members_(members...) {

      }
      template<size_t i>
      auto& get() const noexcept {
        return std::get<i>(members_).get();
      }

      template<typename T>
      T& get() noexcept {
        return std::get<std::reference_wrapper<T>>(members_).get();
      }

      void swap(Soa_proxy_reference& other) {
        (..., std::swap(this->get<Members>(), other.get<Members>()));
      }
      operator std::tuple<std::reference_wrapper<Members>...>&() noexcept {
        return members_;
      }
      operator std::tuple<std::reference_wrapper<Members>...> const&() const noexcept {
        return members_;
      }
      Soa_proxy_reference& operator=(Soa_proxy_reference const& other) {
        (..., (this->get<Members>() = other->get<Members>()));
        return *this;
      }
      Soa_proxy_reference& operator=(Soa_proxy_reference&& other) {
        (..., (this->get<Members>() = std::move(other.get<Members>())));
        return *this;
      }
      Soa_proxy_reference& operator=(std::tuple<std::reference_wrapper<Members>...> const& other) {
        (..., (this->get<Members>() = std::get<std::reference_wrapper<Members>>(other).get()));
        return *this;
      }
      Soa_proxy_reference& operator=(std::tuple<std::reference_wrapper<Members>...>&& other) {
        (..., (this->get<Members>() = std::move(std::get<std::reference_wrapper<Members>>(other).get())));
        return *this;
      }
    private:
      std::tuple<std::reference_wrapper<Members>...> members_;
    };

    template<typename ...Members>
    class Soa_proxy_pointer {
    public:
      Soa_proxy_pointer(Members&... members) noexcept :
        members_{ members... } {

      }

      std::tuple<std::reference_wrapper<Members>...>* operator->() noexcept {
        return &members_;
      }
      const std::tuple<std::reference_wrapper<Members>...>* operator->() const noexcept {
        return &members_;
      }
    private:
      std::tuple<std::reference_wrapper<Members>...> members_;
    };

    template<bool is_const, typename ...Members>
    class Soa_iterator_ex {
    public:
      static_assert(sizeof...(Members) > 0,
        "Why have an iterator with no members?");
      template<typename T>
      using Iterator = std::conditional_t<is_const,
        typename Vector_impl<T>::const_iterator,
        typename Vector_impl<T>::iterator>;

      using difference_type = ::dlib::soa_impl::difference_type;
      using value_type = std::conditional_t<is_const,
        std::tuple<std::reference_wrapper<const Members>...>,
        std::tuple<std::reference_wrapper<Members>...>>;
      using pointer = std::conditional_t<is_const,
        Soa_proxy_pointer<const Members...>,
        Soa_proxy_pointer<Members...>>;
      using reference = std::conditional_t<is_const,
        Soa_proxy_reference<const Members...>,
        Soa_proxy_reference<Members...>>;
      using iterator_category = std::random_access_iterator_tag;

      Soa_iterator_ex() noexcept :
        iters_{} {

      }

      Soa_iterator_ex(Iterator<Members>... iters) noexcept :
        iters_{ std::move(iters)... } {

      }
      template<typename Target>
      Iterator<Target>& get() noexcept {
        return std::get<Iterator<Target>>(iters_);
      }
      template<typename Target>
      Iterator<Target> const& get() const noexcept {
        return std::get<Iterator<Target>>(iters_);
      }
      void swap(Soa_iterator_ex& other) noexcept {
        std::swap(iters_, other.iters_);
      }
      reference operator*() const noexcept {
        return reference{ *get<Members>()... };
      }
      reference operator[](difference_type i) const noexcept {
        return *(*this + i);
      }
      pointer operator->() const noexcept {
        return Soa_proxy_pointer{ *get<Members>()... };
      }
      Soa_iterator_ex& operator++() noexcept {
        (..., ++get<Members>());
        return *this;
      }
      Soa_iterator_ex operator++(int) noexcept {
        Soa_iterator_ex copy{ *this };
        this->operator++();
        return copy;
      }
      Soa_iterator_ex& operator--() noexcept {
        (..., --get<Members>());
        return *this;
      }
      Soa_iterator_ex operator--(int) noexcept {
        Soa_iterator_ex copy{ *this };
        this->operator--();
        return copy;
      }
      Soa_iterator_ex& operator+=(difference_type i) noexcept {
        (..., (get<Members>() += i));
        return *this;
      }
      Soa_iterator_ex& operator-=(difference_type i) noexcept {
        (..., (get<Members>() -= i));
        return *this;
      }
      difference_type operator-(Soa_iterator_ex const& other) const noexcept {
        return std::get<0>(iters_) - std::get<0>(other.iters_);
      }
      bool operator==(Soa_iterator_ex const& other) const noexcept {
        return std::get<0>(iters_) == std::get<0>(other.iters_);
      }
      bool operator!=(Soa_iterator_ex const& other) const noexcept {
        return !this->operator==(other);
      }
      bool operator<(Soa_iterator_ex const& other) const noexcept {
        return std::get<0>(iters_) < std::get<0>(other.iters_);
      }
      bool operator<=(Soa_iterator_ex const& other) const noexcept {
        return this->operator<(other) || this->operator==(other);
      }
      bool operator>(Soa_iterator_ex const& other) const noexcept {
        return !this->operator<=(other);
      }
      bool operator>=(Soa_iterator_ex const& other) const noexcept {
        return !this->operator<(other);
      }
    private:
      std::tuple<Iterator<Members>...> iters_;
    };

    template<bool is_const, typename ...Members>
    Soa_iterator_ex<is_const, Members...> operator+(Soa_iterator_ex<is_const, Members...> const& me, difference_type const& i) noexcept {
      Soa_iterator_ex<is_const, Members...> copy{ me };
      copy += i;
      return copy;
    }

    template<bool is_const, typename ...Members>
    Soa_iterator_ex<is_const, Members...> operator+(difference_type const& i, Soa_iterator_ex<is_const, Members...> const& me) noexcept {
      Soa_iterator_ex<is_const, Members...> copy{ me };
      copy += i;
      return copy;
    }

    template<bool is_const, typename ...Members>
    Soa_iterator_ex<is_const, Members...> operator-(Soa_iterator_ex<is_const, Members...> const& me, difference_type const& i) noexcept {
      Soa_iterator_ex<is_const, Members...> copy{ me };
      copy -= i;
      return copy;
    }

    template<typename ...Members>
    using Soa_iterator = Soa_iterator_ex<false, Members...>;

    template<typename ...Members>
    using Soa_const_iterator = Soa_iterator_ex<true, Members...>;

    template<bool is_const, typename ...Members>
    void swap(Soa_iterator_ex<is_const, Members...>& iter1, Soa_iterator_ex<is_const, Members...>& iter2) noexcept {
      iter1.swap(iter2);
    }

    template<typename ...Members>
    void swap(Soa_proxy_reference<Members...> v1, Soa_proxy_reference<Members...> v2) {
      v1.swap(v2);
    }
  }

  template<typename ...Members>
  class Soa {
  public:
    using size_type = size_t;
    template<typename T>
    using Vector = soa_impl::Vector_impl<T>;
    using iterator = soa_impl::Soa_iterator<Members...>;
    using const_iterator = soa_impl::Soa_const_iterator<Members...>;

    template<typename Member>
    Member& get(size_type i) noexcept {
      return get<Member>()[i];
    }
    template<typename Member>
    Member const& get(size_type i) const noexcept {
      return get<Member>()[i];
    }
    template<typename Member>
    auto begin_underlying() noexcept {
      return get_underlying<Member>().begin();
    }
    template<typename Member>
    auto begin_underlying() const noexcept {
      return get_underlying<Member>().begin();
    }
    template<typename Member>
    auto cbegin_underlying() const noexcept {
      return get_underlying<Member>().cbegin();
    }
    template<typename Member>
    auto end_underlying() noexcept {
      return get_underlying<Member>().end();
    }
    template<typename Member>
    auto end_underlying() const noexcept {
      return get_underlying<Member>().end();
    }
    template<typename Member>
    auto cend_underlying() const noexcept {
      return get_underlying<Member>().cend();
    }
    iterator begin() noexcept {
      return iterator{ begin_underlying<Members>()... };
    }
    const_iterator begin() const noexcept {
      return const_iterator{ begin_underlying<Members>()... };
    }
    const_iterator cbegin() const noexcept {
      return const_iterator{ cbegin_underlying<Members>()... };
    }
    template<typename ...Targets>
    auto begin() noexcept {
      return soa_impl::Soa_iterator<Targets...>{ begin_underlying<Targets>()... };
    }
    template<typename ...Targets>
    auto begin() const noexcept {
      return soa_impl::Soa_const_iterator<Targets...>{ begin_underlying<Targets>()... };
    }
    template<typename ...Targets>
    auto cbegin() const noexcept {
      return soa_impl::Soa_const_iterator<Targets...>{ cbegin_underlying<Targets>()... };
    }
    iterator end() noexcept {
      return iterator{ end_underlying<Members>()... };
    }
    const_iterator end() const noexcept {
      return const_iterator{ end_underlying<Members>()... };
    }
    const_iterator cend() const noexcept {
      return const_iterator{ cend_underlying<Members>()... };
    }
    template<typename ...Targets>
    auto end() noexcept {
      return soa_impl::Soa_iterator<Targets...>{ end_underlying<Targets>()... };
    }
    template<typename ...Targets>
    auto end() const noexcept {
      return soa_impl::Soa_const_iterator<Targets...>{ end_underlying<Targets>()... };
    }
    template<typename ...Targets>
    auto cend() const noexcept {
      return soa_impl::Soa_const_iterator<Targets...>{ cend_underlying<Targets>()... };
    }

    template<typename Member>
    Vector<Member>& get_underlying() noexcept {
      return std::get<Vector<Member>>(holding_);
    }
    template<typename Member>
    Vector<Member> const& get_underlying() const noexcept {
      return std::get<Vector<Member>>(holding_);
    }
    void push(Members... members) noexcept {
      (..., get_underlying<Members>().emplace(std::move(members)));
    }
    void erase(size_t i) noexcept {
      (..., get_underlying<Members>().erase(get_underlying<Members>().begin() + i));
    }
    template<typename ...Targets>
    void erase(soa_impl::Soa_iterator<Targets...> const& iter) noexcept {
      using Type1 = First<Targets...>;
      erase(iter.get<Type1>() - get_underlying<Type1>())
    }
    template<typename ...Targets>
    void erase(soa_impl::Soa_const_iterator<Targets...> const& iter) noexcept {
      using Type1 = First<Targets...>;
      erase(iter.get<Type1>() - get_underlying<Type1>())
    }
    auto size() const noexcept {
      return std::get<0>(holding_).size();
    }

    void reserve(size_t i) noexcept {
      (..., get_underlying<Members>().reserve(i));
    }
  private:
    std::tuple<Vector<Members>...> holding_;
  };
}

namespace std {
  template<typename ...Members>
  class tuple_size<::dlib::soa_impl::Soa_proxy_reference<Members...>> :
    public ::std::integral_constant<::std::size_t, sizeof...(Members)> {
  };

  template<typename First, typename ...Rest>
  class tuple_element<0, ::dlib::soa_impl::Soa_proxy_reference<First, Rest...>> {
  public:
    using type = First;
  };

  template<::std::size_t i, typename First, typename ...Rest>
  class tuple_element<i, ::dlib::soa_impl::Soa_proxy_reference<First, Rest...>> {
  public:
    using type = typename tuple_element<i - 1, ::dlib::soa_impl::Soa_proxy_reference<Rest...>>::type;
  };

  template<::std::size_t i, typename ...Members>
  ::std::tuple_element_t<i, ::dlib::soa_impl::Soa_proxy_reference<Members...>>& get(::dlib::soa_impl::Soa_proxy_reference<Members...>& t) noexcept {
    return t.template get<i>();
  }

  template<::std::size_t i, typename ...Members>
  ::std::tuple_element_t<i, ::dlib::soa_impl::Soa_proxy_reference<Members...>> const& get(::dlib::soa_impl::Soa_proxy_reference<Members...> const& t) noexcept {
    return t.template get<i>();
  }

  template<::std::size_t i, typename ...Members>
  ::std::tuple_element_t<i, ::dlib::soa_impl::Soa_proxy_reference<Members...>> && get(::dlib::soa_impl::Soa_proxy_reference<Members...>&& t) noexcept {
    return std::move(t.template get<i>());
  }

  template<::std::size_t i, typename ...Members>
  ::std::tuple_element_t<i, ::dlib::soa_impl::Soa_proxy_reference<Members...>> && get(::dlib::soa_impl::Soa_proxy_reference<Members...>const && t) noexcept {
    return std::move(t.template get<i>());
  }
}