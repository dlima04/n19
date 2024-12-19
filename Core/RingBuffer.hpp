/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef RINGBUFFER_HPP
#define RINGBUFFER_HPP
#include <Core/RingBase.hpp>
#include <Core/Result.hpp>
#include <Core/Maybe.hpp>
#include <Core/Forward.hpp>
BEGIN_NAMESPACE(n19);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// n19::RingBuffer is a circular buffer with atomic read/writes.
// The difference between this class and n19::RingQueue is that this class
// is wait-free. When the buffer is full and a write attempts to be made,
// the write will fail. depending on the method called. This is unlike
// RingQueue which includes blocking writes.

// Lastly, note that this class should be used in
// Single-Producer Single-Consumer (SPSC) scenarios ONLY.
// It is unsafe to use multiple producers or consumers because
// code here heavily relies on release-acquire memory ordering semantics,
// which may be sequentially inconsistent otherwise.

template<typename T, size_t size_>
class RingBuffer : public RingBase<T, size_>{
public:
  using ValueType     = T;
  using ReferenceType = T&;
  using PointerType   = T*;

  using RingBase<T, size_>::head_;
  using RingBase<T, size_>::tail_;
  using RingBase<T, size_>::buff_;
  using RingBase<T, size_>::can_mod_opt_;
  using RingBase<T, size_>::size_mask_;

  // write() will attempt to construct
  // an object of type T directly at the head indice using the
  // parameter pack Args.
  template<typename ...Args> auto write(Args&&... args) -> bool;
  template<typename ...Args> auto overwrite(Args&&... args) -> void;

  // For reading values from the ringbuffer.
  // note that current() and try_current retrieve the value at
  // tail_ without incrementing it.
  auto read() -> Maybe<ValueType>;
  auto try_current() const -> Maybe<ValueType>;
  auto current() const -> ValueType;

  ~RingBuffer() = default;
  RingBuffer()  = default;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, size_t size_> template<typename ... Args>
N19_FORCEINLINE auto RingBuffer<T, size_>::write(Args&&... args) -> bool {
  static_assert(std::constructible_from<T, Args...>);
  if(this->is_full()) {
    return false;
  }

  buff_[ head_.load() & size_mask_ ] = T{forward<Args>(args)...};
  head_.fetch_add(1, std::memory_order::release);
  return true;
}

template<typename T, size_t size_> template<typename ... Args>
N19_FORCEINLINE auto RingBuffer<T, size_>::overwrite(Args&&... args) -> void {
  static_assert(std::constructible_from<T, Args...>);
  if(this->is_full()) {
    tail_.fetch_add(1, std::memory_order::release);
  }

  buff_[ head_.load() & size_mask_ ] = T{forward<Args>(args)...};
  head_.fetch_add(1, std::memory_order::release);
}

template<typename T, size_t size_>
N19_FORCEINLINE auto RingBuffer<T, size_>::read() -> Maybe<ValueType> {
  const size_t lhead = head_.load(std::memory_order::acquire) & size_mask_;
  const size_t ltail = tail_.load(std::memory_order::acquire) & size_mask_;
  if(lhead == ltail) {    // buffer is empty.
    return Nothing;       // we can't read anything.
  }

  const ValueType val = buff_[ ltail ];
  tail_.fetch_add(1, std::memory_order::release);
  return val;
}

template<typename T, size_t size_>
N19_FORCEINLINE auto RingBuffer<T, size_>::try_current() const -> Maybe<ValueType> {
  const size_t lhead = head_.load(std::memory_order::acquire);
  const size_t ltail = tail_.load(std::memory_order::acquire);

  if((lhead & size_mask_) == (ltail & size_mask_)) {
    return Nothing;
  }

  return buff_[ ltail & size_mask_ ];
}

template<typename T, size_t size_>
N19_FORCEINLINE auto RingBuffer<T, size_>::current() const -> ValueType {
  const size_t lhead = head_.load(std::memory_order::acquire);
  const size_t ltail = tail_.load(std::memory_order::acquire);

  if((lhead & size_mask_) == (ltail & size_mask_)) {
    head_.wait(lhead, std::memory_order::acquire);
  }

  return buff_[ ltail & size_mask_ ];
}

END_NAMESPACE(n19);
#endif //RINGBUFFER_HPP
