/*
* Copyright (c) 2025 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
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

  N19_FORCEINLINE auto operator*(this auto&& self) -> auto&& {
    return *std::forward<decltype(self)>(self).ptr_;
  }

  N19_FORCEINLINE auto operator->(this auto&& self) -> auto* {
    return std::forward<decltype(self)>(self).ptr_;
  }

  N19_FORCEINLINE auto operator++() -> BasicIterator& {
    ++ptr_;        /// Simply increment the internal pointer
    return *this;  /// and return self.
  }

  N19_FORCEINLINE auto operator++(int) -> BasicIterator {
    BasicIterator temp = *this;
    ++(*this);     /// Modify self, return the previous
    return temp;   /// incremented value.
  }

  N19_FORCEINLINE auto operator--() -> BasicIterator& {
    --ptr_;        /// Simply decrement the internal pointer
    return *this;  /// and return self.
  }

  N19_FORCEINLINE auto operator--(int) -> BasicIterator {
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
