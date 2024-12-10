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
#include <array>
#include <cstdint>
#include <cstring>
#include <new>
#include <memory>
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

  [[nodiscard]] auto alive() const  -> bool;
  [[nodiscard]] auto value() const& -> ReferenceType;
  [[nodiscard]] auto value() &&     -> ValueType;
  [[nodiscard]] auto value() &      -> ReferenceType;
  [[nodiscard]] auto bytes() const  -> Bytes;

  auto operator->(this auto&& self) -> decltype(auto);
  auto operator*(this auto&& self)  -> decltype(auto);

  template<class O> auto operator==(const ByteCopy<O>&) -> bool;
  template<class O> auto operator==(const O&)           -> bool;

  auto operator=(ByteCopy&& other)      -> ByteCopy&;
  auto operator=(const ByteCopy& other) -> ByteCopy&;

  auto clear()   -> void;
  auto release() -> T;

  template<class ...Args>
  explicit ByteCopy(Args&&... args);
  explicit ByteCopy(const ByteCopy& other);
  explicit ByteCopy(ByteCopy&& other);

  N19_FORCEINLINE ~ByteCopy() { clear(); }
  N19_FORCEINLINE ByteCopy()  { /*....*/ }
private:
  bool is_active_ = false;
  alignas(T) uint8_t value_[ sizeof(T) ];
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Methods for n19::ByteCopy:

template<class T>
N19_FORCEINLINE auto ByteCopy<T>::alive() const -> bool {
  return is_active_;
}

template<class T>
N19_FORCEINLINE auto ByteCopy<T>::value() const& -> ReferenceType {
  ASSERT(is_active_ == true && "Bad bytecopy access!");
  return *std::launder<T>( reinterpret_cast<T*>(&value_) );
}

template<class T>
N19_FORCEINLINE auto ByteCopy<T>::value() & -> ReferenceType {
  ASSERT(is_active_ == true && "Bad bytecopy access!");
  return *std::launder<T>( reinterpret_cast<T*>(&value_) );
}

template<class T>
N19_FORCEINLINE auto ByteCopy<T>::value() && -> ValueType {
  ASSERT(is_active_ == true && "Bad bytecopy access!");
  return release(); // For an Rvalue reference, release ownership.
}

template<class T>
N19_FORCEINLINE auto ByteCopy<T>::bytes() const -> Bytes {
  return Bytes( reinterpret_cast<const Byte*>(&value_), sizeof(value_) );
}

template<class T> template<class O>
N19_FORCEINLINE auto ByteCopy<T>::operator==(const ByteCopy<O>& other) -> bool {
  return is_active_ == other.is_active_ && (!is_active_ || value() == other.value());
}

template<class T> template<class O>
N19_FORCEINLINE auto ByteCopy<T>::operator==(const O& other) -> bool {
  return is_active_ && value() == other;
}

template<class T>
N19_FORCEINLINE auto ByteCopy<T>::operator->(this auto &&self) -> decltype(auto) {
  ASSERT(self.is_active_ == true && "Bad bytecopy access!");
  return &( forward<decltype(self)>(self).value() );
}

template<class T>
N19_FORCEINLINE auto ByteCopy<T>::operator*(this auto &&self) -> decltype(auto) {
  ASSERT(self.is_active_ == true && "Bad bytecopy access!");
  return forward<decltype(self)>(self).value();
}

template<class T>
N19_FORCEINLINE auto ByteCopy<T>::operator=(ByteCopy&& other) -> ByteCopy& {
  if(is_active_) {
    clear();
  } if(other.is_active_) {
    std::construct_at<T>(reinterpret_cast<T*>(&value_), other.release());
    is_active_ = true;
  }
  return *this;
}

template<class T>
N19_FORCEINLINE auto ByteCopy<T>::operator=(const ByteCopy& other) -> ByteCopy& {
  if(is_active_) {
    clear();
  } if(other.is_active_) {
    std::construct_at<T>(reinterpret_cast<T*>(&value_), other.value());
    is_active_ = true;
  }
  return *this;
}

template<class T>
N19_FORCEINLINE auto ByteCopy<T>::clear() -> void {
  if(!is_active_) return;
  value().~T();
  is_active_ = false;
}

template<class T>
N19_FORCEINLINE auto ByteCopy<T>::release() -> T {
  ASSERT(is_active_ == true);
  T released = std::move(value());
  value().~T();
  is_active_ = false;
  return released;
}

template<class T> template<class ... Args>
N19_FORCEINLINE ByteCopy<T>::ByteCopy(Args&&... args) {
  std::construct_at<T>( reinterpret_cast<T*>(&value_), forward<Args>(args)... );
  is_active_ = true;
}

template<class T>
N19_FORCEINLINE ByteCopy<T>::ByteCopy(const ByteCopy& other) {
  if(!other.is_active_) return;
  std::construct_at<T>( reinterpret_cast<T*>(&value_), other.value() );
  is_active_ = true;
}

template<class T>
N19_FORCEINLINE ByteCopy<T>::ByteCopy(ByteCopy&& other) {
  if(!other.is_active_) return;
  std::construct_at<T>( reinterpret_cast<T*>(&value_), other.release() );
  is_active_ = true;
}

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
  return ByteCopy<T>{ forward<Args>(args)... };
}

END_NAMESPACE(n19);
#endif //BYTES_HPP
