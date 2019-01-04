#pragma once

#include <array>
#include <utility>
#include <type_traits>

#include <dlib/math.hpp>
#include <dlib/util.hpp>
#include <dlib/serialization.hpp>

namespace dlib::geometry {
  template<typename Distance, size_t n>
  class Vector {
  public:
    template<typename = std::enable_if_t<std::is_default_constructible_v<Distance>>>
    constexpr Vector() noexcept :
      data_{ 0 } {

    }

    Vector(Vector const&) = default;
    Vector(Vector&&) = default;

    constexpr Vector(std::initializer_list<Distance> in) {
      const size_t min = common::vmin(in.size(), data_.size());

      std::copy_n(in.begin(), min, data_.begin());
    }

    constexpr Vector& operator=(Vector const&) = default;
    constexpr Vector& operator=(Vector&&) = default;

    constexpr Vector& operator+=(Vector adding) noexcept {
      for (size_t i = 0; i < n; ++i) {
        data_[i] += adding[i];
      }

      return *this;
    }

    constexpr Vector operator+(Vector adding) const noexcept {
      Vector returning = *this;
      return returning += adding;
    }

    constexpr Vector& operator-=(Vector subbing) noexcept {
      for (int i = 0; i < n; ++i) {
        data_[i] -= subbing[i];
      }

      return *this;
    }

    constexpr Vector operator-(Vector subbing) const noexcept {
      Vector returning = *this;
      return returning -= subbing;
    }

    constexpr Vector operator-() const noexcept {
      Vector returning;

      for (size_t i = 0; i < n; ++i) {
        returning[i] = -data_[i];
      }

      return returning;
    }

    constexpr bool operator==(Vector const& other) const noexcept {
      return data_ == other.data_;
    }

    constexpr Distance& operator[](int i) noexcept {
      return data_[i];
    }
    constexpr Distance const& operator[](int i) const noexcept {
      return data_[i];
    }
    constexpr Distance& operator[](size_t i) noexcept {
      return data_[i];
    }
    constexpr Distance const& operator[](size_t i) const noexcept {
      return data_[i];
    }
  private:
    std::array<Distance, n> data_;
  };

  template<typename Cmp = std::less<>>
  class VectorCmp :
    private Cmp {
  public:
    template<typename Distance, size_t n>
    constexpr bool operator()(Vector<Distance, n> const& a, Vector<Distance, n> const& b) const noexcept {
      for (size_t i = 0; i < n; ++i) {
        const bool less = static_cast<Cmp const&>(*this)(a[i], b[i]);
        const bool greater = static_cast<Cmp const&>(*this)(b[i], a[i]);

        if (less && !greater) {
          return true;
        } else if (!less && greater) {
          return false;
        }
      }

      return false; //equal
    }
  };

  struct Column {
    const size_t val;
  };

  struct Row {
    const size_t val;
  };

  struct Major {
    const size_t val;
  };

  struct Minor {
    const size_t val;
  };

  namespace impl {
    template<typename Distance, size_t minor_size_, size_t major_size_>
    class Matrix {
    public:

      Distance& get(Major major, Minor minor) noexcept {
        return data_[major.val][minor.val];
      }

      Distance get(Major major, Minor minor) const noexcept {
        return data_[major.val][minor.val];
      }

      constexpr static size_t minor_size() noexcept {
        return minor_size_;
      }

      constexpr static size_t major_size() noexcept {
        return major_size_;
      }
    private:
      std::array<std::array<Distance, minor_size_>, major_size_> data_;
    };
  }

  template<typename Distance, size_t width_, size_t height_>
  class RowMajorMatrix :
    public impl::Matrix<Distance, width_, height_> {
  public:
    using impl::Matrix<Distance, width_, height_>::get;

    Distance& get(Column column, Row row) noexcept {
      return get(Major{ column.val }, Minor{ row.val });
    }

    Distance const& get(Column column, Row row) const noexcept {
      return get(Major{ column.val }, Minor{ row.val });
    }

    constexpr static size_t width() noexcept {
      return width_;
    }

    constexpr static size_t height() noexcept {
      return height_;
    }
  };

