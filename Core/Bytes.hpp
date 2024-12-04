/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef BYTES_HPP
#define BYTES_HPP
#include <Core/Panic.hpp>
#include <cstddef>
#include <concepts>
#include <type_traits>
#include <span>
#include <array>
#include <cstring>

namespace n19 {
  using Byte = std::byte;
  using WritableBytes = std::span<Byte>;
  using Bytes = std::span<const Byte>;

  template<typename T> auto as_writable_bytes(T& val)        -> WritableBytes;
  template<typename T> auto as_bytes(const T& val)           -> Bytes;
  template<typename T> auto as_scalar_bytecopy(const T& val) -> std::array<Byte, sizeof(T)>;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
auto n19::as_writable_bytes(T& val) -> WritableBytes {
  static_assert(std::ranges::contiguous_range<T>);
  return std::as_writable_bytes( std::span(val) );
}

template<typename T>
auto n19::as_bytes(const T& val) -> Bytes {
  static_assert(std::ranges::contiguous_range<T>);
  return std::as_bytes( std::span(val) );
}

template<typename T>
auto n19::as_scalar_bytecopy(const T& val) -> std::array<Byte, sizeof(T)> {
  static_assert(std::is_trivially_constructible_v<T>);
  std::array<Byte, sizeof(T)> arr;
  const std::span bytes( (std::byte*)&val, sizeof(T) );

  // Copy the span into the array
  ASSERT(bytes.size_bytes() == sizeof(T));
  std::memcpy(arr.data(), bytes.data(), sizeof(T));
  return arr;
}

#endif //BYTES_HPP
