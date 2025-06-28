/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

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
#  error "Core/Platform.hpp: Unsupported target!"
#endif

///
/// Attempt to detect target operating system...
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#  define N19_WIN32
#elif defined(__APPLE__) && defined(__MACH__)
#  define N19_DARWIN
#elif defined(__linux__)
#  define N19_LINUX
#else
#  error "Core/Platform.hpp: Unsupported target!"
#endif

#if defined(N19_DARWIN) || defined(N19_LINUX)
#  define N19_POSIX
#endif

///
/// The page size of the platform we're building for.
#if defined(N19_WIN32)
#  define PLATFORM_PAGE_SIZE_ 4096
#else
#  include <unistd.h>
#  define PLATFORM_PAGE_SIZE_ ::sysconf(_SC_PAGESIZE)
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
/// Portable macro for specifying naked functions.
#if N19_CLANG || N19_GCC
#  define NAKED_ __attribute__((naked))
#elif N19_MSVC
#  define NAKED_ __declspec(naked)
#else
#  error "No cpp attribute present -- naked"
#endif

///
/// Attribute macros that aren't compiler specific.
#if __has_cpp_attribute(nodiscard)
#  define NODISCARD_ [[nodiscard]]
#else
#  error "No cpp attribute present -- [[nodiscard]]"
#endif

#if __has_cpp_attribute(fallthrough)
#  define FALLTHROUGH_ [[fallthrough]]
#else
#  error "No cpp attribute present -- [[fallthrough]]"
#endif

#if __has_cpp_attribute(noreturn)
#  define NORETURN_ [[noreturn]]
#else
#  error "No cpp attribute present -- [[noreturn]]"
#endif
