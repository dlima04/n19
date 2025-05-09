/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef SYS_LASTERROR_HPP
#define SYS_LASTERROR_HPP
#include <Sys/String.hpp>
#include <Core/Platform.hpp>

#ifdef N19_WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <errno.h>
#endif

namespace n19::sys {
#ifdef N19_WIN32
  using ErrorCode = ::DWORD;
#else
  using ErrorCode = int;
#endif
  NODISCARD_ auto last_error() -> std::string;
  NODISCARD_ auto translate_native_error(ErrorCode) -> std::string;
}

#endif //SYS_LASTERROR_HPP
