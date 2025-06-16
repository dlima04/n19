/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <Core/Concepts.hpp>
#include <Core/ClassTraits.hpp>
#include <Core/Platform.hpp>
#include <Misc/Macros.hpp>
#include <utility>
BEGIN_NAMESPACE(n19);

#define DEFER_IF(COND, ...)                        \
  ::n19::DeferImpl N19_UNIQUE_NAME(n19defer_)      \
  { [&]{ if((COND)){ __VA_ARGS__; }} }             \

#define DEFER(...)                                 \
  ::n19::DeferImpl N19_UNIQUE_NAME(n19defer_)      \
  { [&]{ __VA_ARGS__; } }                          \

template<typename T>
class DeferBase_ {
  N19_MAKE_NONCOPYABLE(DeferBase_);
public:
  template<CallableWith<T> ...Args>
  auto operator()(Args&&... args) -> decltype(auto);

  DeferBase_(const T& obj) : obj_(obj) {}
  DeferBase_(T&& obj     ) : obj_(std::move(obj)) {}
protected:
  T obj_;
};

template<CallableWith T>
class DeferImpl final : public DeferBase_<T> {
  N19_MAKE_NONMOVABLE(DeferImpl);
  N19_MAKE_NONCOPYABLE(DeferImpl);
public:
  using DeferBase_<T>::obj_;
  using DeferBase_<T>::operator();

 ~DeferImpl() { obj_(); }
  DeferImpl(const T& obj) : DeferBase_<T>(obj) {}
  DeferImpl(T&& obj     ) : DeferBase_<T>(std::move(obj)) {}
};

template<typename T> template<CallableWith<T> ...Args>
FORCEINLINE_ auto DeferBase_<T>::operator()(Args&&... args) -> decltype(auto) {
  return obj_( std::forward<Args>(args)... );
}

END_NAMESPACE(n19);