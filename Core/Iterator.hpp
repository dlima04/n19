/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <Core/Common.hpp>
#include <utility>
#include <iterator>
#include <cstdint>
BEGIN_NAMESPACE(n19);

template<typename T>
class BasicIterator {
public:
  using PointerType   = T*;
  using ReferenceType = T&;
  using ValueType     = T;

  constexpr auto operator*(this auto&& self) -> auto&& {
    return *std::forward<decltype(self)>(self).ptr_;
  }

  constexpr auto operator->(this auto&& self) -> auto* {
    return std::forward<decltype(self)>(self).ptr_;
  }

  constexpr auto operator++() -> BasicIterator& {
    ++ptr_;        /// Simply increment the internal pointer
    return *this;  /// and return self.
  }

  constexpr auto operator++(int) -> BasicIterator {
    BasicIterator temp = *this;
    ++(*this);     /// Modify self, return the previous
    return temp;   /// incremented value.
  }

  constexpr auto operator--() -> BasicIterator& {
    --ptr_;        /// Simply decrement the internal pointer
    return *this;  /// and return self.
  }

  constexpr auto operator--(int) -> BasicIterator {
    BasicIterator temp = *this;
    --(*this);     /// Modify self, return the previous
    return temp;   /// incremented value.
  }

  constexpr BasicIterator& operator+=(ptrdiff_t n) {
    ptr_ += n;     /// increment ptr_ by n.
    return *this;  /// and return self.
  }

  constexpr BasicIterator& operator-=(ptrdiff_t n) {
    ptr_ -= n;     /// decrement ptr_ by n.
    return *this;  /// and return self.
  }

  constexpr BasicIterator operator+(ptrdiff_t n) const {
    return BasicIterator(ptr_ + n);
  }

  constexpr BasicIterator operator-(ptrdiff_t n) const {
    return BasicIterator(ptr_ - n);
  }

  constexpr ptrdiff_t operator-(const BasicIterator& other) const {
    return ptr_ - other.ptr_;
  }

  constexpr auto operator<=>(const BasicIterator& other) const = default;

  constexpr BasicIterator(PointerType ptr) : ptr_(ptr) {}
  constexpr BasicIterator(BasicIterator&&) = default;
  constexpr BasicIterator(const BasicIterator&) = default;
  constexpr BasicIterator() = default;
protected:
  PointerType ptr_{};
};

END_NAMESPACE(n19);
