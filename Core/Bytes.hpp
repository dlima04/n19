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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
class ByteCopy {
static_assert(!IsReference<T>);
static_assert(!std::is_array_v<T>);
public:
  using ValueType      = T;
  using ReferenceType  = T&;
  using PointerType    = T*;

  [[nodiscard]] N19_FORCEINLINE auto value() const& -> const T& {
    ASSERT(is_active_ == true, "Bad bytecopy access!");
    return *std::launder<const T>( reinterpret_cast<const T*>(&value_) );
  }

  [[nodiscard]] N19_FORCEINLINE auto value() & -> T& {
    ASSERT(is_active_ == true, "Bad bytecopy access!");
    return *std::launder<T>( reinterpret_cast<T*>(&value_) );
  }

  [[nodiscard]] N19_FORCEINLINE auto value() && -> T {
    ASSERT(is_active_ == true, "Bad bytecopy access!");
    return release(); // For an Rvalue reference, release ownership.
  }

  [[nodiscard]] N19_FORCEINLINE auto bytes() const -> Bytes {
    return Bytes( reinterpret_cast<const Byte*>(&value_), sizeof(value_) );
  }

  N19_FORCEINLINE auto operator->(this auto &&self) -> decltype(auto) {
    ASSERT(self.is_active_ == true && "Bad bytecopy access!");
    return &( forward<decltype(self)>(self).value() );
  }

  N19_FORCEINLINE auto operator*(this auto &&self) -> decltype(auto) {
    ASSERT(self.is_active_ == true && "Bad bytecopy access!");
    return forward<decltype(self)>(self).value();
  }

  template<class O>
  N19_FORCEINLINE auto operator==(const ByteCopy<O>& other) -> bool {
    return is_active_ == other.is_active_ && (!is_active_ || value() == other.value());
  }

  template<class O>
  N19_FORCEINLINE auto operator==(const O& other) -> bool {
    return is_active_ && value() == other;
  }

  N19_FORCEINLINE auto operator=(ByteCopy&& other) -> ByteCopy& {
    clear();                        // Clear the existing value.
    is_active_ = other.is_active_;  // Change active state.
    if(other.is_active_)            //////////////////////////////
      std::construct_at<T>(reinterpret_cast<T*>(&value_), other.release());
    return *this;
  }

  N19_FORCEINLINE auto operator=(const ByteCopy& other) -> ByteCopy& {
    clear();                        // Clear the existing value.
    is_active_ = other.is_active_;  // Change active state
    if(other.is_active_)            //////////////////////////////
      std::construct_at<T>(reinterpret_cast<T*>(&value_), other.value());
    return *this;
  }

  template<class ... Args>
  N19_FORCEINLINE auto emplace(Args&&... args) -> void {
    clear();           // Clear the existing value.
    is_active_ = true; // maintain the value state.
    std::construct_at<T>( reinterpret_cast<T*>(&value_), forward<Args>(args)... );
  }

  template<class ... Args>
  N19_FORCEINLINE ByteCopy(Args&&... args) {
    std::construct_at<T>( reinterpret_cast<T*>(&value_), forward<Args>(args)... );
    is_active_ = true;
  }

  N19_FORCEINLINE ByteCopy(const ByteCopy& other) {
    if(!other.is_active_) return;
    std::construct_at<T>( reinterpret_cast<T*>(&value_), other.value() );
    is_active_ = true;
  }

  N19_FORCEINLINE ByteCopy(ByteCopy&& other) {
    if(!other.is_active_) return;
    std::construct_at<T>( reinterpret_cast<T*>(&value_), other.release() );
    is_active_ = true;
  }

  N19_FORCEINLINE auto clear() -> void {
    if(is_active_) value().~T();
    is_active_ = false;
  }

  N19_FORCEINLINE auto release() -> T {
    T released = std::move(value());
    value().~T();
    is_active_ = false;
    return released;
  }

  auto alive() const -> bool     { return is_active_; }
  explicit operator bool() const { return is_active_; }

  N19_FORCEINLINE ~ByteCopy() { clear(); }
  N19_FORCEINLINE ByteCopy()  { /*....*/ }
private:
  bool is_active_{false};
  alignas(T) uint8_t value_[ sizeof(T) ]{};
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Utility functions:

template<typename T>
N19_FORCEINLINE auto as_writable_bytes(T& val) -> WritableBytes {
  static_assert(std::ranges::contiguous_range<T>);
  return std::as_writable_bytes( std::span(val) );
}

template<typename T>
N19_FORCEINLINE auto as_bytes(const T& val) -> Bytes {
  static_assert(std::ranges::contiguous_range<T>);
  return std::as_bytes( std::span(val) );
}

template<typename T>
N19_FORCEINLINE auto as_bytecopy(const T& val) -> ByteCopy<T> {
  return ByteCopy<T>{ val };
}

template<typename T, typename ...Args>
N19_FORCEINLINE auto construct_bytecopy(Args&&... args) -> ByteCopy<T> {
  return ByteCopy<T>{ T{forward<Args>(args)...} };
}

END_NAMESPACE(n19);
#endif //BYTES_HPP
