/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef NOTHING_HPP
#define NOTHING_HPP

struct Nothing_ {
  unsigned char dummy_value_ = 0;
  constexpr Nothing_() = default;
};

constexpr inline Nothing_ Nothing;
#endif //NOTHING_HPP
