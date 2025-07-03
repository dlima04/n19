/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <Core/Common.hpp>
#include <Core/Platform.hpp>
#include <Core/ClassTraits.hpp>
#include <utility>
#include <iterator>
#include <cstdint>
BEGIN_NAMESPACE(n19);

template<typename T>
class BasicIterator {
  N19_MAKE_DEFAULT_CONSTRUCTIBLE(BasicIterator);
  N19_MAKE_SPACESHIP(BasicIterator);
public:
  using iterator_category = std::bidirectional_iterator_tag;
  using value_type        = T;
  using difference_type   = std::ptrdiff_t;
  using pointer           = T*;
  using reference         = T&;

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

  BasicIterator(BasicIterator::pointer ptr) : ptr_(ptr) {}
  BasicIterator() = default;
  ~BasicIterator() = default;
protected:
  BasicIterator::pointer ptr_{};
};

END_NAMESPACE(n19);
