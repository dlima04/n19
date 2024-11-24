/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef RESULTMACROS_H
#define RESULTMACROS_H
#include <Core/Result.h>
#include <Core/GlobalLastError.h>
#include <Core/Panic.h>
#include <Core/Fmt.h>
#include <type_traits>

// Provides additional support for n19::Result<T>
// in the form of a set of macros that allow us to "try"
// for errors, unwrap them safely, and method chain on objects
// that return Results from their methods.

#define N19_UNWRAP_RESULT_IMPL_(EXPR)                       \
  [&]() -> std::remove_reference_t<decltype((EXPR))> {      \
    const auto temp_ = (EXPR);                              \
    if(!temp_.has_value()) {                                \
      n19_last_errc_ = temp_.error().code;                  \
      n19_last_msg_  = temp_.error().msg;                   \
    } else {                                                \
      n19_last_errc_ = ::n19::ErrC::None;                   \
    }                                                       \
    return temp_;                                           \
  }();                                                      \

// TRY() and MUST():
// When used on an expression that produces a
// n19::Result<T>, attempts to produce the Result with
// the expected value. If it cannot do this, the error
// is either returned from the function, or an assert is
// raised.

#define TRY(EXPR)                                           \
  N19_UNWRAP_RESULT_IMPL_(EXPR)                             \
  if(n19_last_errc_ != ::n19::ErrC::None) {                 \
    return ::n19::make_error(                               \
      n19_last_errc_,                                       \
      "{}",                                                 \
      n19_last_msg_);                                       \
  }                                                         \

#define MUST(EXPR)                                          \
  N19_UNWRAP_RESULT_IMPL_(EXPR)                             \
  if(n19_last_errc_ != ::n19::ErrC::None) {                 \
    PANIC(::n19::fmt(                                       \
      "The expression \""                                   \
      #EXPR                                                 \
      "\" Evaluated to an error in a \"MUST\" context. \n"  \
      "Error message = {} ",                                \
      n19_last_msg_                                         \
    ));                                                     \
  }                                                         \

// Used for method chaining on classes that use
// n19::Result<None> as a return value.
// example:
// foo.do_something()
//  .THEN(foo.something())
//  .THEN(foo.something_else())
//  .OR_RETURN();

#define N19_TRANSFORM_RESULT_IMPL_                          \
  transform_error([&](                                      \
  const ::n19::ErrorDescriptor& err_) {                     \
    n19_last_errc_ = err_.code;                             \
    n19_last_msg_  = err_.msg;                              \
    return decltype(err_)(err_);                            \
  });                                                       \

#define OR_RETURN()                                         \
  N19_TRANSFORM_RESULT_IMPL_                                \
  if(n19_last_errc_ != ::n19::ErrC::None) {                 \
    return ::n19::make_error(                               \
      n19_last_errc_, "{}", n19_last_msg_);                 \
  }                                                         \

#define OR_PANIC()                                          \
  N19_TRANSFORM_RESULT_IMPL_                                \
  if(n19_last_errc_ != ::n19::ErrC::None) {                 \
    PANIC(::n19::fmt(                                       \
      "An n19::Result evaluated to an error "               \
      "in a situation where failure is not allowed.\n"      \
      "Error message = {} ",                                \
      n19_last_msg_                                         \
    ));                                                     \
  }                                                         \

#define THEN(EXPR)                                          \
  and_then([&](auto&& _){                                   \
    return (EXPR);                                          \
  })                                                        \

#endif //RESULTMACROS_H
