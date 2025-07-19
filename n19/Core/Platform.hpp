/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

// =========================================
// Compiler
// =========================================
#  if defined(__clang__)
#define N19_GCC   0
#define N19_CLANG 1
#define N19_MSVC  0
#  elif defined(__GNUC__)
#define N19_GCC   1
#define N19_CLANG 0
#define N19_MSVC  0
#  elif defined(_MSC_VER)
#define N19_GCC   0
#define N19_CLANG 0
#define N19_MSVC  1
#  else
#error "Core/Platform.hpp: Unsupported target!"
#  endif

#  if N19_MSVC
#  error "MSVC is currently not supported."
#  endif

// =========================================
// Build platform
// =========================================
#  if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#define N19_WIN32
#  elif defined(__APPLE__) && defined(__MACH__)
#define N19_DARWIN
#  elif defined(__linux__)
#define N19_LINUX
#  else
#error "Core/Platform.hpp: Unsupported target!"
#  endif

#  if defined(N19_DARWIN) || defined(N19_LINUX)
#define N19_POSIX
#  endif

// =========================================
// Cache line size guess
// =========================================
#  if defined(__x86_64__) || defined(_M_X64)
#define N19_CACHE_LINE_SIZE_GUESS 64
#  elif defined(__aarch64__)
#define N19_CACHE_LINE_SIZE_GUESS 64
#  else
#warning "Cache line size guess -- could not determine arch"
#define N19_CACHE_LINE_SIZE_GUESS 64
#  endif

#define N19_PACKED_IMPL_(KIND, NAME, BODY) \
    KIND __attribute__((packed)) \
    NAME \
    BODY;

#define PACKED_STRUCT_(NAME, BODY) N19_PACKED_IMPL_(struct, NAME, BODY)
#define PACKED_CLASS_(NAME, BODY)  N19_PACKED_IMPL_(class, NAME, BODY)

// =========================================
// Cpp attributes
// =========================================
#define NODISCARD_    [[nodiscard]]
#define UNUSED_       [[maybe_unused]]
#define NORETURN_     [[noreturn]]
#define FALLTHROUGH_  [[fallthrough]]

// =========================================
// GNU/Clang attributes
// =========================================
#define NAKED_        __attribute__((naked))
#define PACKED_       __attribute__((packed))
#define FORCEINLINE_  __attribute__((always_inline)) inline
#define NOINLINE_     __attribute__((noinline))
#define WEAK_         __attribute__((weak))
