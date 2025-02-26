/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef N19_CORE_REF_HPP
#define N19_CORE_REF_HPP
#include <Core/ClassTraits.hpp>
#include <Core/Platform.hpp>
#include <utility>
BEGIN_NAMESPACE(n19);

template<typename T>
class Ref {
N19_MAKE_DEFAULT_CONSTRUCTIBLE(Ref);
N19_MAKE_DEFAULT_ASSIGNABLE(Ref);
public:
  using ReferenceType = T&;
  using ValueType     = T;
  using PointerType   = T*;

  NODISCARD_ FORCEINLINE_ auto&& value(this auto&& self) {
    return *std::forward<decltype(self)>(self).ptr_;
  }

  NODISCARD_ FORCEINLINE_ auto&& operator*(this auto&& self) {
    return std::forward<decltype(self)>(self).value();
  }

  NODISCARD_ FORCEINLINE_ auto* operator->(this auto&& self) {
    return std::forward<decltype(self)>(self).ptr_;
  }

  Ref() = delete;
  Ref(ReferenceType ref) : ptr_(&ref) {}
 ~Ref() = default;
private:
  PointerType ptr_{};
};

END_NAMESPACE(n19);
#endif //N19_CORE_REF_HPP
