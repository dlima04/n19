/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
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
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Begin error code, default error types.

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
struct ErrorType_ {   /// n19's default error type.
  std::string msg;
  ErrC_ code = ErrC_::None;
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

  [[nodiscard]] N19_FORCEINLINE auto value() const -> const T& {
    ASSERT( has_value() == true, "Result contains an error!" );
    return std::get<T>( value_ );
  }

  [[nodiscard]] N19_FORCEINLINE auto value() -> T& {
    ASSERT( has_value() == true, "Result contains an error!" );
    return std::get<T>( value_ );
  }

  [[nodiscard]] N19_FORCEINLINE auto error() const -> const E& {
    ASSERT( has_value() == false, "Result contains a value!" );
    return std::get<E>( value_ );
  }

  [[nodiscard]] N19_FORCEINLINE auto error() -> E& {
    ASSERT( has_value() == false, "Result contains a value!" );
    return std::get<E>( value_ );
  }

  auto operator->(this auto&& self) -> decltype(auto) {
    ASSERT( self.has_value() == true, "Result contains an error!" );
    return &(std::forward<decltype(self)>(self).value());
  }

  auto operator*(this auto&& self) -> decltype(auto) {
    ASSERT( self.has_value() == true, "Result contains an error!" );
    return std::forward<decltype(self)>(self).value();
  }

  auto value_or(this auto&& self, T&& val) -> T {
    if(self.has_value())    /// return the error if it exists.
      return self.value();  /// else:
    return val;             /// return provided value type.
  }

  auto error_or(this auto&& self, E&& val) -> E {
    if(!self.has_value())   /// return the error if it exists.
      return self.error();  /// else:
    return val;             /// return provided error type.
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

  [[nodiscard]] auto has_value() const -> bool {
    return std::holds_alternative<T>( value_ );
  }

  [[nodiscard]] explicit operator bool() const {
    return std::holds_alternative<T>( value_ );
  }

  Result_(T&& value)      : value_(std::move(value)) {}
  Result_(E&& error)      : value_(std::move(error)) {}
  Result_(const T& value) : value_(value) {}
  Result_(const E& error) : value_(error) {}
  Result_(/*.....*/)      : value_(E{}  ) {}
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
using ErrorT = ErrorType_;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Begin error type helper functions

inline ErrorT make_error(const ErrC code) {
  return ErrorT{.msg = "", .code = code};
}

inline ErrorT make_error(const ErrC code, const std::string& msg) {
  return ErrorT{.msg = msg, .code = code};
}

inline ErrorT make_error(const ErrC code, std::string&& msg) {
  return ErrorT{.msg = msg, .code = code};
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Utility function for constructing result types.

template<typename T, typename ...Args>
[[nodiscard]] Result<T> make_result(Args&&... args) {
  if constexpr(IsVoid<T>)
    return Result<Nothing_>{Nothing_{}};
  else
    return T{ std::forward<Args>(args)... };
}

END_NAMESPACE(n19);
#endif //RESULT_HPP
