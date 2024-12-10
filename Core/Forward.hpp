/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef FORWARD_HPP
#define FORWARD_HPP
#include <Core/TypeTraits.hpp>
BEGIN_NAMESPACE(n19);

template<typename T>
constexpr inline auto forward( RemoveReference<T>& __t ) -> T&& {
  return static_cast< T&& >(__t);
}

template<typename T>
constexpr inline auto forward( RemoveReference<T>&& __t ) -> T&& {
  static_assert(!IsLvalueReference<T>);
  return static_cast< T&& >(__t);
}

END_NAMESPACE(n19);
#endif //FORWARD_HPP
