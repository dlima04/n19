/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef BYTES_H
#define BYTES_H
#include <Core/Panic.h>
#include <cstddef>
#include <concepts>
#include <type_traits>
#include <span>
#include <array>
#include <cstring>

namespace n19 {
  using Byte = std::byte;
  using ByteView = std::span<Byte>;

  template<typename T>
  auto as_bytes(const T& val) -> std::span<Byte>;

  template<typename T>
  auto as_scalar_bytecopy(const T& val) -> std::array<Byte, sizeof(T)>;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
auto n19::as_bytes(const T &val) -> std::span<Byte> {
  static_assert(std::constructible_from<std::span<T>, T>);
  return std::as_bytes( std::span<T>(val) );
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

#endif //BYTES_H
