/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef CORE_MACRO_UTIL_HPP
#define CORE_MACRO_UTIL_HPP

#define N19_MACRO_CONCAT_IMPL_(X, Y) X##Y
#define N19_MACRO_CONCAT(X, Y) N19_MACRO_CONCAT_IMPL_(X, Y)
#define N19_EXPAND_SAFE_IF(COND, ...) do{ if( COND ) __VA_ARGS__ }while(false);

#ifdef __COUNTER__
#   define N19_UNIQUE_NAME(X) N19_MACRO_CONCAT(X, __COUNTER__)
#else
#   define N19_UNIQUE_NAME(X)
#endif

#define EXPAND(...) __VA_ARGS__
#define STRINGIFY_IMPL(EXPR) #EXPR
#define STRINGIFY(EXPR) STRINGIFY_IMPL(EXPR)

#endif //CORE_MACRO_UTIL_HPP
