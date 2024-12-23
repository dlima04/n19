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

struct __ErrC final {
N19_MAKE_COMPARABLE_MEMBER(__ErrC, value);
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
  constexpr __ErrC(const Value v) : value(v) {}
  constexpr __ErrC() = default;
};
                       ///
struct __ErrorType {   /// n19's default error type.
  std::string msg;
  __ErrC code = __ErrC::None;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Begin default result type.

template<typename T, typename E = __ErrorType>
class __Result {
N19_MAKE_DEFAULT_MOVE_CONSTRUCTIBLE(__Result);
N19_MAKE_DEFAULT_COPY_CONSTRUCTIBLE(__Result);
public:
  using ValueType   = T;
  using PointerType = T*;
  using __Variant   = std::variant<T, E> ;

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

  __Result(T&& value)      : value_(std::move(value)) {}
  __Result(E&& error)      : value_(std::move(error)) {}
  __Result(const T& value) : value_(value) {}
  __Result(const E& error) : value_(error) {}
  __Result(/*.....*/)      : value_(E{}  ) {}
protected:
  __Variant value_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Begin type aliases

template<typename T>
struct __Result_Dispatch {
  using Type = __Result<T>;
};

template<>
struct __Result_Dispatch<void> {
  using Type = __Result<__Nothing>;
};

template<typename T>
using Result = typename __Result_Dispatch<T>::Type;
using ErrC   = __ErrC;
using ErrorT = __ErrorType;

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
    return Result<__Nothing>{__Nothing{}};
  else
    return T{ std::forward<Args>(args)... };
}

END_NAMESPACE(n19);
#endif //RESULT_HPP
