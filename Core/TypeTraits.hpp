/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef TYPETRAITS_HPP
#define TYPETRAITS_HPP
BEGIN_NAMESPACE(n19);

template<typename...>
using VoidType = void;

template<typename T, typename = void>
struct __AddReference {
  using LvalueType = T;
  using TvalueType = T;
};

template<typename T>
struct __AddReference<T, VoidType<T&>> {
  using LvalueType = T&;
  using RvalueType = T&&;
};

template<typename T>
using AddLvalueReference = typename __AddReference<T>::LvalueType;

template<typename T>
using AddRvalueReference = typename __AddReference<T>::RvalueType;

template<typename T>
typename __AddReference<T>::RvalueType declval();

template<typename T>
struct __RemoveReference {
  using Type = T;
};

template<class T>
struct __RemoveReference<T&> {
  using Type = T;
};

template<class T>
struct __RemoveReference<T&&> {
  using Type = T;
};

template<class T>
struct __RemoveConst {
  using Type = T;
};

template<class T>
struct __RemoveConst<T const> {
  using Type = T;
};

template<class T>
using AddConst = T const;

template<class T>
using RemoveConst = typename __RemoveConst<T>::Type;

template<typename T>
using RemoveReference = typename __RemoveReference<T>::Type;

template<class T>
struct __RemoveVolatile {
  using Type = T;
};

template<class T>
struct __RemoveVolatile<T volatile> {
  using Type = T;
};

template<typename T>
using RemoveVolatile = typename __RemoveVolatile<T>::Type;

template<typename T>
using RemoveCV = RemoveVolatile<RemoveConst<T>>;

template<typename T>
using RemoveCVRef = RemoveReference<RemoveCV<T>>;

template<typename T, typename... Args>
inline constexpr bool IsConstructible = requires { ::new T(declval<Args>()...); };

template<class T,  class ...Us>
inline constexpr bool IsInvocableWith = requires { T(declval<Us>()...); };

template<typename T, typename U>
inline constexpr bool IsSame = false;

template<typename T>
inline constexpr bool IsSame<T, T> = true;

template<class T>
inline constexpr bool IsLvalueReference = false;

template<class T>
inline constexpr bool IsLvalueReference<T&> = true;

template<class T>
inline constexpr bool IsRvalueReference = false;

template<class T>
inline constexpr bool IsRvalueReference<T&&> = true;

template<class T>
inline constexpr bool IsReference = false;

template<class T>
inline constexpr bool IsReference<T&> = true;

template<class T>
inline constexpr bool IsReference<T&&> = true;

END_NAMESPACE(n19);
#endif //TYPETRAITS_HPP
