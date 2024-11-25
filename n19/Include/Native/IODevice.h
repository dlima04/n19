/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef NATIVE_IODEVICE_H
#define NATIVE_IODEVICE_H
#include <Native/Handle.h>
#include <Native/String.h>
#include <Core/Bytes.h>
#include <Core/Result.h>
#include <array>
#include <cstdint>
#include <span>
#include <cstddef>

#if defined(N19_WIN32)
  #include <windows.h>
#else // POSIX
  #include <unistd.h>
#endif

namespace n19::native {
  class IODevice;
}

class n19::native::IODevice final
  #if defined(N19_WIN32)
  : public Handle<::HANDLE>
  #else
  : public Handle<int>
  #endif
{
public:
  enum Permissions : uint8_t {
    NoAccess = 0,
    Read     = 1,
    Write    = 1 << 1,
    Execute  = 1 << 2,
  };

  // Overridden base class members.
  // Usually we can implement these in-header,
  // since they're only a couple lines.
  auto close()      -> void override;
  auto invalidate() -> void override;
  auto is_invalid() -> bool override;

  // public methods for reading to and
  // writing to the device.
  auto write(const ByteView& bytes) -> IODevice&;
  auto read_into(ByteView& bytes) -> IODevice&;

  // Some operator overloads to simplify
  // reading/writing to the IODevice.
  template<typename T> auto operator<<(const T& val) -> IODevice&;
  template<typename T> auto operator>>(T& val) -> IODevice&;

  // Static methods / factories.
  // These can be called to gain access
  // to common native I/O handles.
  static auto from_stdout() -> Result<IODevice>;
  static auto from_stderr() -> Result<IODevice>;
  static auto from_stdin()  -> Result<IODevice>;
  static auto create_pipe() -> Result<std::array<IODevice, 2>>;
  ~IODevice() override = default;
private:
  uint8_t perms_ = NoAccess;
  IODevice() = default;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
auto n19::native::IODevice::operator<<(const T& val) -> IODevice& {
  if constexpr(std::constructible_from<std::span<T>, T>) {
    auto bytes = n19::as_bytes(val);
    write(bytes);
  } else if constexpr (std::is_trivially_constructible_v<T>){
    auto bytes = as_scalar_bytecopy(val);
    write(bytes);
  } else {
    static_assert(
    "IODevice::operator<< must be called with "
    "a type easily convertible to n19::ByteView.");
  }

  return *this;
}

template<typename T>
auto n19::native::IODevice::operator>>(T& val) -> IODevice & {
  static_assert(std::constructible_from<std::span<T>, T>);
  auto bv = n19::as_bytes(val);
  return read_into(bv);
}

#if defined(N19_POSIX)
inline auto n19::native::IODevice::invalidate() -> void {
  value_ = -1;
}

inline auto n19::native::IODevice::close() -> void {
  ::close(value_);
  invalidate();
}

inline auto n19::native::IODevice::is_invalid() -> bool {
  return value_ == -1;
}

#else // IF WINDOWS
inline auto n19::native::IODevice::invalidate() -> void {
  value_ = (::HANDLE)nullptr;
}

inline auto n19::native::IODevice::close() -> void {
  ::CloseHandle(value_);
  invalidate();
}

inline auto n19::native::IODevice::is_invalid() -> bool {
  return value_ == (::HANDLE)nullptr;
}

#endif //IF defined(N19_POSIX)
#endif //NATIVE_IODEVICE_H
