/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef DEFER_HPP
#define DEFER_HPP
#include <Core/ClassTraits.hpp>
#include <type_traits>

#define DEFER(obj)          ::n19::Defer _(obj);
#define DEFER_IF(cond, obj) ::n19::Defer _([&](){ if((cond)){obj();} })

namespace n19 {
  template<typename T> requires std::is_invocable_v<T>
  class Defer {
    N19_MAKE_NONMOVABLE(Defer);
    N19_MAKE_NONCOPYABLE(Defer);
  public:
    explicit Defer(T obj) : obj_(obj) {}
    ~Defer() { obj_(); }
  private:
    T obj_;
  };
}

#endif //DEFER_HPP
