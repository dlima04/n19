/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <n19/Core/Common.hpp>
#include <n19/Core/Platform.hpp>
#include <n19/Core/ClassTraits.hpp>
#include <n19/System/IODevice.hpp>
#include <cstdint>
#include <filesystem>
BEGIN_NAMESPACE(n19::sys);

#if defined(N19_WIN32)
using SeekDist = ::LONGLONG;
#else
using SeekDist = off_t;
#endif

enum class FSeek : uint8_t {
  Beg, End, Cur,
};

class File final : public IODevice {
  N19_MAKE_DEFAULT_CONSTRUCTIBLE(File);
  N19_MAKE_DEFAULT_ASSIGNABLE(File);
public:
  File() = default;
 ~File() override = default;

  NODISCARD_ auto path() const -> std::filesystem::path;
  NODISCARD_ auto size() const -> Result<size_t>;
  NODISCARD_ auto dev()  const -> IODevice;
  auto seek(SeekDist dist, FSeek method) const -> Result<SeekDist>;

  NODISCARD_ static auto create_or_open(
    const String& name,
    const bool append = false,
    const uint8_t perms = Read | Write
  ) -> Result<File>;

  NODISCARD_ static auto open(
    const String& name,
    const bool append = false,
    const uint8_t perms = Read | Write
  ) -> Result<File>;

  NODISCARD_ static auto create_trunc(
    const String& name,
    const uint8_t perms = Read | Write
  ) -> Result<File>;

  sys::String name_;
};

END_NAMESPACE(n19::sys);
