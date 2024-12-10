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
#include <utility>
#include <variant>
BEGIN_NAMESPACE(n19)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
class [[nodiscard]] Maybe {
public:
  using ValueType     = T;
  using PointerType   = T*;
  using ReferenceType = T&;
  using __Variant     = std::variant<T, __Nothing>;

  // Reassignment ops, these can simply be defaulted as
  // variant will handle the semantics for us.
  auto operator=(Maybe&& other)      -> Maybe& = default;
  auto operator=(const Maybe& other) -> Maybe& = default;

  // Comparison. Can be between another Maybe type, or
  // the type O as long as it's compatible with T.
  template<class O> auto operator==(const Maybe<O>&) -> bool;
  template<class O> auto operator==(const O&)        -> bool;

  // Accessor operators for cleaner syntax mostly.
  auto operator->(this auto&& self)  -> decltype(auto);
  auto operator*(this auto&& self)   -> decltype(auto);

  [[nodiscard]] auto has_value() const       -> bool;
  [[nodiscard]] auto value(this auto&& self) -> decltype(auto);

  // In-place construction of the value type T.
  template<class ... Args> auto emplace(Args&&... args) -> void;
  template<class ... Args> Maybe(Args&&... args);

  auto clear()   -> void;
  auto release() -> T;

  N19_FORCEINLINE Maybe(Maybe&& other)      = default;
  N19_FORCEINLINE Maybe(const Maybe& other) = default;

  N19_FORCEINLINE Maybe(T&& val)     : value_(forward<T>(val)) {}
  N19_FORCEINLINE Maybe(__Nothing&&) : value_(__Nothing{}) {}
  N19_FORCEINLINE Maybe(/*......*/)  : value_(__Nothing{}) {}
protected:
  bool has_value_ { false };
  __Variant value_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
N19_FORCEINLINE auto Maybe<T>::value(this auto&& self) -> decltype(auto) {
  ASSERT(self.has_value_ == true);
  return std::get<T>( forward<decltype(self)>(self).value_ );
}

template<class T>
N19_FORCEINLINE auto Maybe<T>::release() -> T {
  ASSERT(has_value_);
  T released = std::move( value() );
  has_value_ = false;
  value_     = Nothing;
  return released;
}

template<class T> template<class O>
N19_FORCEINLINE auto Maybe<T>::operator==(const Maybe<O>& other) -> bool {
  return has_value_ == other.has_value_ && (!has_value_ || value() == other.value());
}

template<class T> template<class O>
N19_FORCEINLINE auto Maybe<T>::operator==(const O& other) -> bool {
  return has_value_ && other == value();
}

template<class T>
N19_FORCEINLINE auto Maybe<T>::operator->(this auto&& self) -> decltype(auto) {
  ASSERT(self.has_value_ == true);
  return &( forward<decltype(self)>(self).value() );
}

template<class T>
N19_FORCEINLINE auto Maybe<T>::operator*(this auto &&self) -> decltype(auto) {
  ASSERT(self.has_value_ == true);
  return forward<decltype(self)>(self).value();
}

template<class T>
N19_FORCEINLINE auto Maybe<T>::has_value() const -> bool {
  return has_value_;
}

template<class T> template<class ... Args>
N19_FORCEINLINE Maybe<T>::Maybe(Args&& ...args){
  value_.template emplace<T>( forward<Args>(args)... );
  has_value_ = true;
}

template<class T> template<class ... Args>
auto Maybe<T>::emplace(Args &&...args) -> void {
  value_.template emplace<T>( forward<Args>(args)... );
  has_value_ = true;
}

template<class T>
N19_FORCEINLINE auto Maybe<T>::clear() -> void {
  if(has_value_) {
    value_ = Nothing;
    has_value_ = false;
  }
}

END_NAMESPACE(n19);
#endif //MAYBE_HPP
