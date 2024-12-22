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
#include <Core/Concepts.hpp>
#include <Core/ClassTraits.hpp>
#include <utility>
#include <variant>
BEGIN_NAMESPACE(n19)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<Concrete T>
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

  auto operator->(this auto&& self) -> decltype(auto) {
    ASSERT( self.has_value_ == true, "Bad Maybe access!" );
    return &self.value();
  }

  auto operator*(this auto&& self) -> decltype(auto) {
    ASSERT( self.has_value_ == true, "Bad Maybe access!" );
    return self.value();
  }

  [[nodiscard]] auto value_or(T &&val) const -> T {
    if(has_value_) {                 /// return the value if it exists.
      return std::get<T>( value_ );  /// else ::
    } return val;                    /// return provided value type.
  }

  [[nodiscard]] auto release() -> T {
    T released = std::move( value() );
    has_value_ = false;
    value_     = Nothing;
    return released;
  }

  template<typename O>
  auto operator==(const Maybe<O>& other) -> bool {
    return has_value_ == other.has_value_
      && (!has_value_ || value() == other.value());
  }

  template<typename O>
  auto operator==(const O& other) -> bool {
    return has_value_ && other == value();
  }

  template<typename ... Args>
  N19_FORCEINLINE explicit Maybe(Args&& ...args){
    value_.template emplace<T>( forward<Args>(args)... );
    has_value_ = true;
  }

  template<typename U> requires std::constructible_from<T, U>
  N19_FORCEINLINE Maybe(U&& value) {
    value_.template emplace<T>( forward<U>(value) );
    has_value_ = true;
  }

  template<typename ... Args>
  N19_FORCEINLINE auto emplace(Args &&...args) -> void {
    value_.template emplace<T>( forward<Args>(args)... );
    has_value_ = true;
  }

  N19_FORCEINLINE auto clear() -> void {
    value_      = Nothing;
    has_value_  = false;
  }

  N19_FORCEINLINE auto has_value() const -> bool { return has_value_; }
  N19_FORCEINLINE explicit operator bool() const { return has_value_; }

  Maybe(const T& val)     : has_value_(true), value_(val) {}
  Maybe(T&& val)          : has_value_(true), value_(std::move(val)) {}
  Maybe(const __Nothing&) : value_(__Nothing{}) {}
  Maybe(/*......*/)       : value_(__Nothing{}) {}
protected:
  bool has_value_ { false };
  __Variant value_;
};

END_NAMESPACE(n19);
#endif //MAYBE_HPP
