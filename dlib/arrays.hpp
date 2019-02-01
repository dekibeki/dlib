#pragma once

#include <array>
#include <vector>
#include <algorithm>
#include <type_traits>
#include <dlib/math.hpp>

namespace dlib {
  namespace arrays_impl {
    template<typename T_, typename Holding_pointer_>
    class Basic_array_view {
    public:
      using Holding_pointer = Holding_pointer_;
      using Size_arg = size_t;
      using Holding_arg = T_;

      using value_type = Holding_arg;
      using size_type = Size_arg;
      using reference = Holding_arg & ;
      using const_reference = Holding_arg const&;
      using pointer = Holding_arg * ;
      using const_pointer = const Holding_arg*;
      using iterator = pointer;
      using const_iterator = const_pointer;
      using reverse_iterator = std::reverse_iterator<iterator>;
      using const_reverse_iterator = std::reverse_iterator<const_iterator>;

      constexpr Basic_array_view() noexcept :
        ptr_{ nullptr },
        size_{ 0 } {

      }

      constexpr Basic_array_view(nullptr_t) noexcept :
        ptr_{ nullptr },
        size_{ 0 } {

      }

      constexpr Basic_array_view(Holding_pointer ptr, size_type size) noexcept :
        ptr_{ std::move(ptr) },
        size_{ size } {

      }

      constexpr pointer data() const noexcept {
        return &ptr_[0];
      }

      constexpr bool empty() const noexcept {
        return size() == 0;
      }

      constexpr Size_arg size() const noexcept {
        return size_;
      }

      constexpr Size_arg max_size() const noexcept {
        return size();
      }

      constexpr void fill(const_reference value) noexcept {
        for (Size_arg i = 0; i < size_; ++i) {
          data()[i] = value;
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
      protected:

    private:
      Holding_pointer ptr_;
      size_type size_;
    };
  }

  template<typename T>
  class Dynamic_array :
    public arrays_impl::Basic_array_view<T, std::unique_ptr<T[]>> {
  public:
    using value_type = typename Basic_array_view::value_type;
    using size_type = typename Basic_array_view::size_type;

    constexpr Dynamic_array(nullptr_t) noexcept :
      Basic_array_view{ nullptr } {

    }

    constexpr Dynamic_array(size_type n) noexcept :
      Basic_array_view{ std::make_unique<T[]>(n), n } {

    }
  };

  template<typename T>
  class Array_view :
    public arrays_impl::Basic_array_view<T, T*> {
  public:
    using Basic_array_view::Basic_array_view;
    using value_type = typename Basic_array_view::value_type;
    using size_type = typename Basic_array_view::size_type;
    
    static constexpr bool t_is_const = std::is_const_v<T>;
    using Unconst_t = std::remove_const_t<T>;

    constexpr Array_view(value_type& single) noexcept :
      Basic_array_view{ &single,1 } {

    }

    template<size_t n, typename = std::enable_if_t<t_is_const>>
    constexpr Array_view(Unconst_t(&data)[n]) noexcept :
      Basic_array_view{ +data, n } {
    }
    template<size_t n>
    constexpr Array_view(value_type(&data)[n]) noexcept :
      Basic_array_view{ +data, n } {

    }

    template<size_t n>
    constexpr Array_view(std::array<value_type, n>& data) noexcept :
      Basic_array_view{ data.data(), data.size() } {
    }

    template<size_t n, typename = std::enable_if_t<t_is_const>>
    constexpr Array_view(std::array<Unconst_t, n>& data) noexcept :
      Basic_array_view{ data.data(), data.size() } {
    }

    constexpr Array_view(std::vector<value_type>& data) noexcept :
      Basic_array_view{ data.data(), data.size() } {

    }

    template<typename = std::enable_if_t<t_is_const>>
    constexpr Array_view(std::vector<Unconst_t>& data) noexcept :
      Basic_array_view{ data.data(), data.size() } {

    }

    constexpr Array_view(Dynamic_array<value_type>& data) noexcept :
      Basic_array_view{ data.data(), data.size() } {

    }

    template<typename = std::enable_if_t<t_is_const>>
    constexpr Array_view(Dynamic_array<Unconst_t>& data) noexcept :
      Basic_array_view{ data.data(), data.size() } {

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
    
    constexpr operator Array_view<const T>() const noexcept {
      return Array_view<const T>(data(), size());
    }
  };
}