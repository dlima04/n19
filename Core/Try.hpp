/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <Core/Panic.hpp>
#include <Core/TypeTraits.hpp>
#include <Core/Platform.hpp>

/*
* Note: this file assumes that Core/Result.hpp
* has already been included.
*/

#if N19_MSVC
#error "MSVC does not support statement expressions."
#endif

#define TRY(EXPR)                                                                  \
  ({                                                                               \
    auto&& temp_result_ = (EXPR);                                                  \
    static_assert(!::n19::IsLvalueReference<decltype((EXPR).release_value())>);    \
    if(!temp_result_.has_value()) return temp_result_.release_error();             \
    temp_result_.release_value();                                                  \
  })                                                                               \

#define MUST(EXPR)                                                                 \
  ({                                                                               \
    auto&& temp_result_ = (EXPR);                                                  \
    static_assert(!::n19::IsLvalueReference<decltype((EXPR).release_value())>);    \
    ASSERT(temp_result_.has_value(), "MUST() expression evaluated to an error!");  \
    temp_result_.release_value();                                                  \
  })                                                                               \

#define ERROR_IF(EXPR, ...) do {                                                   \
    if((EXPR)) { return ::n19::Error{ __VA_ARGS__ }; }                             \
  } while(0)                                                                       \

#define ERROR_IF_NOT(EXPR, ...) do {                                               \
    if(!(EXPR)) { return ::n19::Error{ __VA_ARGS__ }; }                            \
  } while(0)                                                                       \


