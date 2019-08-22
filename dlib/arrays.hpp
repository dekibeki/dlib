#pragma once

#include <array>
#include <vector>
#include <algorithm>
#include <iterator>
#include <type_traits>
#include <dlib/math.hpp>

namespace dlib {
  namespace arrays_impl {
    template<typename T>
    constexpr bool is_valid_array_view_container = true;
  }

  template<typename T_>
  class Array_view {
  public:
    using value_type = T_;
    using size_type = std::size_t;
    using reference = value_type & ;
    using const_reference = value_type const&;
    using pointer = value_type * ;
    using const_pointer = const value_type*;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    constexpr  Array_view() noexcept :
      ptr_{ nullptr },
     size_{ 0 } {
    
    }

    constexpr Array_view(nullptr_t) noexcept :
      ptr_{ nullptr },
      size_{ 0 } {

    }

    constexpr Array_view(pointer ptr, size_type size) noexcept :
      ptr_{ std::move(ptr) },
      size_{ size } {

    }

    template<typename Container, typename = decltype(std::declval<Container>().data()), typename = decltype(std::declval<Container>().size())>
    constexpr Array_view(Container&& container) noexcept :
      Array_view{ container.data(), container.size() } {

    }

    template<size_t n>
    constexpr Array_view(value_type(&p)[n]) noexcept :
      Array_view{ +p, n } {

    }

    constexpr pointer data() const noexcept {
      return &ptr_[0];
    }

    constexpr bool empty() const noexcept {
      return size() == 0;
    }

    constexpr size_type size() const noexcept {
      return size_;
    }

    constexpr size_type max_size() const noexcept {
      return size();
    }

    constexpr void fill(const_reference value) noexcept {
      for (auto& ref : *this) {
        ref = value;
      }
    }

    constexpr reference operator[](size_type i) noexcept {
      return data()[i];
    }
    constexpr const_reference operator[](size_type i) const noexcept {
      return data()[i];
    }

    constexpr reference at(size_type i) noexcept {
      if (i < 0 || i >= size_) {
        //ERROR
      }
      return data()[i];
    }
    constexpr const_reference at(size_type i) const noexcept {
      if (i < 0 || i >= size_) {
        //ERROR
      }
      return data()[i];
    }

    constexpr reference front() noexcept {
      return data()[0];
    }

    constexpr const_reference front() const noexcept {
      return data()[0];
    }

    constexpr reference back() noexcept {
      return data()[size() - 1];
    }

    constexpr const_reference back() const noexcept {
      return data()[size() - 1];
    }

    constexpr iterator begin() noexcept {
      return data();
    }
    constexpr const_iterator begin() const noexcept {
      return data();
    }
    constexpr const_iterator cbegin() const noexcept {
      return data();
    }
    constexpr iterator end() noexcept {
      return data() + size_;
    }
    constexpr const_iterator end() const noexcept {
      return data() + size_;
    }
    constexpr const_iterator cend() const noexcept {
      return data() + size_;
    }

    constexpr void pop_back() noexcept {
      *this = subarray(0, size() - 1);
    }

    constexpr void erase(iterator pos) noexcept {
      if (size() > 1) {
        std::swap(*pos, back());
      }
      pop_back();
    }
    constexpr void erase(iterator begin, iterator end) noexcept {
      if (begin == end) {
        return;
      }
      
      const auto container_end = cend();

      while (container_end != end) {
        std::iter_swap(begin, end);
        ++begin;
        ++end;
      }

      *this = subarray(0, size() - std::distance(begin, end));
    }

    constexpr Array_view subarray_unsafe(size_type offset, size_type len) const noexcept {
      return Array_view{ data() + offset, len };
    }

    constexpr Array_view subarray_safe(size_type offset, size_type len) const noexcept {
      const auto cur_size = size();
      offset = vmin(offset, cur_size);
      len = vmin(len, cur_size - offset);
      return subarray_unsafe(offset, len);
    }

    constexpr Array_view subarray(size_type offset, size_type len) const noexcept {
      return subarray_safe(offset, len);
    }

    constexpr Array_view subarray_unsafe(size_type offset) const noexcept {
      return subarray_unsafe(offset, size() - offset);
    }

    constexpr Array_view subarray_safe(size_type offset) const noexcept {
      return subarray_safe(offset, size() - offset);
    }

    constexpr Array_view subarray(size_type offset) const noexcept {
      return subarray(offset, size() - offset);
    }

    constexpr operator Array_view<const T_>() const noexcept {
      return Array_view<const T_>(data(), size());
    }
  private:
    pointer ptr_;
    size_type size_;
  };

  template<typename Container>
  Array_view(Container&&) noexcept -> Array_view<typename std::decay_t<Container>::value_type>;
}