  template<typename Distance, size_t width_, size_t height_>
  class ColumnMajorMatrix :
    public impl::Matrix<Distance, height_, width_> {
  public:
    using impl::Matrix<Distance, height_, width_>::get;

    Distance& get(Column column, Row row) noexcept {
      return get(Major{ row.val }, Minor{ column.val });
    }

    Distance const& get(Column column, Row row) const noexcept {
      return get(Major{ row.val }, Minor{ column.val });
    }

    constexpr static size_t width() noexcept {
      return width_;
    }

    constexpr static size_t height() noexcept {
      return height_;
    }
  };

  template<typename Distance, size_t n, typename Scalar>
  Vector<Distance, n> scale(Vector<Distance, n> a, Scalar scalar) {
    Vector<Distance, n> returning;

    for (size_t i = 0; i < n; ++i) {
      returning[i] = a[i] * scalar;
    }

    return returning;
  }

  template<typename Distance, size_t n>
  Distance lengthSquared(Vector<Distance, n> a) {
    Distance returning = 0;
    for (size_t i = 0; i < n; ++i) {
      returning += a[i] * a[i];
    }

    return returning;
  }

  template<typename Distance, size_t n>
  Distance length(Vector<Distance, n> a) {
    return std::sqrt(lengthSquared(a));
  }

  template<typename Distance, size_t n>
  Distance dotScaled(Vector<Distance, n> a, Vector<Distance, n> b) {
    Distance returning = 0;

    for (size_t i = 0; i < n; ++i) {
      returning += a[i] * b[i];
    }

    return returning;
  }

  template<typename Distance, size_t n>
  Distance dot(Vector<Distance, n> a, Vector<Distance, n> b) {
    return dotScaled(a, b) / length(b);
  }

  template<typename Distance, size_t n>
  Vector<Distance, n> project(Vector<Distance, n> of, Vector<Distance, n> onto) {
    return scale(of, dot(of, onto) / dot(onto, onto));
  }

  template<template<typename, size_t, size_t> typename Matrix, typename Distance>
  constexpr Distance determinant(Matrix<Distance, 1, 1> matrix) noexcept {
    return matrix.get(Major{ 0 }, Minor{ 0 });
  }

  template<template<typename, size_t, size_t> typename Matrix, typename Distance, size_t n>
  constexpr Distance determinant(Matrix<Distance, n, n> matrix) noexcept {
    Distance returning = 0;

    Distance sign = 1;

    Matrix<Distance, n - 1, n - 1> child;
    for (size_t i = 0; i < n; ++i) {
      for (size_t x = 0; x < i; ++x) {
        for (size_t y = 1; y < n; ++y) {
          child.get(Major{ x }, Minor{ y - 1 }) = matrix.get(Major{ x }, Minor{ y });
        }
      }
      for (size_t x = i + 1; x < n; ++x) {
        for (size_t y = 1; y < n; ++y) {
          child.get(Major{ x - 1 }, Minor{ y - 1 }) = matrix.get(Major{ x }, Minor{ y });
        }
      }
      returning += sign * matrix.get(Major{ 0 }, Minor{ i }) * determinant(child);
      sign *= -1;
    }

    return returning;
  }

  template<typename Distance, size_t n>
  constexpr Vector<Distance, n> getOrthogonal(std::array<Vector<Distance, n>, n - 1> others) noexcept {

    Vector<Distance, n> returning;
    ColumnMajorMatrix<Distance, n - 1, n - 1> matrix;

    for (size_t i = 0; i < n; ++i) {
      for (size_t x = 0; x < i; ++x) {
        for (size_t y = 0; y < n - 1; ++y) {
          matrix.get(Major{ y }, Minor{ x }) = others[y][x];
        }
      }
      for (size_t x = i + 1; x < n; ++x) {
        for (size_t y = 0; y < n - 1; ++y) {
          matrix.get(Major{ y }, Minor{ x - 1 }) = others[y][x];
        }
      }
      returning[i] = determinant(matrix);
    }
    return returning;
  }
}

