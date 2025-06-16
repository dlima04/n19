/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <Core/Platform.hpp>
#include <Core/Panic.hpp>
#include <Core/Nothing.hpp>
#include <Core/Concepts.hpp>
#include <utility>
#include <new>
#include <concepts>
#include <functional>
BEGIN_NAMESPACE(n19)

template<Concrete T>
class Maybe_ {
public:
  using ValueType     = T;
  using ReferenceType = T&;
  using PointerType   = T*;
    
  FORCEINLINE_ auto value() -> ReferenceType {
    ASSERT(has_value_, "Maybe_ has no contained type!");
    return *std::launder<T>(reinterpret_cast<T*>(&value_));
  }

  FORCEINLINE_ auto value() const -> const T& {
    ASSERT(has_value_, "Maybe_ has no contained type!");
    return *std::launder<const T>(reinterpret_cast<const T*>(&value_));
  }

  FORCEINLINE_ auto release_value() -> ValueType {
    ASSERT(has_value_, "Maybe_ has no contained type!");
    T released = std::move(value());
    value().~T();           /// We'll have to manually call the destructor.
    has_value_ = false;     ///
    return released;
  }

  auto operator->(this auto&& self) -> decltype(auto) {
    ASSERT( self.has_value_ == true, "Bad Maybe_ access!" );
    return &self.value();
  }

  auto operator*(this auto&& self) -> decltype(auto) {
    ASSERT( self.has_value_ == true, "Bad Maybe_ access!" );
    return self.value();
  }

  FORCEINLINE_ auto value_or(T&& other) const -> ValueType {
    if(has_value_) return value();
    return other;
  }

  FORCEINLINE_ auto operator=(Maybe_&& other) noexcept -> Maybe_& {
    if(&other == this) return *this;
    clear();                /// clear the current value
    if(other.has_value_) {  /// Check the other Maybe_'s state...
      has_value_ = true;
      ::new (&value_) T(other.release_value());
    }

    return *this;
  }

  FORCEINLINE_ auto operator=(const Maybe_& other) -> Maybe_& {
    if(&other == this) return *this;
    clear();                /// clear the current value
    if(other.has_value_) {  /// Check the other Maybe_'s state...
      has_value_ = true;
      ::new(&value_) T(other.value());
    }
    
    return *this;
  }

  template<typename ...Args>
  FORCEINLINE_ auto emplace(Args&&... args) -> void {
    clear();
    ::new (&value_) T(std::forward<Args>(args)...);
    has_value_ = true;
  }

  FORCEINLINE_ auto clear() -> void {
    if(has_value_) value().~T();
    has_value_ = false;
  }

  template<typename ...Args>
  FORCEINLINE_ static auto create(Args&&... args) -> Maybe_ {
    return Maybe_{ std::forward<Args>(args)... };
  }

  template<typename ...Args> requires std::constructible_from<T, Args...>
  FORCEINLINE_ Maybe_(Args&&... args) {
    ::new (&value_) T(std::forward<Args>(args)...);
    has_value_ = true;
  }

  FORCEINLINE_ Maybe_(const Maybe_& other) {
    if(!other.has_value_) return;
    ::new (&value_) T(other.value());
    has_value_ = true;
  }

  FORCEINLINE_ Maybe_(Maybe_&& other) noexcept {
    if(!other.has_value_) return;
    ::new (&value_) T(other.release_value());
    has_value_ = true;
  }

  auto has_value() const -> bool { return has_value_; }
  explicit operator bool() const { return has_value_; }
  
 ~Maybe_() { clear(); }
  Maybe_(const Nothing_&){}
  Maybe_() = default;
private:
  bool has_value_{false};
  alignas(T) uint8_t value_[ sizeof(T) ]{};
};

template<typename T>
struct MaybeDispatch_ {
  using Type = Maybe_<T>;
};

template<>
struct MaybeDispatch_<void> {
  using Type = Maybe_<Nothing_>;
};

template<typename T>
struct MaybeDispatch_<T&> {
  using Type = Maybe_<std::reference_wrapper<T>>;
};

template<typename T>
using Maybe = typename MaybeDispatch_<T>::Type;

END_NAMESPACE(n19);