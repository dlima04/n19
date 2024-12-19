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
#include <Core/Forward.hpp>
#include <string>
#include <utility>
#include <variant>
#include <type_traits>
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
  X(Conversion) \

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

template<typename T, typename E = __ErrorType>
class /* [[nodiscard]] */ __Result {
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

  N19_FORCEINLINE auto operator->(this auto&& self) -> decltype(auto) {
    ASSERT( self.has_value() == true, "Result contains an error!" );
    return &( forward<decltype(self)>(self).value() );
  }

  N19_FORCEINLINE auto operator*(this auto&& self) -> decltype(auto) {
    ASSERT( self.has_value() == true, "Result contains an error!" );
    return forward<decltype(self)>(self).value();
  }

  N19_FORCEINLINE auto value_or(this auto&& self, T&& val) -> T {
    if(self.has_value()) {  // return the error if it exists.
      return self.value();  // else ::
    } return val;           // return provided value type.
  }

  N19_FORCEINLINE auto error_or(this auto&& self, E&& val) -> E {
    if(!self.has_value()) { // return the error if it exists.
      return self.error();  // else ::
    } return val;           // return provided error type.
  }

  template<typename C> /* C = Callable type */
  N19_FORCEINLINE auto call_if_error(this auto&& self, C&& cb) -> decltype(self) {
    if(!self.has_value()) cb( forward<decltype(self)>(self) );
    return self;
  }

  template<typename C> /* C = Callable type */
  N19_FORCEINLINE auto call_if_value(this auto&& self, C&& cb) -> decltype(self) {
    if(self.has_value()) cb( forward<decltype(self)>(self) );
    return self;
  }

  template<typename O>
  N19_FORCEINLINE auto operator==(const __Result<O>& other) -> bool {
    if(has_value() != other.has_value()) {
      return false;
    } return !has_value() || value() == other.value();
  }

  template<typename O>
  N19_FORCEINLINE auto operator==(const O& other) -> bool {
    return has_value() && other == value();
  }

  [[nodiscard]] N19_FORCEINLINE auto has_value() const -> bool {
    return std::holds_alternative<T>( value_ );
  }

  [[nodiscard]] N19_FORCEINLINE explicit operator bool() const {
    return std::holds_alternative<T>( value_ );
  }

  N19_FORCEINLINE __Result(T&& value)       : value_(std::move(value)) {}
  N19_FORCEINLINE __Result(E&& error)       : value_(std::move(error)) {}
  N19_FORCEINLINE __Result(const T& value)  : value_(value) {}
  N19_FORCEINLINE __Result(const E& error)  : value_(error) {}
  N19_FORCEINLINE __Result(/*.....*/)       : value_(E{}  ) {}
protected:
  __Variant value_;
};

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

template<typename T, typename ...Args>
auto make_result(Args&&... args) -> Result<T> {
  if constexpr(std::is_void_v<T>)
    return Result<__Nothing>{__Nothing{}};
  else
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
