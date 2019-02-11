#pragma once

#include <tuple>
#include <utility>
#include <iterator>
#include <utility>
#include <vector>
#include <dlib/vector_adaptors.hpp>
#include <dlib/util.hpp>
#include <dlib/arrays.hpp>

#include <dlib/iterators.hpp>

namespace dlib {

  namespace soa_impl {
    template<typename T>
    using Vector_impl = Unordered_vector<T, std::vector>;    
  
    template<typename ...Members>
    class Soa_impl {
    public:
      template<typename ...Ts>
      using Iterator = Soa_iterator<typename Ts::iterator...>;
      template<typename ...Ts>
      using Const_iterator = Soa_iterator<typename Ts::const_iterator...>;

      using size_type = size_t;
      using iterator = Iterator<Members...>;
      using const_iterator = Const_iterator<Members...>;
      using reference = typename iterator::reference;
      using const_reference = typename const_iterator::reference;

      iterator begin() noexcept {
        return std::apply(
          [](auto&&... vecs) { return iterator{ vecs.begin()... };},
          holding_);
      }
      const_iterator begin() const noexcept {
        return std::apply(
          [](auto&&... vecs) {return iterator{ vecs.begin()... };},
          holding_);
      }
      const_iterator cbegin() const noexcept {
        return std::apply(
          [](auto&&... vecs) {return make_soa_iterator(vecs.cbegin()...);},
          holding_);
      }
      iterator end() noexcept {
        return std::apply(
          [](auto&&... vecs) {return iterator{ vecs.end()... };},
          holding_);
      }
      const_iterator end() const noexcept {
        return std::apply(
          [](auto&&... vecs) {return iterator{ vecs.end()... };},
          holding_);
      }
      const_iterator cend() const noexcept {
        return std::apply(
          [](auto&&... vecs) {return iterator{ vecs.cend()... };},
          holding_);
      }
      void push_back(typename Members::value_type const&... members) noexcept {
        std::tuple<typename Members::value_type const&...> members_tuple{ members... };

        for_each_tuples(
          [](auto& vec, auto const& member) { vec.push_back(member);},
          holding_, members_tuple);
      }
      void push_back(typename Members::value_type&&... members) noexcept {
        std::tuple<typename Members::value_type&&...> members_tuple{ std::move(members)... };

        for_each_tuples(
          [](auto& vec, auto&& member) {vec.push_back(std::move(member));},
          holding_, members_tuple);
      }
      void push_back(reference const& refs) noexcept {
        for_each_tuples(
          [](auto& vec, auto const& ref) { vec.push_back(ref.get());},
          holding_, refs.get_underlying());
      }
      void push_back(reference&& refs) noexcept {
        for_each_tuples(
          [](auto& vec, auto&& ref) { vec.push_back(std::move(ref.get()));},
          holding_, refs.get_underlying());
      }
      iterator erase(iterator iter) noexcept {
        return std::apply(
          [](auto&&... new_iters) { return iterator{ std::move(new_iters)... };},
          for_each_tuples(
            [](auto& vec, auto const& iter) { return vec.erase(iter);},
            holding_, iter.get_underlying()));
      }
      iterator erase(const_iterator iter) noexcept {
        return std::apply(
          [](auto&&... new_iters) { return iterator{ std::move(new_iters)... };},
          for_each_tuples(
            [](auto& vec, auto const& iter) { return vec.erase(iter);},
            holding_, iter.get_underlying()));
      }
      iterator erase(iterator start, iterator end) noexcept {
        return std::apply(
          [](auto&&... new_iters) { return iterator{ std::move(new_iters)... };},
          ::dlib::for_each_tuples(
            [](auto& vec, auto const& start, auto const& end) { return vec.erase(start, end);},
            holding_, start.get_underlying(), end.get_underlying()));
      }
      iterator erase(const_iterator start, const_iterator end) noexcept {
        return std::apply(
          [](auto&&... new_iters) { return iterator{ std::move(new_iters)... };},
          for_each_tuples(
            [](auto& vec, auto const& start, auto const& end) { return vec.erase(stat, end);},
            holding_, start.get_underlying(), end.get_underlying()));
      }

      template<typename T>
      constexpr Array_view<T> view() noexcept {
        auto& vec = std::get<::dlib::soa_impl::Vector_impl<T>>(holding_);

        return { vec.data(), vec.size() };
      }

      template<typename T>
      constexpr Array_view<const T> view() const noexcept {
        auto const& vec = std::get<Vector_impl<T>>(holding_);

        return { vec.data(), vec.size() };
      }

      void move_element(iterator iter, Soa_impl& to) noexcept {
        to.push_back(std::move(*iter));
        erase(iter);
      }

      bool empty() const noexcept {
        return size() == 0;
      }

      size_type size() const noexcept {
        return std::get<0>(holding_).size();
      }

      void reserve(size_t i) noexcept {
        std::apply(
          [i](auto& vec) { vec.reserve(i); },
          holding_);
      }

      reference operator[](size_type i) noexcept {
        return std::apply(
          [i](auto&&... vecs) { return reference{ vecs[i]... };},
          holding_);
      }
      const_reference operator[](size_type i) const noexcept {
        return std::apply(
          [i](auto&&... vecs) { return const_reference{ vecs[i]... };},
          holding_);
      }
    private:
      std::tuple<Members...> holding_;
    };

    template<typename ...Members>
    using Aos_impl = soa_impl::Vector_impl<std::tuple<Members...>>;
  }
  
  template<typename ...Members>
  using Soa = soa_impl::Soa_impl<soa_impl::Vector_impl<Members>...>;

  template<typename ...Members>
  using Aos = soa_impl::Aos_impl<Members...>;
}