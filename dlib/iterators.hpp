#pragma once

#include <type_traits>
#include <iterator>
#include <tuple>

#include <dlib/util.hpp>

namespace dlib {
  namespace iterators_impl {
    template<typename T, typename Inner_iterator_, template<typename> typename ...Additions>
    struct Casting_iterator_core :
      public Additions<Casting_iterator_core<T, Inner_iterator_, Additions...>>... {
    public:
      using Inner_iterator = Inner_iterator_;
      using value_type = T;
      using difference_type = typename std::iterator_traits<Inner_iterator>::difference_type;
      using pointer = T * ;
      using reference = T & ;
      using iterator_category = typename std::iterator_traits<Inner_iterator>::iterator_category;

      Casting_iterator_core() = default;
      Casting_iterator_core(Inner_iterator inner) :
        inner_{ std::move(inner) } {

      }
      Inner_iterator& get_inner() noexcept {
        return inner_;
      }
      Inner_iterator const& get_inner() const noexcept {
        return inner_;
      }
      pointer operator->() const {
        return &this->operator*();
      }

      Casting_iterator_core& operator++() {
        ++inner_;
        return *this;
      }

      Casting_iterator_core operator++(int) {
        Casting_iterator_core returning{ *this };
        operator++();
        return returning;
      }

      bool operator==(Casting_iterator_core const& other) const {
        return inner_ == other.inner_;
      }

      bool operator!=(Casting_iterator_core const& other) const {
        return inner != other.inner_;
      }
    private:
      Inner_iterator inner_;
    };

    template<typename Parent, typename Me>
    typename Parent::Inner_iterator& get_inner(Me* me) noexcept {
      return static_cast<Parent*>(me)->get_inner();
    }

    template<typename Parent, typename Me>
    typename Parent::Inner_iterator const& get_inner(const Me*) noexcept {
      return static_cast<const Parent*>(me)->get_inner();
    }

    template<typename Parent>
    struct Static_cast {
      using reference = typename Parent::reference;

      reference operator*() const {
        return static_cast<reference>(*get_inner<Parent>(this));
      }
    };

    template<typename Parent>
    struct Reinterpret_cast {
      using reference = typename Parent::reference;

      reference operator*() const {
        return reinterpret_cast<reference>(*get_inner<Parent>(this));
      }
    };

    template<typename Parent>
    struct Dynamic_cast {
      using reference = typename Parent::reference;

      reference operator*() const {
        return dynamic_cast<reference>(*get_inner<Parent>(this));
      }
    };

    template<typename Parent>
    struct Bi_directional {
      Parent& operator--() {
        --get_inner<Parent>(this);
        return static_cast<Parent&>(*this);
      }

      Parent operator--(int) {
        Parent returning{ static_cast<Parent const&>(*this) };
        operator--();
        return returning;
      }
    };

    template<typename Parent>
    struct Random_access :
      public Bi_directional<Parent> {
      using difference_type = typename Parent::difference_type;
      using reference = typename Parent::reference;

      Parent& operator+=(difference_type n) {
        get_inner<Parent>(this) += n;
        return static_cast<Parent&>(*this);
      }
      Parent& operator-=(difference_type n) {
        get_inner<Parent>(this) -= n;
        return static_cast<Parent&>(*this);
      }

      difference_type operator-(Parent const& other) const {
        return get_inner<Parent>(this) - get_inner<Parent>(&other);
      }

      Parent operator-(difference_type n) const {
        Parent returning{ static_cast<Parent const&>(*this) };
        get_inner<Parent>(&returning) - n;
        return returning;
      }

      bool operator<(Parent const& other) const {
        return get_inner<Parent>(this) < get_inner<Parent>(&other);
      }
      bool operator>(Parent const& other) const {
        return get_inner<Parent>(this) > get_inner<Parent>(&other);
      }
      bool operator<=(Parent const& other) const {
        return get_inner<Parent>(this) <= get_inner<Parent>(&other);
      }
      bool operator>=(Parent const& other) const {
        return get_inner<Parent>(this) >= get_inner<Parent>(&other);
      }
      reference operator[](difference_type i) {
        Parent copy{ static_cast<Parent const&>(this) };
        copy += i;
        return *copy;
      }
    };

    template<typename Parent>
    Parent operator+(Random_access<Parent> const& first, typename Parent::difference_type const& second) {
      Parent copy{ static_cast<Parent const&>(first) };
      copy += second;
      return copy;
    }
    template<typename Parent>
    Parent operator+(typename Parent::difference_type first, Random_access<Parent> const& second) {
      Parent copy{ static_cast<Parent const&> (second) };
      copy += first;
      return copy;
    }

