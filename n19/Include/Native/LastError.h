/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef NATIVE_LASTERROR_H
#define NATIVE_LASTERROR_H
#include <Native/String.h>

namespace n19::native {
  [[nodiscard]] auto last_error() -> String;
}

#endif //LASTERROR_H
