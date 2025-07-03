/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <Core/Common.hpp>
#include <Core/Platform.hpp>
#include <Core/ClassTraits.hpp>
#include <Core/TypeTraits.hpp>
BEGIN_NAMESPACE(n19::sys);

template<typename Value>
class Handle {
  N19_MAKE_COMPARABLE_MEMBER(Handle, value_);
  N19_MAKE_DEFAULT_ASSIGNABLE(Handle);
  N19_MAKE_DEFAULT_CONSTRUCTIBLE(Handle);
public:
  virtual auto close()      -> void = 0;
  virtual auto invalidate() -> void = 0;
  virtual auto is_invalid() -> bool = 0;
  virtual ~Handle() = default;
  Handle() = default;

  NODISCARD_ auto value() const -> Value;
  explicit operator bool() const;

  static_assert(!IsReference<Value>);
  using ValueType = Value;
protected:
  Value value_{};
};

template<typename Value>
FORCEINLINE_ auto Handle<Value>::value() const -> Value {
  return value_;
}

template<typename Value>
FORCEINLINE_ Handle<Value>::operator bool() const {
  return !is_invalid();
}

END_NAMESPACE(n19::sys);
