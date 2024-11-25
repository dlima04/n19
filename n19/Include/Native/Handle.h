/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef NATIVE_HANDLE_H
#define NATIVE_HANDLE_H
#include <Core/Concepts.h>

namespace n19::native {
  template<typename Value>
  class Handle;
}

template<typename Value>
class n19::native::Handle {
public:
  auto operator==(const Handle& other) const -> bool;
  auto operator!=(const Handle& other) const -> bool;
  explicit operator bool() const;

  virtual auto close()      -> void = 0;
  virtual auto invalidate() -> void = 0;
  virtual auto is_invalid() -> bool = 0;
  virtual ~Handle() = default;
protected:
  Value value_{};
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename Value>
auto n19::native::Handle<Value>::operator!=(
  const Handle& other ) const -> bool
{
  return other.value_ != this->value_;
}

template<typename Value>
auto n19::native::Handle<Value>::operator==(
  const Handle& other ) const -> bool
{
  return other.value_ == this->value_;
}

template<typename Value>
n19::native::Handle<Value>::operator bool() const {
  return !is_invalid();
}

#endif //HANDLE_H
