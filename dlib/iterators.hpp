#pragma once

#include <type_traits>
#include <iterator>
#include <tuple>

#include <dlib/util.hpp>
#include <dlib/soa_reference.hpp>

namespace dlib {
  template<typename Iterator_tag>
  constexpr bool is_at_least_bidirectional =
    std::is_same_v<Iterator_tag, std::bidirectional_iterator_tag>
    || std::is_same_v<Iterator_tag, std::random_access_iterator_tag>;

  template<typename Iterator_tag>
  constexpr bool is_at_least_random_access =
    std::is_same_v<Iterator_tag, std::random_access_iterator_tag>;

  template<typename Functor_, typename Inner_iterator_>
  struct Converting_iterator {
  public:
    using Functor = Functor_;
    using Inner_iterator = Inner_iterator_;
    using Inner_iterator_traits = std::iterator_traits<Inner_iterator>;
    using Inner_iterator_value_type = typename Inner_iterator_traits::value_type;
    using Inner_iterator_reference = typename Inner_iterator_traits::reference;
    static_assert(std::is_invocable_v<Functor, Inner_iterator_reference>);
    using value_type = std::invoke_result_t<Functor, Inner_iterator_reference>;
    using difference_type = typename Inner_iterator_traits::difference_type;
    using pointer = value_type * ;
    using reference = value_type & ;
    using iterator_category = typename Inner_iterator_traits::iterator_category;

    Converting_iterator(Functor functor = Functor()) :
      inner{},
      functor_{ std::move(functor) } {

    }
    Converting_iterator(Inner_iterator inner, Functor functor = Functor()) :
      inner_{ std::move(inner) },
      functor_{ std::move(functor) } {

    }
    Inner_iterator& get_inner() noexcept {
      return inner_;
    }
    Inner_iterator const& get_inner() const noexcept {
      return inner_;
    }
    Functor& get_functor() noexcept {
      return functor_;
    }
    Functor const& get_functor() const noexcept {
      return functor_;
    }

    value_type operator*() {
      return functor_(*inner_);
    }

    pointer operator->() const {
      return &this->operator*();
    }

    Converting_iterator& operator++() {
      ++inner_;
      return *this;
    }

    Converting_iterator operator++(int) {
      Converting_iterator returning{ *this };
      operator++();
      return returning;
    }

    template<typename = std::enable_if_t<is_at_least_bidirectional<iterator_category>>>
    Converting_iterator& operator--() {
      --inner_;
      return *this;
    }

    template<typename = std::enable_if_t<is_at_least_bidirectional<iterator_category>>>
    Converting_iterator operator--(int) {
      Converting_iterator returning{ *this };
      operator--();
      return returning;
    }

    template<typename = std::enable_if_t<is_at_least_random_access<iterator_category>>>
    Converting_iterator& operator+=(difference_type n) {
      inner_ += n;
      return *this;
    }

    template<typename = std::enable_if_t<is_at_least_random_access<iterator_category>>>
    Converting_iterator& operator-=(difference_type n) {
      inner_ -= n;
      return *this;
    }

    template<typename = std::enable_if_t<is_at_least_random_access<iterator_category>>>
    difference_type operator-(Converting_iterator const& other) const {
      return inner_ - other.inner_;
    }

    template<typename = std::enable_if_t<is_at_least_random_access<iterator_category>>>
    Converting_iterator operator-(difference_type n) const {
      Converting_iterator returning{ *this };
      returning -= n;
      return returning;
    }

    bool operator==(Converting_iterator const& other) const {
      return inner_ == other.inner_;
    }

    bool operator!=(Converting_iterator const& other) const {
      return inner != other.inner_;
    }

    template<typename = std::enable_if_t<is_at_least_random_access<iterator_category>>>
    bool operator<(Converting_iterator const& other) const {
      return inner_ < other.inner_;
    }
    template<typename = std::enable_if_t<is_at_least_random_access<iterator_category>>>
    bool operator>(Converting_iterator const& other) const {
      return inner_ > other.inner_;
    }
    template<typename = std::enable_if_t<is_at_least_random_access<iterator_category>>>
    bool operator<=(Converting_iterator const& other) const {
      return inner_ <= other.inner_;
    }
    template<typename = std::enable_if_t<is_at_least_random_access<iterator_category>>>
    bool operator>=(Converting_iterator const& other) const {
      return inner_ >= other.inner_;
    }
    template<typename = std::enable_if_t<is_at_least_random_access<iterator_category>>>
    value_type operator[](difference_type i) const {
      return functor_(inner_[i]);
    }

