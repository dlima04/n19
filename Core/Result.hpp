/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef RESULT_HPP
#define RESULT_HPP
#include <Core/Platform.hpp>
#include <Core/Panic.hpp>
#include <Core/ClassTraits.hpp>
#include <Core/TypeTraits.hpp>
#include <Core/Nothing.hpp>
#include <string>
#include <utility>
#include <variant>
BEGIN_NAMESPACE(n19);

struct ErrC_ final {
N19_MAKE_COMPARABLE_MEMBER(ErrC_, value);
  enum Value : uint16_t {
    None       = 0x00, /// No error has occurred.
    InvalidArg = 0x01, /// A provided argument is incorrect.
    FileIO     = 0x02, /// Filesystem error of some kind.
    Internal   = 0x03, /// Internal, shouldn't be exposed user side
    NotFound   = 0x04, /// Couldn't find what you're looking for.
    BadToken   = 0x05, /// For lexing usually.
    Native     = 0x06, /// Generic native / OS error.
    Conversion = 0x07, /// No valid conversion available.
  };
                       ///
  Value value = None;  /// Underlying error value
  constexpr ErrC_(const Value v) : value(v) {}
  constexpr ErrC_() = default;
};
                       ///
struct ErrorType_ {    /// n19's default error type.
  std::string msg;
  ErrC_ code = ErrC_::None;

  constexpr ErrorType_(ErrC_ c) : code(c) {}
  constexpr ErrorType_() = default;
  constexpr ErrorType_(ErrC_ c, const std::string& m)
  : msg(m) , code(c) {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Begin default result type.

template<typename T, typename E = ErrorType_>
class Result_ {
N19_MAKE_DEFAULT_MOVE_CONSTRUCTIBLE(Result_);
N19_MAKE_DEFAULT_COPY_CONSTRUCTIBLE(Result_);
public:
  using ValueType   = T;
  using PointerType = T*;
  using Variant_    = std::variant<T, E> ;

  template<typename ...Args>
  FORCEINLINE_ static auto create(Args&&... args) -> Result_ {
    return T{std::forward<Args>(args)...};
  }

  NODISCARD_ FORCEINLINE_ auto value() const -> const T& {
    ASSERT(has_value() == true, "Result contains an error!");
    return std::get<T>( value_ );
  }

  NODISCARD_ FORCEINLINE_ auto value() -> T& {
    ASSERT(has_value() == true, "Result contains an error!");
    return std::get<T>( value_ );
  }

  NODISCARD_ FORCEINLINE_ auto error() const -> const E& {
    ASSERT(has_value() == false, "Result contains a value!");
    return std::get<E>( value_ );
  }

  NODISCARD_ FORCEINLINE_ auto error() -> E& {
    ASSERT(has_value() == false, "Result contains a value!");
    return std::get<E>( value_ );
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
  : value_(std::forward<Args>(args)...){}

  NODISCARD_ auto has_value() const -> bool {
    return std::holds_alternative<T>( value_ );
  }

  NODISCARD_ explicit operator bool() const {
    return std::holds_alternative<T>( value_ );
  }

  Result_(T&& value) : value_(std::forward<T>(value)) {}
  Result_(E&& error) : value_(std::forward<E>(error)) {}
  Result_(/*.....*/) : value_(E{}  ) {}
protected:
  Variant_ value_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Begin type aliases

template<typename T>
struct Result_Dispatch_ {
  using Type = Result_<T>;
};

template<>
struct Result_Dispatch_<void> {
  using Type = Result_<Nothing_>;
};

template<typename T>
using Result = typename Result_Dispatch_<T>::Type;
using ErrC   = ErrC_;
using Error  = ErrorType_;

END_NAMESPACE(n19);
#endif //RESULT_HPP