    template<typename Iterator>
    using Iter_category = typename std::iterator_traits<Iterator>::iterator_category;

    template<typename T, typename Iterator, template<typename> typename Caster>
    using Casting_iterator =
      std::conditional_t<std::is_same_v<Iter_category<Iterator>, std::random_access_iterator_tag>, Casting_iterator_core<T, Iterator, Caster, Random_access>,
      std::conditional_t<std::is_same_v<Iter_category<Iterator>, std::bidirectional_iterator_tag>, Casting_iterator_core<T, Iterator, Caster, Bi_directional>,
      Casting_iterator_core<T, Iterator, Caster>>>;

    
  }

  template<typename T, typename Iterator>
  using Reinterpret_iterator = iterators_impl::Casting_iterator<T, Iterator, iterators_impl::Reinterpret_cast>;

  template<typename T, typename Iterator>
  using Static_iterator = iterators_impl::Casting_iterator<T, Iterator, iterators_impl::Static_cast>;

  namespace iterators_impl {
    template<typename ...Ts>
    class Soa_reference {
    public:

      using Value = std::tuple<std::reference_wrapper<Ts>...>;

      Soa_reference(Ts&... ts) :
        holding_{ ts... } {

      }

      Soa_reference(Soa_reference const&) = delete;
      Soa_reference(Soa_reference&&) = delete;

      Soa_reference& operator=(Soa_reference const& other) {
        for_each_tuples(
          [](auto& left, auto const& right) { left.get() = right.get(); },
          holding_, other.holding_);

        return *this;
      }

      Soa_reference& operator=(Value const& other) {
        for_each_tuples(
          [](auto& left, auto const& right) { left.get() = right.get(); },
          holding_, other);

        return *this;
      }

      Soa_reference& operator=(Soa_reference&& other) {
        for_each_tuples(
          [](auto& left, auto&& right) { left.get() = std::move(right.get());},
          holding_, other.holding_);

        return *this;
      }

      Soa_reference& operator=(Value&& other) {
        for_each_tuples(
          [](auto& left, auto&& right) {left.get() = std::move(right.get()); },
          holding_, other);

        return *this;
      }

      void swap(Soa_reference& other) {
        for_each_tuples(
          [](auto& left, auto& right) { std::swap(left.get(), right.get());},
          holding_, other.holding_);
      }
    
      Value const& get_underlying() const noexcept {
        return holding_;
      }
      constexpr operator Value&() noexcept {
        return holding_;
      }
      constexpr operator Value const&() const noexcept {
        return holding_;
      }
    private:
      Value holding_;
    };
  }

  template<typename First, typename ...Rest>
  class Soa_iterator {
  private:

    template<typename T>
    using Value_type = typename std::iterator_traits<T>::value_type;

    template<typename T>
    using Pointer = typename std::iterator_traits<T>::pointer;
  public:
    using difference_type = typename std::iterator_traits<First>::difference_type;
    using value_type = std::tuple<
      std::reference_wrapper<Value_type<First>>,
      std::reference_wrapper<Value_type<Rest>>...>;
    using pointer = std::tuple<
      Pointer<First>,
      Pointer<Rest>...>;
    using reference = iterators_impl::Soa_reference<
      Value_type<First>,
      Value_type<Rest>...>;
    using iterator_category = std::random_access_iterator_tag;

    Soa_iterator() noexcept :
      iters_{} {

    }

    Soa_iterator(First first, Rest... rest) noexcept :
      iters_{ std::move(first), std::move(rest)... } {

    }

