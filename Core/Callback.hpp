/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef CALLBACK_HPP
#define CALLBACK_HPP
#include <Core/Concepts.hpp>
#include <Core/Forward.hpp>
#include <Core/ClassTraits.hpp>
#include <Core/Platform.hpp>
BEGIN_NAMESPACE(n19);

#define DEFER_IF(COND, ...) ::n19::DeferImpl _([&]{ if((COND)){ __VA_ARGS__; }})
#define DEFER(...)          ::n19::DeferImpl _([&]{ __VA_ARGS__; })

// n19::Callback is a lightweight wrapper around an
// invocable object. It should ONLY be used with
// function pointers or lambdas. Expensive objects with
// overloaded call operators should never be used.
// It's also not default constructible to keep things simple.

template<class T>
class Callback {
N19_MAKE_NONCOPYABLE(Callback);
public:
  template<CallableWith<T> ...Args>
  auto operator()(Args&&... args) -> decltype(auto);

  Callback(const T& obj) : obj_(obj) {}
  Callback(T&& obj     ) : obj_(obj) {}
protected:
  T obj_;
};

template<CallableWith T>
class DeferImpl final : public Callback<T> {
  N19_MAKE_NONMOVABLE(DeferImpl);
  N19_MAKE_NONCOPYABLE(DeferImpl);
public:
  using Callback<T>::obj_;
  using Callback<T>::operator();

 ~DeferImpl() { obj_(); }
  DeferImpl(const T& obj) : Callback<T>(obj) {}
  DeferImpl(T&& obj     ) : Callback<T>(obj) {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T> template<CallableWith<T> ... Args>
N19_FORCEINLINE auto Callback<T>::operator()(Args &&...args) -> decltype(auto) {
  return obj_( forward<Args>(args)... );
}

END_NAMESPACE(n19);
#endif //CALLBACK_HPP