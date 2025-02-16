/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef NATIVE_IODEVICE_HPP
#define NATIVE_IODEVICE_HPP
#include <Sys/Handle.hpp>
#include <Core/Bytes.hpp>
#include <Core/Result.hpp>
#include <array>
#include <cstdint>
#include <span>

#if defined(N19_WIN32)
#  include <windows.h>
#else // POSIX
#  include <unistd.h>
#  include <poll.h>
#endif

BEGIN_NAMESPACE(n19::sys);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class IODevice final
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

  // public methods for reading to,
  // writing to, and flushing the device.
  auto write(const Bytes& bytes) const -> Result<void>;
  auto read_into(WritableBytes& bytes) const -> Result<void>;
  auto flush_handle() const -> void;

  // Some operator overloads to simplify
  // reading/writing to the IODevice.
  template<typename T> auto operator<<(const T& val) -> IODevice&;
  template<typename T> auto operator>>(T& val)       -> IODevice&;

  // Static methods / factories.
  // These can be called to gain access
  // to common native I/O handles.
  static auto from_stdout() -> IODevice;
  static auto from_stderr() -> IODevice;
  static auto from_stdin()  -> IODevice;
  static auto create_pipe() -> Result<std::array<IODevice, 2>>;

  IODevice() = default;
 ~IODevice() override = default;

  uint8_t perms_ = NoAccess;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
auto IODevice::operator<<(const T& val) -> IODevice& {
  if constexpr(std::ranges::contiguous_range<T>) {
    auto bytes = as_bytes(val);
    write(bytes);
  } else if constexpr (std::is_trivially_constructible_v<T>){
    auto copy = as_bytecopy(val);
    write(copy.bytes());
  } else {
    static_assert(
    "IODevice::operator<< must be called with "
    "a type easily convertible to n19::Bytes.");
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

#if defined(N19_POSIX)
inline auto IODevice::invalidate() -> void {
  value_ = -1;
}

inline auto IODevice::close() -> void {
  ::close(value_);
  invalidate();
}

N19_FORCEINLINE auto IODevice::is_invalid() -> bool {
  return value_ == -1;
}

N19_FORCEINLINE auto IODevice::flush_handle() const -> void {
  ::fsync(value_); // sync the file descriptor.
}

#else // IF WINDOWS
inline auto IODevice::invalidate() -> void {
  value_ = (::HANDLE)nullptr;
}

inline auto IODevice::close() -> void {
  ::CancelIoEx(value_, nullptr);
  ::CloseHandle(value_);
  invalidate();
}

N19_FORCEINLINE auto IODevice::is_invalid() -> bool {
  return value_ == (::HANDLE)nullptr;
}

N19_FORCEINLINE auto IODevice::flush_handle() const -> void {
  ::FlushFileBuffers(value_); // Flush Win32 file buff.
}

#endif //IF defined(N19_POSIX)
END_NAMESPACE(n19::sys);
#endif //NATIVE_IODEVICE_HPP
