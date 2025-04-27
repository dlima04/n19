/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef PLATFORM_HPP
#define PLATFORM_HPP

///
/// Should be 64 on the vast majority of architectures...
#if defined(__x86_64__) || defined(_M_X64)
#  define N19_CACHE_LINE_SIZE_GUESS 64
#elif defined(__aarch64__)
#  define N19_CACHE_LINE_SIZE_GUESS 64
#else
#  warning "Cache line size guess -- could not determine arch"
#  define N19_CACHE_LINE_SIZE_GUESS 64
#endif

///
/// Compiler definitions, for detecting which compiler is being used.
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
#  warning "Core/Platform.hpp: Unknown compiler!"
#  warning "Expected Clang, GCC, or MSVC."
#endif

///
/// Weird edge case where we're using Apple's version of Clang,
/// not sure if this will actually make any difference (probably not)
/// but probably still worth keeping here.
#if N19_CLANG && defined(N19_MACOS)
#  define N19_CLANG_APPLE 1
#else
#  define N19_CLANG_APPLE 0
#endif

///
/// The size of "regular" page (i.e. not a huge page which many
/// platforms permit under certain circumstances) on the platform we're
/// building for.
#if defined(N19_WIN32)
#  define PLATFORM_PAGE_SIZE_ 4096
#else
#  include <unistd.h>
#  define PLATFORM_PAGE_SIZE_ sysconf(_SC_PAGESIZE)
#endif

///
/// Portable macros for forcing/preventing function inlining.
#if N19_CLANG || N19_GCC
#  define NOINLINE_    __attribute__((noinline))
#  define FORCEINLINE_ __attribute__((always_inline)) inline
#elif N19_MSVC
#  define NOINLINE_    __declspec(noinline)
#  define FORCEINLINE_ __forceinline inline
#else
#  define NOINLINE_
#  define FORCEINLINE_
#endif

///
/// Portable macro for specifying packed structures.
#if N19_CLANG || N19_GCC
#  define N19_PACKED_IMPL_(KIND, NAME, BODY) \
    KIND __attribute__((packed)) NAME BODY;
#elif N19_MSVC
#  define N19_PACKED_IMPL_(KIND, NAME, BODY) \
  __pragma(pack(push, 1))                    \
    KIND NAME BODY;                          \
  __pragma(pack(pop))
#else
#  define N19_PACKED_IMPL_(KIND, NAME, BODY) \
    KIND NAME BODY;
#endif

#define PACKED_STRUCT_(NAME, BODY) N19_PACKED_IMPL_(struct, NAME, BODY)
#define PACKED_CLASS_(NAME, BODY)  N19_PACKED_IMPL_(class, NAME, BODY)

///
/// Note: it seems like MSVC doesn't have an equivalent attribute
/// for this. will need to find a workaround at some point.
#if N19_CLANG || N19_GCC
#  define USED_ __attribute__((used))
#else
#  define USED_
#endif

///
/// Portable macro for specifying naked functions,
/// i.e. ones that don't have prologues/epilogues in their generated code.
#if N19_CLANG || N19_GCC
#  define NAKED_ __attribute__((naked))
#elif N19_MSVC
#  define NAKED_ __declspec(naked)
#endif

///
/// Attribute macros that aren't compiler specific.
#define NODISCARD_   [[nodiscard]]
#define FALLTHROUGH_ [[fallthrough]]
#define NORETURN_    [[noreturn]]

#endif //PLATFORM_HPP
