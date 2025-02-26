/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef CALLBACK_HPP
#define CALLBACK_HPP
#include <Core/Concepts.hpp>
#include <Core/ClassTraits.hpp>
#include <Core/Platform.hpp>
#include <Misc/Macros.hpp>
#include <utility>
BEGIN_NAMESPACE(n19);

#define DEFER_IF(COND, ...)                        \
  ::n19::DeferImpl N19_UNIQUE_NAME(__n19defer)     \
  { [&]{ if((COND)){ __VA_ARGS__; }} }             \

#define DEFER(...)                                 \
  ::n19::DeferImpl N19_UNIQUE_NAME(__n19defer)     \
  { [&]{ __VA_ARGS__; } }                          \

template<typename T>
class Callback {
N19_MAKE_NONCOPYABLE(Callback);
public:
  template<CallableWith<T> ...Args>
  auto operator()(Args&&... args) -> decltype(auto);

  Callback(const T& obj) : obj_(obj) {}
  Callback(T&& obj     ) : obj_(std::move(obj)) {}
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
  DeferImpl(T&& obj     ) : Callback<T>(std::move(obj)) {}
};

template<typename T> template<CallableWith<T> ...Args>
FORCEINLINE_ auto Callback<T>::operator()(Args&&... args) -> decltype(auto) {
  return obj_( std::forward<Args>(args)... );
}

END_NAMESPACE(n19);
#endif //CALLBACK_HPP