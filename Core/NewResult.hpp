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
#include <Core/Nothing.hpp>
#include <Core/TypeTraits.hpp>
#include <Core/Forward.hpp>
#include <string>
#include <utility>
#include <type_traits>
#include <variant>
#include <expected>
BEGIN_NAMESPACE(n19);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define N19_ERRC_LIST \
  X(None)       \
  X(InvalidArg) \
  X(FileIO)     \
  X(Internal)   \
  X(NotFound)   \
  X(BadToken)   \
  X(Native)     \

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct __ErrC final {
N19_MAKE_COMPARABLE_MEMBER(__ErrC, value);
#define X(NAME) NAME,
  enum Value : uint16_t {
    N19_ERRC_LIST
  };
#undef X
  Value value = None;     // Underlying error value

  [[nodiscard]] auto to_string()   -> std::string;
  [[nodiscard]] auto is_critical() -> bool;

  constexpr __ErrC(const Value v) : value(v) {}
  constexpr __ErrC() = default;
};

struct __ErrorType {      // The default error type used by n19.
  std::string msg;        // The optional message. Can be empty.
  __ErrC code = __ErrC::None;
};

template<class T, class E = __ErrorType>
class [[nodiscard]] __Result {
public:
  using ValueType   = T;
  using PointerType = T*;
  using __Variant   = std::variant<T, E> ;

  // Invokes a callable object with a reference to this object
  // if it contains an error or value respectively.
  template<class C> auto call_if_error(this auto&& self, C&&) -> decltype(auto);
  template<class C> auto call_if_value(this auto&& self, C&&) -> decltype(auto);

  // For accessing value or error types.
  [[nodiscard]] auto has_value() const       -> bool;
  [[nodiscard]] auto value(this auto&& self) -> decltype(auto);
  [[nodiscard]] auto error(this auto&& self) -> decltype(auto);

  // value_or(): returns the contained value if it
  // exists, otherwise "val". error_or() is the same
  // but for the error type instead.
  [[nodiscard]] auto value_or(this auto&& self, T&& val) -> T;
  [[nodiscard]] auto error_or(this auto&& self, E&& val) -> E;

  // Reassignment ops, these can simply be defaulted as
  // variant will handle the semantics for us.
  auto operator=(__Result&& other)      -> __Result& = default;
  auto operator=(const __Result& other) -> __Result& = default;

  // Comparison. Can be between another __Result type,
  // or the type O itself which may be comparable with T.
  template<class O> auto operator==(const __Result<O>&)  -> bool;
  template<class O> auto operator==(const O&)            -> bool;

  auto operator->(this auto&& self) -> decltype(auto);
  auto operator*(this auto&& self)  -> decltype(auto);

  N19_FORCEINLINE __Result(__Result&& other)      = default;
  N19_FORCEINLINE __Result(const __Result& other) = default;

  N19_FORCEINLINE __Result(T&& value) : value_(forward<T>(value)) {}
  N19_FORCEINLINE __Result(E&& error) : value_(forward<E>(error)) {}
  N19_FORCEINLINE __Result(/*.....*/) : value_(E{}  ) {}
protected:
  __Variant value_;
};

template<class T>
struct __Result_Dispatch {
  using Type = __Result<T>;
};

template<>
struct __Result_Dispatch<void> {
  using Type = __Result<__Nothing>;
};

template<class T>
using Result = typename __Result_Dispatch<T>::Type;
using ErrC   = __ErrC;
using ErrorT = __ErrorType;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T, class E>
N19_FORCEINLINE auto __Result<T, E>::has_value() const -> bool {
  return std::holds_alternative<T>(value_);
}

template<class T, class E>
N19_FORCEINLINE auto __Result<T, E>::value(this auto&& self) -> decltype(auto) {
  ASSERT( self.has_value() == true && "Result contains an error!" );
  return std::get<T>( forward<decltype(self)>(self).value_ );
}

template<class T, class E>
N19_FORCEINLINE auto __Result<T, E>::error(this auto &&self) -> decltype(auto) {
  ASSERT( self.has_value() == false && "Result has no error!" );
  return std::get<E>( forward<decltype(self)>(self).value_ );
}

template<class T, class E>
N19_FORCEINLINE auto __Result<T, E>::value_or(this auto&& self, T&& val) -> T {
  if(self.has_value()) {
    return std::get<T>( forward<decltype(self)>(self).value_ );
  } return val; // else, return provided value type.
}

template<class T, class E>
N19_FORCEINLINE auto __Result<T, E>::error_or(this auto&& self, E&& val) -> E {
  if(!self.has_value()) {
    return std::get<E>( forward<decltype(self)>(self).value_ );
  } return val; // else, return provided error type.
}

template<class T, class E> /* Callable= */ template<class C>
auto __Result<T, E>::call_if_error(this auto&& self, C&& cb) -> decltype(auto) {
  if(!self.has_value()) cb( self );
  return self;
}

template<class T, class E> /* Callable= */ template<class C>
auto __Result<T, E>::call_if_value(this auto&& self, C&& cb) -> decltype(auto) {
  if(self.has_value()) cb( self );
  return self;
}

template<class T, class E> template<class O>
N19_FORCEINLINE auto __Result<T, E>::operator==(const __Result<O>& other) -> bool {
  if(has_value() != other.has_value()) {
    return false;
  } return !has_value() || value() == other.value();
}

template<class T, class E> template<class O>
N19_FORCEINLINE auto __Result<T, E>::operator==(const O& other) -> bool {
  return has_value() && other == value();
}

template<class T, class E>
N19_FORCEINLINE auto __Result<T, E>::operator->(this auto&& self) -> decltype(auto) {
  ASSERT( self.has_value() == true && "Result contains an error!" );
  return &( forward<decltype(self)>(self).value() );
}

template<class T, class E>
N19_FORCEINLINE auto __Result<T, E>::operator*(this auto&& self) -> decltype(auto) {
  ASSERT( self.has_value() == true && "Result contains an error!" );
  return forward<decltype(self)>(self).value();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename ...Args>
inline auto make_result(Args&&... args) -> Result<T> {
  return T{ std::forward<Args>(args)... };
}

inline auto make_error(const ErrC code) -> ErrorT {
  return ErrorT{ .msg = "", .code = code, };
}

inline auto make_error(const ErrC code, const std::string& msg) -> ErrorT {
  return ErrorT{ .msg = msg, .code = code };
}

inline auto make_error(const ErrC code, std::string&& msg) -> ErrorT {
  return ErrorT{ .msg = msg, .code = code };
}

END_NAMESPACE(n19);
#endif //RESULT_HPP
