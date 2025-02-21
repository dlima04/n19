/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef ITERATOR_HPP
#define ITERATOR_HPP
#include <Core/Platform.hpp>
#include <Core/ClassTraits.hpp>
#include <utility>
BEGIN_NAMESPACE(n19);

template<typename T>
class BasicIterator {
N19_MAKE_DEFAULT_CONSTRUCTIBLE(BasicIterator);
N19_MAKE_SPACESHIP(BasicIterator);
public:
  using PointerType   = T*;
  using ReferenceType = T&;
  using ValueType     = T;

  FORCEINLINE_ auto operator*(this auto&& self) -> auto&& {
    return *std::forward<decltype(self)>(self).ptr_;
  }

  FORCEINLINE_ auto operator->(this auto&& self) -> auto* {
    return std::forward<decltype(self)>(self).ptr_;
  }

  FORCEINLINE_ auto operator++() -> BasicIterator& {
    ++ptr_;        /// Simply increment the internal pointer
    return *this;  /// and return self.
  }

  FORCEINLINE_ auto operator++(int) -> BasicIterator {
    BasicIterator temp = *this;
    ++(*this);     /// Modify self, return the previous
    return temp;   /// incremented value.
  }

  FORCEINLINE_ auto operator--() -> BasicIterator& {
    --ptr_;        /// Simply decrement the internal pointer
    return *this;  /// and return self.
  }

  FORCEINLINE_ auto operator--(int) -> BasicIterator {
    BasicIterator temp = *this;
    --(*this);     /// Modify self, return the previous
    return temp;   /// incremented value.
  }

  BasicIterator(PointerType ptr) : ptr_(ptr) {}
  BasicIterator() = default;
  ~BasicIterator() = default;
protected:
  PointerType ptr_{};
};

END_NAMESPACE(n19);
#endif //ITERATOR_HPP
