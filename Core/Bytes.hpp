/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <Core/Common.hpp>
#include <Core/Panic.hpp>
#include <Core/Concepts.hpp>
#include <Core/Platform.hpp>
#include <cstddef>
#include <concepts>
#include <type_traits>
#include <span>
#include <cstdint>
#include <cstring>
#include <memory>
#include <new>
#include <utility>
BEGIN_NAMESPACE(n19);

using Byte           = std::byte;
using Bytes          = std::span<const Byte>;
using WritableBytes  = std::span<Byte>;

template<Concrete T>
class ByteCopy final {
  static_assert(!std::is_array_v<T>);
public:
  using ValueType      = T;
  using ReferenceType  = T&;
  using PointerType    = T*;

  NODISCARD_ FORCEINLINE_ auto value() const& -> const T& {
    ASSERT(is_active_ == true, "Bad bytecopy access!");
    return *std::launder<const T>( reinterpret_cast<const T*>(&value_) );
  }

  NODISCARD_ FORCEINLINE_ auto value() & -> T& {
    ASSERT(is_active_ == true, "Bad bytecopy access!");
    return *std::launder<T>( reinterpret_cast<T*>(&value_) );
  }

  NODISCARD_ FORCEINLINE_ auto value() && -> T {
    ASSERT(is_active_ == true, "Bad bytecopy access!");
    return release(); // For an Rvalue reference, release ownership.
  }

  NODISCARD_ FORCEINLINE_ auto bytes() const -> Bytes {
    return Bytes( reinterpret_cast<const Byte*>(&value_), sizeof(value_) );
  }

  FORCEINLINE_ auto operator->(this auto &&self) -> decltype(auto) {
    ASSERT(self.is_active_ == true && "Bad bytecopy access!");
    return &( std::forward<decltype(self)>(self).value() );
  }

  FORCEINLINE_ auto operator*(this auto &&self) -> decltype(auto) {
    ASSERT(self.is_active_ == true && "Bad bytecopy access!");
    return std::forward<decltype(self)>(self).value();
  }

  template<typename O>
  FORCEINLINE_ auto operator==(const ByteCopy<O>& other) -> bool {
    return is_active_ == other.is_active_ && (!is_active_ || value() == other.value());
  }

  template<typename O>
  FORCEINLINE_ auto operator==(const O& other) -> bool {
    return is_active_ && value() == other;
  }

  FORCEINLINE_ auto operator=(ByteCopy&& other) noexcept -> ByteCopy& {
    if(&other == this) return *this;
    clear();                        /// Clear the existing value.
    is_active_ = other.is_active_;  /// Change active state.
    if(other.is_active_)            ///
      std::construct_at<T>(reinterpret_cast<T*>(&value_), other.release());

    return *this;
  }

  FORCEINLINE_ auto operator=(const ByteCopy& other) -> ByteCopy& {
    if(&other == this) return *this;
    clear();                        /// Clear the existing value.
    is_active_ = other.is_active_;  /// Change active state
    if(other.is_active_)            ///
      std::construct_at<T>(reinterpret_cast<T*>(&value_), other.value());

    return *this;
  }

  template<typename ...Args> requires std::constructible_from<T, Args...>
  FORCEINLINE_ ByteCopy(Args&&... args) {
    std::construct_at<T>( reinterpret_cast<T*>(&value_), std::forward<Args>(args)... );
    is_active_ = true;
  }

  FORCEINLINE_ ByteCopy(const ByteCopy& other) {
    if(!other.is_active_) return;
    std::construct_at<T>( reinterpret_cast<T*>(&value_), other.value() );
    is_active_ = true;
  }

  FORCEINLINE_ ByteCopy(ByteCopy&& other) {
    if(!other.is_active_) return;
    std::construct_at<T>( reinterpret_cast<T*>(&value_), other.release() );
    is_active_ = true;
  }

  FORCEINLINE_ auto clear() -> void {
    if(is_active_) value().~T();
    is_active_ = false;
  }

  FORCEINLINE_ auto release() -> T {
    T released = std::move(value());
    value().~T();
    is_active_ = false;
    return released;
  }

  auto alive() const -> bool     { return is_active_; }
  explicit operator bool() const { return is_active_; }

  FORCEINLINE_ ~ByteCopy() { clear(); }
  FORCEINLINE_ ByteCopy()  { /*....*/ }
private:
  bool is_active_{false};
  alignas(T) uint8_t value_[ sizeof(T) ]{};
};

template<typename T>
FORCEINLINE_ auto as_writable_bytes(T& val) -> WritableBytes {
  static_assert(std::ranges::contiguous_range<T>);
  return std::as_writable_bytes( std::span(val) );
}

template<typename T>
FORCEINLINE_ auto as_bytes(const T& val) -> Bytes {
  static_assert(std::ranges::contiguous_range<T>);
  return std::as_bytes( std::span(val) );
}

template<typename T>
FORCEINLINE_ auto as_bytecopy(const T& val) -> ByteCopy<T> {
  return ByteCopy<T>{ val };
}

template<typename T, typename ...Args>
FORCEINLINE_ auto construct_bytecopy(Args&&... args) -> ByteCopy<T> {
  return ByteCopy<T>{ T{std::forward<Args>(args)...} };
}

END_NAMESPACE(n19);
