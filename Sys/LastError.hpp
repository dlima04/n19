/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef SYS_LASTERROR_HPP
#define SYS_LASTERROR_HPP
#include <Sys/String.hpp>

namespace n19::sys {
  [[nodiscard]] auto last_error() -> String;
}

#endif //SYS_LASTERROR_HPP
