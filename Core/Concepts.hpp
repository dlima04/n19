/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef CONCEPTS_HPP
#define CONCEPTS_HPP
#include <Core/TypeTraits.hpp>
#include <utility>
BEGIN_NAMESPACE(n19);

template<typename T, typename ...Us>
concept AreAll = (IsSame<T, Us> && ...);

template<typename T, typename ...Us>
concept AnyOf = (IsSame<T, Us> || ...);

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

template<typename T>
concept Concrete = !IsReference<T>;

template<typename T, typename U>
concept Is = IsSame<DecayT<T>, U>;

template<typename T, typename ...Us>
concept CallableWith = requires(T&& t, Us&&... us){
  { t(std::forward<Us>(us)...) };
};

END_NAMESPACE(n19);
#endif //CONCEPTS_HPP
