/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <Core/Common.hpp>
#include <Core/Platform.hpp>
#include <Core/Iterator.hpp>
#include <atomic>
#include <type_traits>
BEGIN_NAMESPACE(n19);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Base class for n19::RingBuffer and n19::RingQueue.
// Implements basic shared functionality like head and tail indices,
// checking if the buffer is full/empty, etc.
//
// NOTE:
// The size of the buffer should be a power of 2, always.
// This allows us to eliminate modulo, which not only makes code
// faster but also simpler to reason about, since 0 % N
// is technically undefined behaviour and can potentially cause crashes.

template<typename T, size_t size_>
class RingBase {
public:
  constexpr static bool can_mod_opt_ = (size_ & (size_ - 1)) == 0;
  constexpr static size_t size_mask_ = size_ - 1;

  static_assert( std::is_trivially_destructible_v<T> );
  static_assert( can_mod_opt_, "size must be a power of 2!" );
  static_assert( size_ > 1, "Size must be greater than 1!" );

  auto is_full()  const -> bool;
  auto is_empty() const -> bool;
  auto data()     const -> T*;

  /// Define some iterator methods. These aren't really all that
  /// important for a ring buffer but migh still come in handy.
  using IteratorType = BasicIterator<T>;
  NODISCARD_ auto end()   -> IteratorType;
  NODISCARD_ auto begin() -> IteratorType;

  ~RingBase() = default;
  RingBase()  = default;
protected:
  alignas(N19_CACHE_LINE_SIZE_GUESS) T buff_[ size_ ];
  alignas(N19_CACHE_LINE_SIZE_GUESS) std::atomic<size_t> head_{ 0 };
  alignas(N19_CACHE_LINE_SIZE_GUESS) std::atomic<size_t> tail_{ 0 };
};


template<typename T, size_t size_>
FORCEINLINE_ auto RingBase<T, size_>::is_full() const -> bool {
  constexpr auto order  = std::memory_order::acquire;
  const size_t lhead    = (head_.load(order) + 1) & size_mask_;
  const size_t ltail    = tail_.load(order) & size_mask_;
  return lhead == ltail;
}

template<typename T, size_t size_>
FORCEINLINE_ auto RingBase<T, size_>::is_empty() const -> bool {
  constexpr auto order  = std::memory_order::acquire;
  const size_t lhead    = head_.load(order) & size_mask_;
  const size_t ltail    = tail_.load(order) & size_mask_;
  return lhead == ltail;
}

template<typename T, size_t size_>
FORCEINLINE_ auto RingBase<T, size_>::data() const -> T* {
  return buff_;
}

template<typename T, size_t size_>
FORCEINLINE_ auto RingBase<T, size_>::begin() -> IteratorType {
  return buff_;
}

template<typename T, size_t size_>
FORCEINLINE_ auto RingBase<T, size_>::end() -> IteratorType {
  return buff_ + (sizeof(buff_) / sizeof(T));
}

END_NAMESPACE(n19);