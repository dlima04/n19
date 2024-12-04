/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef NATIVE_HANDLE_HPP
#define NATIVE_HANDLE_HPP
#include <Core/Concepts.hpp>
#include <Core/ClassTraits.hpp>

namespace n19::sys {
  template<typename Value>
  class Handle;
}

template<typename Value>
class n19::sys::Handle {
N19_MAKE_COMPARABLE_MEMBER(Handle, value_);
public:
  virtual auto close()      -> void = 0;
  virtual auto invalidate() -> void = 0;
  virtual auto is_invalid() -> bool = 0;
  explicit operator bool() const;
  virtual ~Handle() = default;
protected:
  Value value_{};
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename Value>
n19::sys::Handle<Value>::operator bool() const {
  return !is_invalid();
}

#endif //NATIVE_HANDLE_HPP