namespace dlib::serialization {
  template<typename Distance, size_t n, typename OutputIterator>
  OutputIterator serialize(OutputIterator iter, geometry::Vector<Distance, n> const& vector) noexcept {
    for (size_t i = 0; i < n; ++i) {
      iter = serialize(std::move(iter), vector[i]);
    }
    return iter;
  }

  template<typename Distance, size_t width, size_t height, typename OutputIterator>
  OutputIterator serialize(OutputIterator iter, geometry::RowMajorMatrix<Distance, width, height> const& matrix) {
    using Matrix = geometry::RowMajorMatrix<Distance, width, height>;
    constexpr auto major_size = Matrix::major_size();
    constexpr auto minor_size = Matrix::minor_size();

    for (size_t major = 0; major < major_size; ++major) {
      for (size_t minor = 0; minor < minor_size; ++minor) {
        auto writing = matrix.get(geometry::Major{ major }, geometry::Minor{ minor });
        iter = serialize(std::move(iter), std::move(writing));
      }
    }
    return iter;
  }

  template<typename Distance, size_t width, size_t height, typename OutputIterator>
  OutputIterator serialize(OutputIterator iter, geometry::ColumnMajorMatrix<Distance, width, height> const& matrix) {
    using Matrix = geometry::ColumnMajorMatrix<Distance, width, height>;
    constexpr auto major_size = Matrix::major_size();
    constexpr auto minor_size = Matrix::minor_size();

    for (size_t major = 0; major < major_size; ++major) {
      for (size_t minor = 0; minor < minor_size; ++minor) {
        auto writing = matrix.get(Major{ major }, Minor{ minor });
        iter = serialize(std::move(iter), std::move(writing));
      }
    }
    return iter;
  }

  template<typename Distance, size_t n, typename InputIterator, typename EndIterator>
  Result<Deserialization<geometry::Vector<Distance, n>, InputIterator>> deserialize(Type_arg<geometry::Vector<Distance, n>>, InputIterator iter, EndIterator end) noexcept {
    return deserialize_aggregate<n, geometry::Vector<Distance, n>, Distance>(std::move(iter), std::move(end));
  }

  template<typename Distance, size_t width, size_t height, typename InputIterator, typename EndIterator>
  Result<Deserialization<geometry::RowMajorMatrix<Distance, width, height>, InputIterator>> deserialize(Type_arg<geometry::RowMajorMatrix<Distance, width, height>>, InputIterator iter, EndIterator end) noexcept {

    using Matrix = geometry::RowMajorMatrix<Distance, width, height>;
    constexpr auto major_size = Matrix::major_size();
    constexpr auto minor_size = Matrix::minor_size();

    Matrix matrix;

    for (size_t major = 0; major < major_size; ++major) {
      for (size_t minor = 0; minor < minor_size; ++minor) {
        OUTCOME_TRY(read, (deserialize(type_arg<Distance>, std::move(iter), end)));
        iter = std::move(read.iter);
        matrix.get(geometry::Major{ major }, geometry::Minor{ minor }) = std::move(read.val);
      }
    }
    return Deserialization<Matrix, InputIterator>{std::move(matrix), std::move(iter)};
  }

  template<typename Distance, size_t width, size_t height, typename InputIterator, typename EndIterator>
  Result<Deserialization<geometry::ColumnMajorMatrix<Distance, width, height>, InputIterator>> deserialize(Type_arg<geometry::ColumnMajorMatrix<Distance, width, height>>, InputIterator iter, EndIterator end) noexcept {

    using Matrix = geometry::ColumnMajorMatrix<Distance, width, height>;
    constexpr auto major_size = Matrix::major_size();
    constexpr auto minor_size = Matrix::minor_size();

    Matrix matrix;

    for (size_t major = 0; major < major_size; ++major) {
      for (size_t minor = 0; minor < minor_size; ++minor) {
        OUTCOME_TRY(read, (deserialize(type_arg<Distance>, std::move(iter), end)));
        iter = std::move(read.iter);
        matrix.get(geometry::Major{ major }, geometry::Minor{ minor }) = std::move(read.val);
      }
    }
    return Deserialization<Matrix, InputIterator>{std::move(matrix), std::move(iter)};
  }
}