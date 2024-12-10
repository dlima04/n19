/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef NOTHING_HPP
#define NOTHING_HPP

constexpr inline struct __Nothing {
  unsigned char __dummy_value = 0;
  constexpr __Nothing() = default;
} Nothing;

#endif //NOTHING_HPP
