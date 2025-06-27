/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <Core/Common.hpp>
#include <Core/Panic.hpp>
#include <Core/RingBase.hpp>
#include <Core/Maybe.hpp>
#include <concepts>
#include <utility>
BEGIN_NAMESPACE(n19);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// n19::RingQueue is a circular atomic queue with blocking
// functionality. Unlike n19::RingBuffer, it's possible to wait for an object
// to be queued or dequeued to the buffer when it's empty or full.

// Similarly to n19::RingBuffer, this is intended to be used in
// Single-Producer Single-Consumer (SPSC) scenarios ONLY.
// It is unsafe to use the class otherwise.

template<typename T, size_t size_>
class RingQueue : public RingBase<T, size_> {
public:
  using RingBase<T, size_>::head_;
  using RingBase<T, size_>::tail_;
  using RingBase<T, size_>::buff_;
  using RingBase<T, size_>::can_mod_opt_;
  using RingBase<T, size_>::size_mask_;

  using ValueType     = T;
  using ReferenceType = T&;
  using PointerType   = T*;

  template<typename ...Args> auto enqueue(Args&&... args) -> void;
  template<typename ...Args> auto try_enqueue(Args&&... args) -> bool;

  // Similarly to enqueue() and try_enqueue(), we have blocking
  // and non-blocking operations here. current() gets the current
  // element at the tail without dequeueing it.
  auto dequeue()           -> ValueType;
  auto try_dequeue()       -> Maybe<ValueType>;
  auto try_current() const -> Maybe<ValueType>;
  auto current() const     -> ValueType;

  // For peeking operations. i.e. when the consumer wants to peek
  // ahead a certain amount of elements without consuming anything.
  auto can_peek(size_t amnt) -> bool;
  auto try_peek(size_t amnt) -> Maybe<ValueType>;
  auto peek(size_t amnt)     -> ValueType;

  // wake any threads waiting on head_ or tail_, if any.
  auto wake_all() -> void;
  
  ~RingQueue() = default;
  RingQueue()  = default;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, size_t size_>
FORCEINLINE_ auto RingQueue<T, size_>::dequeue() -> ValueType {
  constexpr auto read_order  = std::memory_order::acquire;
  constexpr auto write_order = std::memory_order::release;
  const size_t lhead = head_.load(read_order);
  const size_t ltail = tail_.load(read_order);

  if((lhead & size_mask_) == (ltail & size_mask_)) {
    head_.wait(lhead, read_order);
  }

  const ValueType val = buff_[ tail_.load(read_order) & size_mask_ ];
  tail_.fetch_add(1, write_order);
  tail_.notify_all();
  return val;
}

template<typename T, size_t size_>
FORCEINLINE_ auto RingQueue<T, size_>::try_dequeue() -> Maybe<ValueType> {
  const size_t lhead = head_.load(std::memory_order::acquire) & size_mask_;
  const size_t ltail = tail_.load(std::memory_order::acquire) & size_mask_;
  if(lhead == ltail) {   // the buffer is empty.
    return Nothing;      // we can't read anything.
  }
  const ValueType val = buff_[ ltail ];
  tail_.fetch_add(1, std::memory_order::release);
  tail_.notify_all();
  return val;
}

template<typename T, size_t size_>
FORCEINLINE_ auto RingQueue<T, size_>::can_peek(const size_t amnt) -> bool {
  const size_t lhead = head_.load(std::memory_order::acquire) & size_mask_;
  const size_t ltail = tail_.load(std::memory_order::acquire) & size_mask_;

  ASSERT(amnt < size_); // Amount must be less than buffer size
  const size_t max_distance = lhead >= ltail
    ? lhead - ltail
    : (size_ - ltail) + lhead;
  return amnt < max_distance;
}

template<typename T, size_t size_>
FORCEINLINE_ auto RingQueue<T, size_>::try_peek(const size_t amnt) -> Maybe<ValueType> {
  const size_t lhead = head_.load(std::memory_order::acquire) & size_mask_;
  const size_t ltail = tail_.load(std::memory_order::acquire) & size_mask_;

  const size_t max_distance = lhead >= ltail
    ? lhead - ltail
    : (size_ - ltail) + lhead;
  if(amnt >= max_distance) {
    return Nothing;
  }

  ASSERT(amnt < size_); // Amount must be less than buffer size
  ASSERT(((ltail + amnt) & size_mask_) < size_); 
  return buff_[ (ltail + amnt) & size_mask_ ];
}

template<typename T, size_t size_>
FORCEINLINE_ auto RingQueue<T, size_>::peek(const size_t amnt) -> ValueType {
  const size_t ltail = tail_.load(std::memory_order::acquire) & size_mask_;
  while(!can_peek(amnt)) {
    head_.wait(head_.load(std::memory_order::acquire));
  }

  return buff_[ (ltail + amnt) & size_mask_ ];
}

template<typename T, size_t size_> template<typename ... Args>
FORCEINLINE_ auto RingQueue<T, size_>::enqueue(Args&&... args) -> void {
  static_assert(std::constructible_from<T, Args...>);
  const size_t lhead = head_.load(std::memory_order::acquire);
  const size_t ltail = tail_.load(std::memory_order::acquire);

  if( ((lhead + 1) & size_mask_) == (ltail & size_mask_) ) {
    tail_.wait(ltail, std::memory_order::acquire);
  }

  buff_[ lhead & size_mask_ ] = T{std::forward<Args>(args)...};
  head_.fetch_add(1, std::memory_order::release);
  head_.notify_all();
}

template<typename T, size_t size_> template<typename ... Args>
FORCEINLINE_ auto RingQueue<T, size_>::try_enqueue(Args&&... args) -> bool {
  static_assert(std::constructible_from<T, Args...>);
  const size_t lhead = head_.load(std::memory_order::acquire);
  const size_t ltail = tail_.load(std::memory_order::acquire);

  if(((lhead + 1) & size_mask_) == (ltail & size_mask_)) {
    return false;
  }

  buff_[ lhead & size_mask_ ] = T{std::forward<Args>(args)...};
  head_.fetch_add(1, std::memory_order::release);
  head_.notify_all();
  return true;
}

template<typename T, size_t size_>
FORCEINLINE_ auto RingQueue<T, size_>::wake_all() -> void {
  head_.notify_all();
  tail_.notify_all();
}

template<typename T, size_t size_>
FORCEINLINE_ auto RingQueue<T, size_>::current() const -> ValueType {
  const size_t lhead = head_.load(std::memory_order::acquire);
  const size_t ltail = tail_.load(std::memory_order::acquire);

  if((lhead & size_mask_) == (ltail & size_mask_)) {
    head_.wait(lhead, std::memory_order::acquire);
  }

  return buff_[ ltail & size_mask_ ];
}

template<typename T, size_t size_>
FORCEINLINE_ auto RingQueue<T, size_>::try_current() const -> Maybe<ValueType> {
  const size_t lhead = head_.load(std::memory_order::acquire);
  const size_t ltail = tail_.load(std::memory_order::acquire);

  if((lhead & size_mask_) == (ltail & size_mask_)) {
    return Nothing;
  }

  return buff_[ ltail & size_mask_ ];
}

END_NAMESPACE(n19);