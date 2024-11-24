/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef GLOBALLASTERROR_H
#define GLOBALLASTERROR_H
#include <Core/Result.h>

// Global error values.
// Used by a few n19::Result related macros that need
// to produce error side effects to work correctly.

static thread_local auto n19_last_errc_ = n19::ErrC::None;
static thread_local auto n19_last_msg_  = std::string();

#endif //GLOBALLASTERROR_H
