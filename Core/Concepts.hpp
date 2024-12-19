/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef CONCEPTS_HPP
#define CONCEPTS_HPP
#include <Core/TypeTraits.hpp>
#include <Core/Forward.hpp>
BEGIN_NAMESPACE(n19);

template<typename T, typename ...Us>
concept AreAll = (IsSame<T, Us> && ...);

template<typename T, typename ...Us>
concept AnyOf = (IsSame<T, Us> || ...);

template<typename T, typename ...Us>
concept CallableWith = requires(T&& t, Us&&... us){
  { t(forward<Us>(us)...) };
};

template<typename T>
concept Character = IsCharacter<T>;

template<typename T>
concept Integer = IsIntegral<T>;

template<typename T>
concept Pointer = IsPointer<T>;

template<typename T>
concept Reference = IsReference<T>;

template<typename T>
concept FloatingPoint = IsFloatingPoint<T>;

template<typename T>
concept IntOrFloat = IsIntegral<T> || IsFloatingPoint<T>;

END_NAMESPACE(n19);
#endif //CONCEPTS_HPP
