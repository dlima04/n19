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
#include <Core/TypeTraits.hpp>
#include <string>
#include <type_traits>

inline thread_local auto n19_last_errc_ = n19::ErrC::None;
inline thread_local auto n19_last_msg_  = std::string{};

#define N19_TRANSFORM_RESULT_IMPL_(EXPR)                          \
  [&]() -> DecayT<decltype(*(EXPR))> {                            \
    using __ExprType = DecayT<decltype(*(EXPR))>;                 \
    static_assert(                                                \
      ::std::is_default_constructible_v<__ExprType>,              \
      "Unwrapped type must be default constructible!"             \
    ); /* This is a limitation. */                                \
    return (EXPR).call_if_error([](auto&& value){                 \
      ::n19_last_errc_ = value.error().code.value;                \
      ::n19_last_msg_  = value.error().msg;                       \
    }).value_or();                                                \
  }();                                                            \

#define TRY(EXPR)                                                 \
  N19_TRANSFORM_RESULT_IMPL_(EXPR)                                \
  if(::n19_last_errc_ != ::n19::ErrC::None) {                     \
    auto tmpe_ = ::n19_last_errc_;                                \
    ::n19_last_errc_ = ::n19::ErrC::None;                         \
    return ::n19::make_error(tmpe_, ::n19_last_msg_);             \
  }                                                               \

#define MUST(EXPR)                                                \
  N19_TRANSFORM_RESULT_IMPL_(EXPR)                                \
  if(::n19_last_errc_ != ::n19::ErrC::None) {                     \
    PANIC(::n19::fmt(                                             \
      "The expression \"" #EXPR                                   \
      "\" Evaluated to an error in a \"MUST\" context. \n"        \
      "Error message = {} ",                                      \
     ::n19_last_msg_ ));                                          \
  }                                                               \

#endif //TRY_HPP
