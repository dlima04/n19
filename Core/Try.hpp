/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef TRY_HPP
#define TRY_HPP
#include <Core/Result.hpp>
#include <Core/Fmt.hpp>
#include <Core/Panic.hpp>
#include <string>

inline thread_local auto __n19_last_errc = n19::ErrC::None;
inline thread_local auto __n19_last_msg  = std::string();

#define __N19_TRANSFORM_RESULT_IMPL                          \
  call_if_error([](auto&& value){                            \
    __n19_last_errc = value.error().code.value;              \
    __n19_last_msg  = value.error().msg;                     \
  });                                                        \

#define TRY(EXPR)                                            \
  (EXPR).__N19_TRANSFORM_RESULT_IMPL                         \
  if(__n19_last_errc != ::n19::ErrC::None) {                 \
    auto __tmp = __n19_last_errc;                            \
    __n19_last_errc = ::n19::ErrC::None;                     \
    return ::n19::make_error(__tmp, __n19_last_msg);         \
  }                                                          \

#define MUST(EXPR)                                           \
  (EXPR).__N19_TRANSFORM_RESULT_IMPL                         \
  if(__n19_last_errc != ::n19::ErrC::None) {                 \
    PANIC(::n19::fmt(                                        \
      "The expression \"" #EXPR                              \
      "\" Evaluated to an error in a \"MUST\" context. \n"   \
      "Error message = {} ",                                 \
     __n19_last_msg ));                                      \
  }

#define OR_RETURN()                                          \
  __N19_TRANSFORM_RESULT_IMPL                                \
  if(__n19_last_errc != ::n19::ErrC::None) {                 \
    auto __tmp = __n19_last_errc;                            \
    __n19_last_errc = ::n19::ErrC::None;                     \
    return ::n19::make_error(__tmp, __n19_last_msg);         \
  }                                                          \

#define OR_PANIC()                                           \
  __N19_TRANSFORM_RESULT_IMPL                                \
  if(__n19_last_errc != ::n19::ErrC::None) {                 \
    PANIC(::n19::fmt(                                        \
      "A n19::Result<T> evaluated to an error "              \
      "in a \"MUST\" context.\n Last error message = {}",    \
     __n19_last_msg ));                                      \
  }                                                          \

#endif //TRY_HPP
