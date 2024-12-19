/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef MAYBE_HPP
#define MAYBE_HPP
#include <Core/Platform.hpp>
#include <Core/Panic.hpp>
#include <Core/Nothing.hpp>
#include <Core/Forward.hpp>
#include <Core/ClassTraits.hpp>
#include <utility>
#include <variant>
BEGIN_NAMESPACE(n19)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
class /* [[nodiscard]] */ Maybe {
N19_MAKE_DEFAULT_CONSTRUCTIBLE(Maybe);
N19_MAKE_DEFAULT_ASSIGNABLE(Maybe);
public:
  using ValueType     = T;
  using PointerType   = T*;
  using ReferenceType = T&;
  using __Variant     = std::variant<T, __Nothing>;

  [[nodiscard]] N19_FORCEINLINE auto value() const -> const T& {
    ASSERT( has_value_ == true, "Bad Maybe access!" );
    return std::get<T>( value_ );
  }

  [[nodiscard]] N19_FORCEINLINE auto value() -> T& {
    ASSERT(has_value_ == true, "Bad Maybe access!");
    return std::get<T>( value_ );
  }

  N19_FORCEINLINE auto operator->(this auto&& self) -> decltype(auto) {
    ASSERT( self.has_value_ == true, "Bad Maybe access!" );
    return &self.value();
  }

  N19_FORCEINLINE auto operator*(this auto&& self) -> decltype(auto) {
    ASSERT( self.has_value_ == true, "Bad Maybe access!" );
    return self.value();
  }

  [[nodiscard]] N19_FORCEINLINE auto value_or(T &&val) const -> T {
    if(has_value_) {                 // return the value if it exists.
      return std::get<T>( value_ );  // else ::
    } return val;                    // return provided value type.
  }

  [[nodiscard]] N19_FORCEINLINE auto release() -> T {
    T released = std::move( value() );
    has_value_ = false;
    value_     = Nothing;
    return released;
  }

  template<class O>
  N19_FORCEINLINE auto operator==(const Maybe<O>& other) -> bool {
    return has_value_ == other.has_value_
      && (!has_value_ || value() == other.value());
  }

  template<class O>
  N19_FORCEINLINE auto operator==(const O& other) -> bool {
    return has_value_ && other == value();
  }

  template<class ... Args>
  N19_FORCEINLINE explicit Maybe(Args&& ...args){
    value_.template emplace<T>( forward<Args>(args)... );
    has_value_ = true;
  }

  template<class U> requires std::constructible_from<T, U>
  N19_FORCEINLINE Maybe(U&& value) {
    value_.template emplace<T>( forward<U>(value) );
    has_value_ = true;
  }

  template<class ... Args>
  N19_FORCEINLINE auto emplace(Args &&...args) -> void {
    value_.template emplace<T>( forward<Args>(args)... );
    has_value_ = true;
  }

  N19_FORCEINLINE auto clear() -> void {
    value_      = Nothing;
    has_value_  = false;
  }

  auto has_value() const -> bool { return has_value_; }
  explicit operator bool() const { return has_value_; }

  N19_FORCEINLINE Maybe(const T& val)     : has_value_(true), value_(val) {}
  N19_FORCEINLINE Maybe(T&& val)          : has_value_(true), value_(std::move(val)) {}
  N19_FORCEINLINE Maybe(const __Nothing&) : value_(__Nothing{}) {}
  N19_FORCEINLINE Maybe(/*......*/)       : value_(__Nothing{}) {}
protected:
  bool has_value_ { false };
  __Variant value_;
};

END_NAMESPACE(n19);
#endif //MAYBE_HPP
