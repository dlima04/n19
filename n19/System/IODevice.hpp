/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <n19/Core/Common.hpp>
#include <n19/Core/Platform.hpp>
#include <n19/Core/TypeTraits.hpp>
#include <n19/System/Handle.hpp>
#include <n19/System/String.hpp>
#include <n19/Core/Bytes.hpp>
#include <n19/Core/Result.hpp>
#include <array>
#include <cstdint>
#include <span>

#if defined(N19_WIN32)
#include <n19/System/Win32.hpp>
#else // POSIX
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#endif

BEGIN_NAMESPACE(n19::sys);
#if defined(N19_WIN32)
using IODeviceBase_ = Handle<::HANDLE>;
#else
using IODeviceBase_ = Handle<int>;
#endif

class IODevice : public IODeviceBase_ {
public:
  using IODeviceBase_::ValueType;
  enum Permissions : uint8_t {
    NoAccess = 0x00,
    Read     = 0x01,
    Write    = 0x01 << 1,
    Execute  = 0x01 << 2,
  };

  auto close()      -> void override;
  auto invalidate() -> void override;
  auto is_invalid() -> bool override;

  auto write(const Bytes& bytes) -> Result<void>;
  auto read_into(WritableBytes& bytes) -> Result<void>;
  auto flush_handle() const -> void;

  template<typename T>
  auto operator<<(const T&) -> IODevice&;

  template<typename T>
  auto operator>>(T& val) -> IODevice&;

  static auto from_stdout() -> IODevice;
  static auto from_stderr() -> IODevice;
  static auto from_stdin()  -> IODevice;
  static auto from(ValueType vt, uint8_t perms = Read | Write) -> IODevice;
  static auto create_pipe() -> Result<std::array<IODevice, 2>>;

  IODevice(const IODevice& other);
  IODevice& operator=(const IODevice& other);

  IODevice(IODevice&& other) noexcept;
  IODevice& operator=(IODevice&& other) noexcept;

  IODevice();
 ~IODevice() override = default;

  uint8_t perms_ = NoAccess;
};

///////////////////////////////////////////////////////////////////////////////////

template<typename T>
auto IODevice::operator<<(const T& val) -> IODevice& {
  if constexpr(std::ranges::contiguous_range<T>) {
    auto bytes = as_bytes(val);
    write(bytes);
  } else if constexpr (std::is_trivially_constructible_v<T>){
    auto copy = as_bytecopy(val);
    write(copy.bytes());
  } else {
    static_assert(FalseType<T>::value, "Invalid type.");
  }

  return *this;
}

template<typename T>
auto IODevice::operator>>(T& val) -> IODevice & {
  static_assert(std::ranges::contiguous_range<T>);
  auto bytes = as_writable_bytes(val);
  read_into(bytes);
  return *this;
}

///////////////////////////////////////////////////////////////////////////////////

inline IODevice::IODevice(const IODevice& other) {
  value_ = other.value_;
  perms_ = other.perms_;
}

inline IODevice::IODevice(IODevice&& other) noexcept {
  value_ = other.value_;
  perms_ = other.perms_;
  other.invalidate();
}

inline IODevice& IODevice::operator=(const IODevice& other) {
  if(this != &other) {
    value_ = other.value_;
    perms_ = other.perms_;
  }
  return *this;
}

inline IODevice& IODevice::operator=(IODevice&& other) noexcept {
  if(this != &other) {
    value_ = other.value_;
    perms_ = other.perms_;
    other.invalidate();
  }
  return *this;
}

///////////////////////////////////////////////////////////////////////////////////
#if defined(N19_POSIX)
FORCEINLINE_ auto IODevice::invalidate() -> void {
  value_ = -1;
  perms_ = IODevice::NoAccess;
}

inline IODevice::IODevice() {
  value_ = -1;
  perms_ = IODevice::NoAccess;
}

FORCEINLINE_ auto IODevice::close() -> void {
  ::close(value_);
  invalidate();
}

FORCEINLINE_ auto IODevice::is_invalid() -> bool {
  return value_ == -1;
}

FORCEINLINE_ auto IODevice::flush_handle() const -> void {
  ::fsync(value_);
}

///////////////////////////////////////////////////////////////////////////////////
#else // IF WINDOWS
FORCEINLINE_ auto IODevice::invalidate() -> void {
  value_ = (::HANDLE)nullptr;
  perms_ = IODevice::NoAccess;
}

inline IODevice::IODevice() {
  value_ = (::HANDLE)nullptr;
  perms_ = IODevice::NoAccess;
}

FORCEINLINE_ auto IODevice::close() -> void {
  ::CancelIoEx(value_, nullptr);
  ::CloseHandle(value_);
  invalidate();
}

FORCEINLINE_ auto IODevice::is_invalid() -> bool {
  return value_ == (::HANDLE)nullptr;
}

FORCEINLINE_ auto IODevice::flush_handle() const -> void {
  ::FlushFileBuffers(value_);
}

#endif //IF defined(N19_POSIX)
END_NAMESPACE(n19::sys);
