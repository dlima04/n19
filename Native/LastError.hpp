/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef NATIVE_LASTERROR_HPP
#define NATIVE_LASTERROR_HPP
#include <Native/String.hpp>

namespace n19::native {
  [[nodiscard]] auto last_error() -> String;
}

#endif //NATIVE_LASTERROR_HPP
