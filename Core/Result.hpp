/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <Core/Common.hpp>
#include <Core/Platform.hpp>
#include <Core/Panic.hpp>
#include <Core/ClassTraits.hpp>
#include <Core/TypeTraits.hpp>
#include <Core/Nothing.hpp>
#include <System/Error.hpp>
#include <Core/Fmt.hpp>
#include <string>
#include <utility>
#include <functional>
#include <variant>
#include <concepts>
BEGIN_NAMESPACE(n19);

struct ErrC_ final {
  N19_MAKE_COMPARABLE_MEMBER(ErrC_, value);
  enum Value : uint16_t {
    None       = 0,
    InvalidArg = 1,
    FileIO     = 2,
    Internal   = 3,
    NotFound   = 4,
    BadToken   = 5,
    Native     = 6,
    Conversion = 7,
    Overflow   = 8,
    NotImplimented = 9,
    BadExpr    = 10,
    BadEnt     = 11,
  };

  Value value = None;
  constexpr ErrC_(const Value v) : value(v) {}
  constexpr ErrC_() = default;
};

struct ErrorType_ {
  std::string msg;
  ErrC_ code = ErrC_::None;

  static auto from_native() -> ErrorType_;
  static auto from_error_code(sys::ErrorCode) -> ErrorType_;

  constexpr ErrorType_(ErrC_ c) : code(c) {}
  constexpr ErrorType_() = default;
  constexpr ErrorType_(ErrC_ c, const std::string& m)
  : msg(m) , code(c) {}
};

template<typename T, typename E = ErrorType_>
class Result_ {
  N19_MAKE_DEFAULT_CONSTRUCTIBLE(Result_);
  N19_MAKE_DEFAULT_ASSIGNABLE(Result_);
public:
  using ValueType   = T;
  using PointerType = T*;
  using Variant_    = std::variant<T, E> ;

  template<typename ...Args>
  FORCEINLINE_ static auto create(Args&&... args) -> Result_ {
    return T{std::forward<Args>(args)...};
  }

  NODISCARD_ FORCEINLINE_
  auto value() const -> const T& {
    ASSERT(has_value() == true, "Result contains an error!");
    return std::get<T>( value_ );
  }

  NODISCARD_ FORCEINLINE_
  auto value() -> T& {
    ASSERT(has_value() == true, "Result contains an error!");
    return std::get<T>( value_ );
  }

  NODISCARD_ FORCEINLINE_
  auto error() const -> const E& {
    ASSERT(has_value() == false, "Result contains a value!");
    return std::get<E>( value_ );
  }

  NODISCARD_ FORCEINLINE_
  auto error() -> E& {
    ASSERT(has_value() == false, "Result contains a value!");
    return std::get<E>( value_ );
  }

  FORCEINLINE_ auto release_value() -> T {
    ASSERT(has_value() == true, "Result contains an error!");
    return T(std::move(std::get<T>( value_ )));
  }

  FORCEINLINE_ auto release_error() -> E {
    ASSERT(has_value() == false, "Result contains a value!");
    return E(std::move(std::get<E>( value_ )));
  }

  auto operator->(this auto&& self) -> decltype(auto) {
    ASSERT(self.has_value() == true, "Result contains an error!");
    return &(std::forward<decltype(self)>(self).value());
  }

  auto operator*(this auto&& self) -> decltype(auto) {
    ASSERT(self.has_value() == true, "Result contains an error!");
    return std::forward<decltype(self)>(self).value();
  }

  template<typename ...Args>
  FORCEINLINE_ auto value_or(this auto&& self, Args&&... args) -> T {
    if(self.has_value()) return self.value();
    return T{std::forward<Args>(args)...};
  }

  template<typename ...Args>
  FORCEINLINE_ auto error_or(this auto&& self, Args&&... args) -> E {
    if(!self.has_value()) return self.error();
    return E{std::forward<Args>(args)...};
  }

  template<typename C>
  auto call_if_error(this auto&& self, C&& cb) -> decltype(self) {
    if(!self.has_value()) cb( std::forward<decltype(self)>(self) );
    return self;
  }

  template<typename C>
  auto call_if_value(this auto&& self, C&& cb) -> decltype(self) {
    if(self.has_value()) cb( std::forward<decltype(self)>(self) );
    return self;
  }

  template<typename ...Args> requires std::constructible_from<T, Args...>
  FORCEINLINE_ Result_(Args&&... args)
  : value_(T{std::forward<Args>(args)...}){}

  NODISCARD_ auto has_value() const -> bool {
    return std::holds_alternative<T>( value_ );
  }

  NODISCARD_ explicit operator bool() const {
    return std::holds_alternative<T>( value_ );
  }

  Result_(T&& value) : value_(std::forward<T>(value)) {}
  Result_(E&& error) : value_(std::forward<E>(error)) {}
  Result_(/*.....*/) : value_(E{}) {}
protected:
  Variant_ value_;
};

template<typename T>
struct Result_Dispatch_ {
  using Type = Result_<T>;
};

template<>
struct Result_Dispatch_<void> {
  using Type = Result_<Nothing_>;
};

template<typename T>
struct Result_Dispatch_<T&> {
  using Type = Result_<std::reference_wrapper<T>>;
};

template<typename T>
using Result = typename Result_Dispatch_<T>::Type;
using ErrC   = ErrC_;
using Error  = ErrorType_;

inline auto ErrorType_::from_native() -> ErrorType_ {
  return ErrorType_{ErrC::Native, sys::last_error()};
}

inline auto ErrorType_::from_error_code(sys::ErrorCode e) -> ErrorType_ {
  return ErrorType_{ErrC::Native, sys::translate_native_error(e)};
}

END_NAMESPACE(n19);