  private:
    Inner_iterator inner_;
    Functor functor_;
  };
  template<typename Functor, typename Inner>
  Converting_iterator<Functor, Inner> operator+(Converting_iterator<Functor, Inner> const& first, typename Converting_iterator<Functor, Inner>::difference_type const& second) {
    Converting_iterator copy{ first };
    copy += second;
    return copy;
  }
  template<typename Functor, typename Inner>
  Converting_iterator<Functor, Inner> operator+(typename Converting_iterator<Functor, Inner>::difference_type first, Converting_iterator<Functor, Inner> const& second) {
    Converting_iterator copy{ second };
    copy += first;
    return copy;
  }

  namespace iterators_impl {
    template<typename To>
    struct Static_cast {
    public:
      template<typename T>
      To& operator()(T& in) const noexcept {
        return cast_<To&>(in);
      }
      template<typename T>
      To const& operator()(T const& in) const noexcept {
        return cast_<To const&>(in);
      }
      template<typename T>
      To&& operator()(T&& in) const noexcept {
        return cast_<To&&>(std::forward<T&&>(in));
      }
      template<typename T>
      To const&& operator()(T const&& in) const noexcept {
        return cast_<const&&>(std::forward<T const&&>(in));
      }
    private:
      template<typename To, typename In>
      To cast_(In&& in) const noexcept {
        return static_cast<To>(std::forward<In>(in));
      }
    };

    template<typename To>
    struct Reinterpret_cast {
      template<typename T>
      To& operator()(T& in) const noexcept {
        return cast_<To&>(in);
      }
      template<typename T>
      To const& operator()(T const& in) const noexcept {
        return cast_<To const&>(in);
      }
      template<typename T>
      To&& operator()(T&& in) const noexcept {
        return cast_<To&&>(std::forward<T&&>(in));
      }
      template<typename T>
      To const&& operator()(T const&& in) const noexcept {
        return cast_<const&&>(std::forward<T const&&>(in));
      }
    private:
      template<typename To, typename In>
      To cast_(In&& in) const noexcept {
        return reinterpret_cast<To>(std::forward<In>(in));
      }
    };

    template<typename To>
    struct Dynamic_cast {
      template<typename T>
      To& operator()(T& in) const noexcept {
        return cast_<To&>(in);
      }
      template<typename T>
      To const& operator()(T const& in) const noexcept {
        return cast_<To const&>(in);
      }
      template<typename T>
      To&& operator()(T&& in) const noexcept {
        return cast_<To&&>(std::forward<T&&>(in));
      }
      template<typename T>
      To const&& operator()(T const&& in) const noexcept {
        return cast_<const&&>(std::forward<T const&&>(in));
      }
    private:
      template<typename To, typename In>
      To cast_(In&& in) const noexcept {
        return dynamic_cast<To>(std::forward<In>(in));
      }
    };
  }

  template<typename To, typename Iterator>
  using Reinterpret_cast_iterator = Converting_iterator<
    iterators_impl::Reinterpret_cast<To>,
    Iterator>;

  template<typename To, typename Iterator>
  using Static_cast_iterator = Converting_iterator<
    iterators_impl::Static_cast<To>,
    Iterator>;

  template<typename To, typename Iterator>
  using Dynamic_cast_iterator = Converting_iterator<
    iterators_impl::Dynamic_cast<To>,
    Iterator>;

  template<typename Functor, typename Iterator>
  Converting_iterator<Functor, Iterator> make_converting_iterator(Functor functor, Iterator iterator) {
    return { std::move(functor), std::move(iterator) };
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
    using reference = Soa_reference<
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
  Soa_iterator<Iters...> make_soa_iterator(Iters... iters) {
    return { std::move(iters)... };
  }

  template<typename ...Containers>
  auto make_soa_begin(Containers&&... containers) {
    return make_soa_iterator(containers.begin()...);
  }

  template<typename ...Containers>
  auto make_soa_end(Containers&&... containers) {
    return make_soa_iterator(containers.end()...);
  }

  namespace iterators_impl {
    template<typename Iterator>
    class Range {
    public:
      Range(Iterator begin, Iterator end) :
        begin_{ std::move(begin) },
        end_{ std::move(end) } {

      }

      Iterator begin() const noexcept {
        return begin_;
      }
      Iterator cbegin() const noexcept {
        return begin_;
      }

      Iterator end() const noexcept {
        return end_;
      }
      Iterator cend() const noexcept {
        return end_;
      }
    private:
      Iterator begin_;
      Iterator end_;
    };
  }

  template<typename ...Containers>
  auto make_soa_range(Containers&&... containers) {
    return iterators_impl::Range{
      make_soa_iterator(containers.begin()...),
      make_soa_iterator(containers.end()...) };
  }
}