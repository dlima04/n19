/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef SYS_LASTERROR_HPP
#define SYS_LASTERROR_HPP
#include <Sys/String.hpp>
#include <Core/Platform.hpp>

namespace n19::sys {
  NODISCARD_ auto last_error() -> String;
}

#endif //SYS_LASTERROR_HPP
