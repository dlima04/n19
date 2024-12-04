/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#if defined(__clang__)
#  define N19_GCC   0
#  define N19_CLANG 1
#  define N19_MSVC  0
#elif defined(__GNUC__)
#  define N19_GCC   1
#  define N19_CLANG 0
#  define N19_MSVC  0
#elif defined(_MSC_VER)
#  define N19_GCC   0
#  define N19_CLANG 0
#  define N19_MSVC  1
#else
#  warning Platform.hpp: Unknown compiler!
#  warning Expected Clang, GCC, or MSVC.
#endif

#if N19_CLANG && defined(N19_MACOS)
#  define N19_CLANG_APPLE 1
#else
#  define N19_CLANG_APPLE 0
#endif

#if defined(N19_WIN32)
#  define PLATFORM_PAGE_SIZE 4096
#else
#  include <unistd.h>
#  define PLATFORM_PAGE_SIZE sysconf(_SC_PAGESIZE)
#endif

#if N19_CLANG || N19_GCC
#  define N19_NOINLINE    __attribute__((noinline))
#  define N19_FORCEINLINE __attribute__((always_inline)) inline
#elif N19_MSVC
#  define N19_NOINLINE    __declspec(noinline)
#  define N19_FORCEINLINE __forceinline inline
#else
#  define N19_NOINLINE
#  define N19_FORCEINLINE
#endif

#if N19_CLANG || N19_GCC
#  define N19_PACKED_IMPL(KIND, NAME, BODY) \
    KIND __attribute__((packed)) NAME BODY;
#elif defined(_MSC_VER)
#  define N19_PACKED_IMPL(KIND, NAME, BODY) \
  __pragma(pack(push, 1))                   \
    KIND NAME BODY;                         \
  __pragma(pack(pop))
#else
#  define N19_PACKED_IMPL(KIND, NAME, BODY) \
    KIND NAME BODY
#endif

#define PACKED_STRUCT(NAME, BODY) N19_PACKED_IMPL(struct, NAME, BODY)
#define PACKED_CLASS(NAME, BODY)  N19_PACKED_IMPL(class, NAME, BODY)

#endif //PLATFORM_HPP
