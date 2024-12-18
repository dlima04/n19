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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Type traits for adding qualifiers

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
struct __MakeUnsigned {
  using Type = void;
};

template<>
struct __MakeUnsigned<signed char> {
  using Type = unsigned char;
};

template<>
struct __MakeUnsigned<short> {
  using Type = unsigned short;
};

template<>
struct __MakeUnsigned<int> {
  using Type = unsigned int;
};

template<>
struct __MakeUnsigned<long> {
  using Type = unsigned long;
};

template<>
struct __MakeUnsigned<long long> {
  using Type = unsigned long long;
};

template<>
struct __MakeUnsigned<unsigned char> {
  using Type = unsigned char;
};

template<>
struct __MakeUnsigned<unsigned short> {
  using Type = unsigned short;
};

template<>
struct __MakeUnsigned<unsigned int> {
  using Type = unsigned int;
};

template<>
struct __MakeUnsigned<unsigned long> {
  using Type = unsigned long;
};

template<>
struct __MakeUnsigned<unsigned long long> {
  using Type = unsigned long long;
};

template<>
struct __MakeUnsigned<char> {
  using Type = unsigned char;
};

template<>
struct __MakeUnsigned<char8_t> {
  using Type = char8_t;
};

template<>
struct __MakeUnsigned<char16_t> {
  using Type = char16_t;
};

template<>
struct __MakeUnsigned<char32_t> {
  using Type = char32_t;
};

template<>
struct __MakeUnsigned<bool> {
  using Type = bool;
};

template<class T>
using AddConst = T const;

template<class T>
using AddPtr = T*;

template<typename T>
using MakeUnsigned = typename __MakeUnsigned<T>::Type;

template<typename T>
using AddLvalueReference = typename __AddReference<T>::LvalueType;

template<typename T>
using AddRvalueReference = typename __AddReference<T>::RvalueType;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// declval: "simulate" an ephemeral construction of type T

template<typename T>
typename __AddReference<T>::RvalueType declval();

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Type traits for removing qualifiers

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

template<typename T>
struct __RemovePointer {
  using Type = T;
};

template<class T>
struct __RemovePointer<T*> {
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
using RemoveConst = typename __RemoveConst<T>::Type;

template<typename T>
using RemoveReference = typename __RemoveReference<T>::Type;

template<class T>
using RemovePointer = typename __RemovePointer<T>::Type;

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
using DecayT = RemoveReference<RemoveCV<T>>;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IsIntegral

template<typename T>
inline constexpr bool __IsIntegral = false; //base

template<> inline constexpr bool __IsIntegral<bool>               = true;
template<> inline constexpr bool __IsIntegral<unsigned char>      = true;
template<> inline constexpr bool __IsIntegral<unsigned short>     = true;
template<> inline constexpr bool __IsIntegral<unsigned int>       = true;
template<> inline constexpr bool __IsIntegral<unsigned long>      = true;
template<> inline constexpr bool __IsIntegral<unsigned long long> = true;
template<> inline constexpr bool __IsIntegral<char8_t>            = true;
template<> inline constexpr bool __IsIntegral<char16_t>           = true;
template<> inline constexpr bool __IsIntegral<char32_t>           = true;

template<typename T>
inline constexpr bool IsIntegral = __IsIntegral<DecayT<MakeUnsigned<T>>>;

template<class T> inline constexpr bool IsLvalueReference         = false;
template<class T> inline constexpr bool IsLvalueReference<T&>     = true;
template<class T> inline constexpr bool IsRvalueReference         = false;
template<class T> inline constexpr bool IsRvalueReference<T&&>    = true;
template<class T> inline constexpr bool IsReference               = false;
template<class T> inline constexpr bool IsReference<T&>           = true;
template<class T> inline constexpr bool IsReference<T&&>          = true;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IsFloatingPoint

template<typename T>
inline constexpr bool __IsFloatingPoint = false; //base

template<> inline constexpr bool __IsFloatingPoint<double>        = true;
template<> inline constexpr bool __IsFloatingPoint<long double>   = true;
template<> inline constexpr bool __IsFloatingPoint<float>         = true;

template<typename T>
inline constexpr bool IsFloatingPoint = __IsFloatingPoint<DecayT<T>>;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IsCharacter

template<typename T>
inline constexpr bool __IsCharacter = false; //base

template<> inline constexpr bool __IsCharacter<char8_t>           = true;
template<> inline constexpr bool __IsCharacter<char16_t>          = true;
template<> inline constexpr bool __IsCharacter<char32_t>          = true;
template<> inline constexpr bool __IsCharacter<char>              = true;
template<> inline constexpr bool __IsCharacter<wchar_t>           = true;

template<typename T>
inline constexpr bool IsCharacter = __IsCharacter<DecayT<T>>;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IsPointer

template<class T> inline constexpr bool __IsPointer               = false;
template<class T> inline constexpr bool __IsPointer<T*>           = true;

template<typename T>
inline constexpr bool IsPointer = __IsPointer<RemoveCV<T>>;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IsConstructible, IsInvocableWith

template<typename T, typename... Args>
inline constexpr bool IsConstructible = requires { ::new T(declval<Args>()...); };

template<class T,  class ...Us>
inline constexpr bool IsInvocableWith = requires { T(declval<Us>()...); };

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IsSame, IsVoid

template<class T>
inline constexpr bool IsVoid = false;

template<>
inline constexpr bool IsVoid<void> = true;

template<typename T, typename U>
inline constexpr bool IsSame = false;

template<typename T>
inline constexpr bool IsSame<T, T> = true;

END_NAMESPACE(n19);
#endif //TYPETRAITS_HPP
