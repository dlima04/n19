/*
* Copyright (c) 2025 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef CORE_MACRO_UTIL_HPP
#define CORE_MACRO_UTIL_HPP

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Miscellanous macro utilities that don't really fit inside any other
// header file should be placed here.

#define __N19_MACRO_CONCAT_IMPL(X, Y) X##Y
#define N19_MACRO_CONCAT(X, Y) __N19_MACRO_CONCAT_IMPL(X, Y)
#define N19_UNIQUE_NAME(X) N19_MACRO_CONCAT(X, __COUNTER__)
#define N19_EXPAND_SAFE_IF(COND, __VA_ARGS__) do{ if( COND ) __VA_ARGS__ }while(false);

#endif //CORE_MACRO_UTIL_HPP
