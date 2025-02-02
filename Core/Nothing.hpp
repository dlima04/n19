/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef NOTHING_HPP
#define NOTHING_HPP

struct Nothing_ {
  unsigned char dummy_value_ = 0;
  constexpr Nothing_() = default;
};

constexpr inline Nothing_ Nothing;
#endif //NOTHING_HPP
