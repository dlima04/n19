/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef RESULTMACROS_HPP
#define RESULTMACROS_HPP
#include <Core/Result.hpp>
#include <Core/Panic.hpp>
#include <Core/Fmt.hpp>

inline thread_local auto n19_last_errc_ = n19::ErrC::None;
inline thread_local auto n19_last_msg_  = std::string();

// Provides additional support for n19::Result<T>
// in the form of a set of macros that allow us to "try"
// for errors, unwrap them safely, and method chain on objects
// that return Results from their methods.

#define N19_TRANSFORM_RESULT_IMPL_                          \
  transform_error([&](                                      \
  const ::n19::ErrorDescriptor& err_) {                     \
    n19_last_errc_ = err_.code;                             \
    n19_last_msg_  = err_.msg;                              \
    return ::n19::ErrorDescriptor{ err_ };                  \
  });                                                       \

#define TRY(EXPR)                                           \
  (EXPR).N19_TRANSFORM_RESULT_IMPL_                         \
  if(n19_last_errc_ != ::n19::ErrC::None) {                 \
    auto _tmp = n19_last_errc_;                             \
    n19_last_errc_ = ::n19::ErrC::None;                     \
    return ::n19::make_error(_tmp, "{}", n19_last_msg_);    \
  }                                                         \

#define MUST(EXPR)                                          \
  (EXPR).N19_TRANSFORM_RESULT_IMPL_                         \
  if(n19_last_errc_ != ::n19::ErrC::None) {                 \
    PANIC(::n19::fmt(                                       \
      "The expression \""                                   \
      #EXPR                                                 \
      "\" Evaluated to an error in a \"MUST\" context. \n"  \
      "Error message = {} ",                                \
      n19_last_msg_                                         \
    ));                                                     \
  }                                                         \

#define OR_RETURN()                                         \
  N19_TRANSFORM_RESULT_IMPL_                                \
  if(n19_last_errc_ != ::n19::ErrC::None) {                 \
    auto _tmp = n19_last_errc_;                             \
    n19_last_errc_ = ::n19::ErrC::None;                     \
    return ::n19::make_error(_tmp, "{}", n19_last_msg_);    \
  }                                                         \

#define OR_PANIC()                                          \
  N19_TRANSFORM_RESULT_IMPL_                                \
  if(n19_last_errc_ != ::n19::ErrC::None) {                 \
    PANIC(::n19::fmt(                                       \
      "A n19::Result evaluated to an error "                \
      "in a \"MUST\" context.\n Error message = {}",        \
      n19_last_msg_                                         \
    ));                                                     \
  }                                                         \

#define THEN(EXPR)                                          \
  and_then([&](auto&& _){                                   \
    return (EXPR);                                          \
  })                                                        \

#endif //RESULTMACROS_HPP
