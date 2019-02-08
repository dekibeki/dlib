#pragma once

#include <cstdint>
#include <cstddef>
#include <array>
#include <vector>
#include <type_traits>
#include <algorithm>
#include <utility>
#include <cstring>

#include <dlib/outcome.hpp>
#include <dlib/meta.hpp>
#include <dlib/arrays.hpp>

namespace dlib {
  template<typename T, typename Iterator>
  struct Deserialization {
    T val;
    Iterator iter;
  };
  namespace serialization {
    namespace impl {
      struct Dummy_proxy {
        void operator=(std::byte) const noexcept {

        }
      };

      class Counting_output_iterator {
      public:
        constexpr Counting_output_iterator() noexcept :
          count_{ 0 } {

        }
        Dummy_proxy operator*() noexcept {
          return {};
        }
        Counting_output_iterator& operator++() noexcept {
          ++count_;
          return *this;
        }
        Counting_output_iterator operator++(int) noexcept {
          Counting_output_iterator returning{ *this };
          ++count_;
          return returning;
        }
        constexpr size_t count() const noexcept {
          return count_;
        }
      private:
        size_t count_;
      };

      template<typename T>
      constexpr bool can_memcpy_serialize = std::is_trivially_copyable_v<T> && !std::is_pointer_v<T>;
    }

    template<typename OutputIterator>
    OutputIterator serialize(OutputIterator iter, std::byte datum) noexcept {
      *iter++ = datum;
      return iter;
    }

    template<typename OutputIterator, typename T, typename = std::enable_if_t<impl::can_memcpy_serialize<T>>>
    OutputIterator serialize(OutputIterator iter, T const& datum) noexcept {
      std::array<std::byte, sizeof(T)> binary;
      std::memcpy(binary.data(), &datum, sizeof(T));
      return ::dlib::serialization::serialize(std::move(iter), binary);
    }

    template<typename OutputIterator, typename T, size_t n>
    OutputIterator serialize(OutputIterator iter, const T(&data)[n]) noexcept {
      for (auto const& datum : data) {
        iter = ::dlib::serialization::serialize(std::move(iter), datum);
      }
      return iter;
    }

    template<typename OutputIterator, typename T, size_t n>
    OutputIterator serialize(OutputIterator iter, std::array<T, n> const& data) noexcept {
      for (T const& datum : data) {
        iter = ::dlib::serialization::serialize(std::move(iter), datum);
      }
      return iter;
    }

    template<typename OutputIterator, typename T>
    OutputIterator serialize(OutputIterator iter, std::vector<T> const& data) noexcept {
      iter = ::dlib::serialization::serialize(std::move(iter), data.size());
      for (T const& datum : data) {
        iter = ::dlib::serialization::serialize(std::move(iter), datum);
      }
      return iter;
    }

    template<typename OutputIterator, typename First, typename Second, typename ...T>
    OutputIterator serialize(OutputIterator iter, First const& first, Second const& second, T const&... rest) noexcept {
      return ::dlib::serialization::serialize(::dlib::serialization::serialize(std::move(iter), first), second, rest...);
    }

    template<typename ...T>
    size_t serialization_size(T const&... data) {
      return ::dlib::serialization::serialize(impl::Counting_output_iterator{}, data...).count();
    }

    template<typename T, typename InputIterator, typename EndIterator>
    Result<Deserialization<T, InputIterator>> deserialize(InputIterator iter, EndIterator end) noexcept {
      return ::dlib::serialization::deserialize(std::move(iter), std::move(end), type_arg<T>);
    }

    template<typename T, typename InputIterator, typename EndIterator, typename = std::enable_if_t<impl::can_memcpy_serialize<T>>>
    Result<Deserialization<T, InputIterator>> deserialize(InputIterator start, EndIterator end, Type_arg<T>) noexcept {
      std::array<std::byte, sizeof(T)> binary;
      for (auto& byte : binary) {
        DLIB_TRY(read, (::dlib::serialization::deserialize<std::byte>(start, end)));
        start = read.iter;
        byte = read.val;
      }

      return Deserialization<T, InputIterator>{std::move(reinterpret_cast<T&>(*binary.data())), std::move(start)};
    }

    template<typename InputIterator, typename EndIterator>
    Result<Deserialization<std::byte, InputIterator>> deserialize(InputIterator start, EndIterator end, Type_arg<std::byte>) noexcept {
      std::byte byte{ 0 };
      if (start != end) {
        byte = *start++;
      } else {
        return Errors::buffer_too_small;
      }

      return Deserialization<std::byte, InputIterator>{byte, std::move(start)};
    }

    template<typename T, typename InputIterator, typename EndIterator>
    Result<Deserialization<std::vector<T>, InputIterator>> deserialize(InputIterator start, EndIterator end, Type_arg<std::vector<T>>) noexcept {
      DLIB_TRY(size, (::dlib::serialization::deserialize<typename std::vector<T>::size_type>(std::move(start), end)));
      start = std::move(size.iter);
      std::vector<T> returning;
      returning.reserve(size.val);
      for (size_t i = 0; i < size.val; ++i) {
        DLIB_TRY(new_val, (::dlib::serialization::deserialize<T>(std::move(start), end)));
        start = std::move(new_val.iter);
        returning.emplace_back(std::move(new_val.val));
      }

      return Deserialization<std::vector<T>, InputIterator>{std::move(returning), std::move(start)};
    }

    template<size_t target, typename Aggregate, typename T, typename InputIterator, typename EndIterator, typename ...Waiting>
    Result<Deserialization<Aggregate, InputIterator>> deserialize_aggregate(InputIterator start, EndIterator end, Waiting&&... waiting) noexcept {

      constexpr auto on = sizeof...(Waiting);
      if constexpr (on == target) {
        return Deserialization<Aggregate, InputIterator>{ { std::move(waiting)... }, std::move(start)};
      } else {
        DLIB_TRY(new_val, (::dlib::serialization::deserialize<T>(std::move(start), end)));
        return deserialize_aggregate<target, Aggregate, T>(std::move(new_val.iter), std::move(end), std::forward<Waiting>(waiting)..., std::move(new_val.val));
      }
    }

    template<typename T, size_t n, typename InputIterator, typename EndIterator>
    Result<Deserialization<std::array<T, n>, InputIterator>> deserialize(InputIterator start, EndIterator end, Type_arg<std::array<T, n>>) noexcept {
      return deserialize_aggregate<n, std::array<T, n>, T>(std::move(start), std::move(end));
    }

    template<typename T, size_t n, typename InputIterator, typename EndIterator>
    Result<Deserialization<T[n], InputIterator>> deserialize(InputIterator start, EndIterator end, Type_arg<T[n]>) noexcept {
      return deserialize_aggregate<n, T[n], T>(std::move(start), std::move(end));
    }
  }

  template<typename OutputIterator, typename ...T>
  OutputIterator serialize(OutputIterator iter, T const&... rest) noexcept {
    return serialization::serialize(std::move(iter), rest...);
  }

  template<typename T, typename InputIterator, typename EndIterator>
  Result<Deserialization<T, InputIterator>> deserialize(InputIterator iter, EndIterator end) noexcept {
    return serialization::deserialize(std::move(iter), std::move(end));
  }
}