    static Soa_iterator make(First first, Rest... rest) noexcept {
      return Soa_iterator{ std::move(first), std::move(rest)... };
    }
    void swap(Soa_iterator& other) noexcept {
      std::swap(iters_, other.iters_);
    }
    reference operator*() const noexcept {
      return std::apply(
        [](auto&&... iters) { return reference{ *iters... };},
        iters_);
    }
    reference operator[](difference_type i) const noexcept {
      return std::apply(
        [i](auto&&... iters) {return reference{ iters[i]... };},
        iters_);
    }
    pointer operator->() const noexcept {
      return std::apply(
        [](auto&&... iters) {return pointer{ &*iters... };},
        iters_);
    }
    Soa_iterator& operator++() noexcept {
      std::apply(
        [](auto&&... iters) {(..., ++iters);},
        iters_);
      return *this;
    }
    Soa_iterator operator++(int) noexcept {
      Soa_iterator copy{ *this };
      this->operator++();
      return copy;
    }
    Soa_iterator& operator--() noexcept {
      std::apply(
        [](auto&&... iters) {(..., --iters);},
        iters_);
      return *this;
    }
    Soa_iterator operator--(int) noexcept {
      Soa_iterator copy{ *this };
      this->operator--();
      return copy;
    }
    Soa_iterator& operator+=(difference_type i) noexcept {
      std::apply(
        [i](auto&&... iters) {(..., (iters += i));},
        iters_);
      return *this;
    }
    Soa_iterator& operator-=(difference_type i) noexcept {
      std::apply(
        [i](auto&&... iters) {(..., (iters -= i));},
        iters_);
      return *this;
    }
    difference_type operator-(Soa_iterator const& other) const noexcept {
      return std::get<0>(iters_) - std::get<0>(other.iters_);
    }
    bool operator==(Soa_iterator const& other) const noexcept {
      return std::get<0>(iters_) == std::get<0>(other.iters_);
    }
    bool operator!=(Soa_iterator const& other) const noexcept {
      return std::get<0>(iters_) != std::get<0>(other.iters_);
    }
    bool operator<(Soa_iterator const& other) const noexcept {
      return std::get<0>(iters_) < std::get<0>(other.iters_);
    }
    bool operator<=(Soa_iterator const& other) const noexcept {
      return std::get<0>(iters_) <= std::get<0>(other.iters_);
    }
    bool operator>(Soa_iterator const& other) const noexcept {
      return std::get<0>(iters_) > std::get<0>(other.iters_);
    }
    bool operator>=(Soa_iterator const& other) const noexcept {
      return std::get<0>(iters_) >= std::get<0>(other.iters_);
    }
  
    std::tuple<First, Rest...> const& get_underlying() const noexcept {
      return iters_;
    }
  private:
    std::tuple<First, Rest...> iters_;
  };

  template<typename ...Iters>
  Soa_iterator<Iters...> operator+(Soa_iterator<Iters...> const& me, typename Soa_iterator<Iters...>::difference_type const& i) noexcept {
    Soa_iterator<Iters...> copy{ me };
    copy += i;
    return copy;
  }

  template<typename ...Iters>
  Soa_iterator<Iters...> operator+(typename Soa_iterator<Iters...>::difference_type const& i, Soa_iterator<Iters...> const& me) noexcept {
    Soa_iterator<Iters...> copy{ me };
    copy += i;
    return copy;
  }

  template<typename ...Iters>
  Soa_iterator<Iters...> operator-(Soa_iterator<Iters...> const& me, typename Soa_iterator<Iters...>::difference_type const& i) noexcept {
    Soa_iterator<Iters...> copy{ me };
    copy -= i;
    return copy;
  }

  template<typename ...Iters>
  Soa_iterator<std::decay_t<Iters>...> make_soa_iterator(Iters&&... iters) {
    return Soa_iterator<std::decay_t<Iters>...>{std::forward<Iters>(iters)...};
  }

  namespace iterators_impl {

    template<typename Searching_for>
    struct Get_checker {
      template<typename T>
      using Check = std::is_same<std::decay_t<T>, std::decay_t<Searching_for>>;
    }; 
  }
}

namespace std {
  template<typename ...Ts>
  class tuple_size<::dlib::iterators_impl::Soa_reference<Ts...>> :
    public ::std::integral_constant<::std::size_t, sizeof...(Ts)> {

  };

  template<::std::size_t i, typename ...Ts>
  class tuple_element<i, ::dlib::iterators_impl::Soa_reference<Ts...>> {
    using type = ::std::tuple_element_t<i, ::std::tuple<Ts...>>;
  };

  template<::std::size_t i, typename ...Ts>
  constexpr decltype(auto) get(::dlib::iterators_impl::Soa_reference<Ts...> const& t) noexcept {
    return std::get<i>(t.get_underlying()).get();
  }

  template<typename T, typename ...Ts>
  constexpr decltype(auto) get(::dlib::iterators_impl::Soa_reference<Ts...> const& t) noexcept {
    return ::dlib::find_if_tuples<::dlib::iterators_impl::Get_checker<T>::template Check>(t);
  }

  template<typename ...Ts>
  void swap(::dlib::iterators_impl::Soa_reference<Ts...>& r1, ::dlib::iterators_impl::Soa_reference<Ts...>& r2) {
    return r1.swap(r2);
  }
}