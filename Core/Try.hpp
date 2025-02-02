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

inline thread_local auto n19_last_errc_ = n19::ErrC::None;
inline thread_local auto n19_last_msg_  = std::string();

#define N19_TRANSFORM_RESULT_IMPL_                         \
  call_if_error([](auto&& value){                          \
    ::n19_last_errc_ = value.error().code.value;           \
    ::n19_last_msg_  = value.error().msg;                  \
  });                                                      \

#define TRY(EXPR)                                          \
  (EXPR).N19_TRANSFORM_RESULT_IMPL_                        \
  if(::n19_last_errc_ != ::n19::ErrC::None) {              \
    auto tmpe_ = ::n19_last_errc_;                         \
    ::n19_last_errc_ = ::n19::ErrC::None;                  \
    return ::n19::make_error(tmpe_, ::n19_last_msg_);      \
  }                                                        \

#define MUST(EXPR)                                         \
  (EXPR).N19_TRANSFORM_RESULT_IMPL_                        \
  if(::n19_last_errc_ != ::n19::ErrC::None) {              \
    PANIC(::n19::fmt(                                      \
      "The expression \"" #EXPR                            \
      "\" Evaluated to an error in a \"MUST\" context. \n" \
      "Error message = {} ",                               \
     ::n19_last_msg_ ));                                   \
  }

#define OR_RETURN()                                        \
  N19_TRANSFORM_RESULT_IMPL_                               \
  if(::n19_last_errc_ != ::n19::ErrC::None) {              \
    auto tmpe_ = ::n19_last_errc_;                         \
    ::n19_last_errc_ = ::n19::ErrC::None;                  \
    return ::n19::make_error(tmpe_, ::n19_last_msg_);      \
  }                                                        \

#define OR_PANIC()                                         \
  N19_TRANSFORM_RESULT_IMPL_                               \
  if(::n19_last_errc_ != ::n19::ErrC::None) {              \
    PANIC(::n19::fmt(                                      \
      "A n19::Result<T> evaluated to an error "            \
      "in a \"MUST\" context.\n Last error message = {}",  \
     ::n19_last_msg_ ));                                   \
  }                                                        \

#endif //TRY_HPP
