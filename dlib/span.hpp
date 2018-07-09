#pragma once

#include <cstddef>
#include <gsl/span>

namespace dlib {
  using Byte = std::byte;
  template<typename T>
  using Span = gsl::span<T>;

  using ByteSpan = Span<Byte>;
}