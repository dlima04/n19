/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef TRY_HPP
#define TRY_HPP
#include <Core/Result.hpp>
#include <Core/Fmt.hpp>
#include <Core/Panic.hpp>
#include <Core/TypeTraits.hpp>
#include <string>
#include <type_traits>

inline thread_local auto n19_last_errc_ = n19::ErrC::None;
inline thread_local auto n19_last_msg_  = std::string{};

#define ERROR_IF(EXPR, ...) do {                               \
    if((EXPR)) { return ::n19::Error(__VA_ARGS__); }      \
  } while(0)                                                   \

#define N19_TRANSFORM_RESULT_IMPL_(EXPR)                       \
  (EXPR).call_if_error([](auto&& value){                       \
    ::n19_last_errc_ = value.error().code.value;               \
    ::n19_last_msg_  = value.error().msg;                      \
  }).value_or();                                               \

#define TRY(EXPR)                                              \
  N19_TRANSFORM_RESULT_IMPL_(EXPR)                             \
  if(::n19_last_errc_ != ::n19::ErrC::None) {                  \
    auto tmpe_ = ::n19_last_errc_;                             \
    ::n19_last_errc_ = ::n19::ErrC::None;                      \
    return ::n19::Error(tmpe_, ::n19_last_msg_);          \
  }                                                            \

#define MUST(EXPR)                                             \
  N19_TRANSFORM_RESULT_IMPL_(EXPR)                             \
  if(::n19_last_errc_ != ::n19::ErrC::None) {                  \
    PANIC(::n19::fmt(                                          \
      "The expression \"" #EXPR                                \
      "\" Evaluated to an error in a \"MUST\" context. \n"     \
      "Error message = {} ",                                   \
    ::n19_last_msg_ ));                                        \
  }                                                            \

#endif //TRY_HPP
