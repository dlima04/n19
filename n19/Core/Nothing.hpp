/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

namespace n19 {
  struct Nothing_ {
    unsigned char dummy_value_ = 0;
    constexpr Nothing_() = default;
  };

  constexpr inline Nothing_ Nothing;
}

