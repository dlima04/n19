/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef COMPILEATTRS_H
#define COMPILEATTRS_H

#if defined(__clang__) || defined(__GNUC__)
  #define N19_NOINLINE    __attribute__((noinline))
  #define N19_FORCEINLINE __attribute__((always_inline)) inline
#elif defined(_MSC_VER)
  #define N19_NOINLINE    __declspec(noinline)
  #define N19_FORCEINLINE __forceinline inline
#else
  static_assert("CompileAttrs.h: Unknown compiler!");
  #define N19_NOINLINE
  #define N19_FORCEINLINE
#endif

#if defined(__clang__) || defined(__GNUC__)
  #define N19_PACKED_IMPL(KIND, NAME, BODY) \
    KIND __attribute__((packed)) NAME BODY;
#elif defined(_MSC_VER)
  #define N19_PACKED_IMPL(KIND, NAME, BODY) \
    __pragma(pack(push, 1))                 \
      KIND NAME BODY;                       \
    __pragma(pack(pop))
#else
  static_assert("CompileAttrs.h: Unknown compiler!");
  #define N19_PACKED_IMPL(KIND, NAME, BODY) \
    KIND NAME BODY
#endif

#define PACKED_STRUCT(NAME, BODY) N19_PACKED_IMPL(struct, NAME, BODY)
#define PACKED_CLASS(NAME, BODY)  N19_PACKED_IMPL(class, NAME, BODY)

#endif //COMPILEATTRS_H
