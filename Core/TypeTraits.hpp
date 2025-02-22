/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef TYPETRAITS_HPP
#define TYPETRAITS_HPP
BEGIN_NAMESPACE(n19);

template<typename...>
using VoidType = void;

template<typename T, typename = void>
struct AddReference_ {
  using LvalueType = T;
  using TvalueType = T;
};

template<typename T>
struct AddReference_<T, VoidType<T&>> {
  using LvalueType = T&;
  using RvalueType = T&&;
};

template<typename T>
struct MakeUnsigned_ {
  using Type = void;
};

template<>
struct MakeUnsigned_<signed char> {
  using Type = unsigned char;
};

template<>
struct MakeUnsigned_<short> {
  using Type = unsigned short;
};

template<>
struct MakeUnsigned_<int> {
  using Type = unsigned int;
};

template<>
struct MakeUnsigned_<long> {
  using Type = unsigned long;
};

template<>
struct MakeUnsigned_<long long> {
  using Type = unsigned long long;
};

template<>
struct MakeUnsigned_<unsigned char> {
  using Type = unsigned char;
};

template<>
struct MakeUnsigned_<unsigned short> {
  using Type = unsigned short;
};

template<>
struct MakeUnsigned_<unsigned int> {
  using Type = unsigned int;
};

template<>
struct MakeUnsigned_<unsigned long> {
  using Type = unsigned long;
};

template<>
struct MakeUnsigned_<unsigned long long> {
  using Type = unsigned long long;
};

template<>
struct MakeUnsigned_<char> {
  using Type = unsigned char;
};

template<>
struct MakeUnsigned_<char8_t> {
  using Type = char8_t;
};

template<>
struct MakeUnsigned_<char16_t> {
  using Type = char16_t;
};

template<>
struct MakeUnsigned_<char32_t> {
  using Type = char32_t;
};

template<>
struct MakeUnsigned_<bool> {
  using Type = bool;
};

template<typename T>
using AddConst = T const;

template<typename T>
using AddPtr = T*;

template<typename T>
using MakeUnsigned = typename MakeUnsigned_<T>::Type;

template<typename T>
using AddLvalueReference = typename AddReference_<T>::LvalueType;

template<typename T>
using AddRvalueReference = typename AddReference_<T>::RvalueType;

template<typename T>
typename AddReference_<T>::RvalueType declval();

template<typename T>
struct RemoveReference_ {
  using Type = T;
};

template<typename T>
struct RemoveReference_<T&> {
  using Type = T;
};

template<typename T>
struct RemoveReference_<T&&> {
  using Type = T;
};

template<typename T>
struct RemovePointer_ {
  using Type = T;
};

template<typename T>
struct RemovePointer_<T*> {
  using Type = T;
};

template<typename T>
struct RemoveConst_ {
  using Type = T;
};

template<typename T>
struct RemoveConst_<T const> {
  using Type = T;
};

template<typename T>
struct FunctorTraits {
  static_assert("Could not deduce FunctorTraits_ of T.");
};

template<typename T, typename... Args>
struct FunctorTraits<T(Args...)> {
  using ReturnType = T;
};

template<typename T, typename C, typename... Args>
struct FunctorTraits<T(C::*)(Args...)> {
  using ReturnType = T;
};

template<typename T>
using RemoveConst = typename RemoveConst_<T>::Type;

template<typename T>
using RemoveReference = typename RemoveReference_<T>::Type;

template<typename T>
using RemovePointer = typename RemovePointer_<T>::Type;

template<typename T>
struct RemoveVolatile_ {
  using Type = T;
};

template<typename T>
struct RemoveVolatile_<T volatile> {
  using Type = T;
};

template<typename T>
using RemoveVolatile = typename RemoveVolatile_<T>::Type;

template<typename T>
using RemoveCV = RemoveVolatile<RemoveConst<T>>;

template<typename T>
using DecayT = RemoveReference<RemoveCV<T>>;

template<typename T>
inline constexpr bool IsIntegral_ = false; //base

template<> inline constexpr bool IsIntegral_<bool>                = true;
template<> inline constexpr bool IsIntegral_<unsigned char>       = true;
template<> inline constexpr bool IsIntegral_<unsigned short>      = true;
template<> inline constexpr bool IsIntegral_<unsigned int>        = true;
template<> inline constexpr bool IsIntegral_<unsigned long>       = true;
template<> inline constexpr bool IsIntegral_<unsigned long long>  = true;
template<> inline constexpr bool IsIntegral_<char8_t>             = true;
template<> inline constexpr bool IsIntegral_<char16_t>            = true;
template<> inline constexpr bool IsIntegral_<char32_t>            = true;

template<typename T>
inline constexpr bool IsIntegral = IsIntegral_<DecayT<MakeUnsigned<T>>>;

template<typename T> inline constexpr bool IsLvalueReference      = false;
template<typename T> inline constexpr bool IsLvalueReference<T&>  = true;
template<typename T> inline constexpr bool IsRvalueReference      = false;
template<typename T> inline constexpr bool IsRvalueReference<T&&> = true;
template<typename T> inline constexpr bool IsReference            = false;
template<typename T> inline constexpr bool IsReference<T&>        = true;
template<typename T> inline constexpr bool IsReference<T&&>       = true;

template<typename T>
inline constexpr bool IsFloatingPoint_ = false; //base

template<> inline constexpr bool IsFloatingPoint_<double>         = true;
template<> inline constexpr bool IsFloatingPoint_<long double>    = true;
template<> inline constexpr bool IsFloatingPoint_<float>          = true;

template<typename T>
inline constexpr bool IsFloatingPoint = IsFloatingPoint_<DecayT<T>>;

template<typename T>
inline constexpr bool IsCharacter_ = false; //base

template<> inline constexpr bool IsCharacter_<char8_t>            = true;
template<> inline constexpr bool IsCharacter_<char16_t>           = true;
template<> inline constexpr bool IsCharacter_<char32_t>           = true;
template<> inline constexpr bool IsCharacter_<char>               = true;
template<> inline constexpr bool IsCharacter_<wchar_t>            = true;

template<typename T>
inline constexpr bool IsCharacter = IsCharacter_<DecayT<T>>;

template<typename T> inline constexpr bool IsPointer_             = false;
template<typename T> inline constexpr bool IsPointer_<T*>         = true;

template<typename T>
inline constexpr bool IsPointer = IsPointer_<RemoveCV<T>>;

template<typename T, typename... Args>
inline constexpr bool IsConstructible = requires { ::new T(declval<Args>()...); };

template<typename T, typename ...Us>
inline constexpr bool IsInvocableWith = requires { T(declval<Us>()...); };

template<typename T>
inline constexpr bool IsVoid = false;

template<>
inline constexpr bool IsVoid<void> = true;

template<typename T, typename U>
inline constexpr bool IsSame = false;

template<typename T>
inline constexpr bool IsSame<T, T> = true;

END_NAMESPACE(n19);
#endif //TYPETRAITS_HPP
