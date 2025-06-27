/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <Core/Common.hpp>
#include <System/String.hpp>
#include <Core/Platform.hpp>

#ifdef N19_WIN32
#include <System/Win32.hpp>
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