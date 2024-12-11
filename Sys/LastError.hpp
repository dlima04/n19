/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef SYS_LASTERROR_HPP
#define SYS_LASTERROR_HPP
#include <Sys/String.hpp>

#if defined(N19_WIN32)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#else
#  include <string.h>
#  include <errno.h>
#endif

namespace n19::sys {
  [[nodiscard]] auto last_error() -> String;
}

#endif //SYS_LASTERROR_HPP
