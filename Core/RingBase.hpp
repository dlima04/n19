/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef RINGBASE_HPP
#define RINGBASE_HPP
#include <Core/ClassTraits.hpp>
#include <Core/Platform.hpp>
#include <atomic>
#include <concepts>

namespace n19 {
  template<class T, size_t size_ = 32>
  class RingBase;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Base class for n19::RingBuffer and n19::RingQueue.
// Implements basic shared functionality like head and tail indices,
// checking if the buffer is full/empty, etc.

template<class T, size_t size_>
class n19::RingBase {
public:
  // The size of the buffer should be a power of 2, always.
  // This allows us to eliminate modulo, which not only makes code
  // faster but also simpler to reason about, since 0 % N
  // is technically undefined behaviour and can potentially cause crashes.
  constexpr static bool can_mod_opt_ = (size_ & (size_ - 1)) == 0;
  constexpr static size_t size_mask_ = size_ - 1;

  static_assert( can_mod_opt_, "size must be a power of 2!" );
  static_assert( size_ > 1, "Size must be greater than 1!" );

  // The public methods inherited by n19::RingBuffer
  // and n19::RingQueue. Implements basic functionality shared
  // across these classes.
  auto is_full()  const -> bool;
  auto is_empty() const -> bool;
  auto data()     const -> T*;

  ~RingBase() = default;
  RingBase()  = default;
protected:
  T buff_[ size_ ];               // data buffer.
  std::atomic<size_t> head_{ 0 }; // write index.
  std::atomic<size_t> tail_{ 0 }; // read index.
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T, size_t size_>
N19_FORCEINLINE auto n19::RingBase<T, size_>::is_full() const -> bool {
  constexpr auto order  = std::memory_order::acquire;
  const size_t lhead    = (head_.load(order) + 1) & size_mask_;
  const size_t ltail    = tail_.load(order) & size_mask_;
  return lhead == ltail;
}

template<class T, size_t size_>
N19_FORCEINLINE auto n19::RingBase<T, size_>::is_empty() const -> bool {
  constexpr auto order  = std::memory_order::acquire;
  const size_t lhead    = head_.load(order) & size_mask_;
  const size_t ltail    = tail_.load(order) & size_mask_;
  return lhead == ltail;
}

template<class T, size_t size_>
N19_FORCEINLINE auto n19::RingBase<T, size_>::data() const -> T* {
  return buff_;
}

#endif //RINGBASE_HPP
