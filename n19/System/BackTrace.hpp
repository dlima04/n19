/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#define N19_BACKTRACE_MAX_FRAMES 42

#include <n19/Core/Common.hpp>
#include <n19/Core/Platform.hpp>
#include <n19/Core/Result.hpp>
#include <n19/System/File.hpp>
#include <n19/System/IODevice.hpp>
#include <n19/Core/Console.hpp>
#include <array>
#include <cstdint>
#include <utility>
BEGIN_NAMESPACE(n19::sys);

struct BacktraceFrame {
  std::string name_;
  void* addr_ = nullptr;
};

struct BackTrace {
  static auto dump_to(OStream& = outs()) -> Result<void>;
  static auto dump_to(File& file)        -> Result<void>;

  NODISCARD_ auto get() -> Result<void>;
  std::array<BacktraceFrame, N19_BACKTRACE_MAX_FRAMES> frames_;
};

END_NAMESPACE(n19::sys);
