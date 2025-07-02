/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <Core/Common.hpp>
#include <Core/Platform.hpp>
#include <Core/Panic.hpp>
#include <System/Win32.hpp>
#include <System/Handle.hpp>
#include <System/String.hpp>
#include <Core/Result.hpp>
#include <Core/Panic.hpp>
#include <memory>
#include <cstdint>
BEGIN_NAMESPACE(n19::sys);

/*
 * The handle type for n19::sys::SharedRegion.
 */
#ifdef N19_WIN32
using SharedRegionHandleType_ = Handle<::HANDLE>;
#else // POSIX
using SharedRegionHandleType_ = Handle<int>;
#endif


/* n19::sys::SharedRegion
 *
 * An abstraction for shared memory regions.
 * USE WITH CAUTION! One can easily cause UB with this
 * class, due to C++'s horrible, horrible, horrible
 * lifetime system.
 */
class SharedRegion : public SharedRegionHandleType_ {
  N19_MAKE_DEFAULT_ASSIGNABLE(SharedRegion);
  N19_MAKE_DEFAULT_CONSTRUCTIBLE(SharedRegion);
public:
  auto invalidate() -> void override;
  auto is_invalid() -> bool override;
  auto close()      -> void override;
  auto destroy()    -> void;

  static auto create(
    const String& name,
    size_t max_size,
    size_t size = 0) -> Result<SharedRegion>;

  static auto create_or_open(
    const String& name,
    size_t max_size,
    size_t size = 0) -> Result<SharedRegion>;

  static auto open(
    const String& name,
    size_t size) -> Result<SharedRegion>;

  size_t size() const { return size_; }
  const String& name() const { return name_; }

  void* get() const {
    ASSERT(addr_ != nullptr);
    return addr_;
  }

  SharedRegion() = default;
 ~SharedRegion() = default;
private:
  static auto create_impl_(
    const String& name,
    size_t max_length,
    bool open_if_exists,
    size_t length
  ) -> Result<SharedRegion>;

  void* addr_ = nullptr;
  size_t size_ = 0;
  sys::String name_;
};

END_NAMESPACE(n19::sys);
