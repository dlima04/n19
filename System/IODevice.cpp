/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <System/IODevice.hpp>
#include <System/Error.hpp>
#include <Core/Try.hpp>
#include <utility>
BEGIN_NAMESPACE(n19::sys);

#if defined(N19_POSIX)
auto IODevice::write(const Bytes& bytes) -> Result<void> {
  ASSERT(!bytes.empty());
  if(!is_invalid() && ::write(value_, bytes.data(), bytes.size_bytes()) == -1) {
    return Error(ErrC::Native, last_error());
  }

  return Result<void>::create();
}

auto IODevice::read_into(WritableBytes& bytes) -> Result<void> {
  ASSERT(!bytes.empty());
  if(!is_invalid() && ::read(value_, bytes.data(), bytes.size_bytes()) == -1) {
    return Error(ErrC::Native, last_error());
  }

  return Result<void>::create();
}

auto IODevice::create_pipe() -> Result<std::array<IODevice, 2>> {
  int pipefds[ 2 ]{};
  std::array<IODevice, 2> arr{};
  if(::pipe(pipefds) == -1) {
    return Error(ErrC::Native, last_error());
  }

  arr[ 0 ].value_ = pipefds[ 0 ];
  arr[ 1 ].value_ = pipefds[ 1 ];
  arr[ 0 ].perms_ = Read;
  arr[ 1 ].perms_ = Write;

  return Result<decltype(arr)>{ std::move(arr) };
}

auto IODevice::from_stderr() -> IODevice {
  IODevice device;
  device.value_ = STDERR_FILENO;
  device.perms_ = Write;
  return device;
}

auto IODevice::from_stdout() -> IODevice {
  IODevice device;
  device.value_ = STDOUT_FILENO;
  device.perms_ = Write;
  return device;
}

auto IODevice::from_stdin() -> IODevice {
  IODevice device;
  device.value_ = STDIN_FILENO;
  device.perms_ = Read;
  return device;
}

auto IODevice::from(ValueType vt, uint8_t perms) -> IODevice {
  IODevice device;
  device.value_ = vt;
  device.perms_ = perms;
  return device;
}

#else // IF WINDOWS

auto IODevice::write(const Bytes &bytes) -> Result<void> {
  ASSERT(!bytes.empty());
  if(!::WriteFile(             ///
    value_,                    /// The output file handle.
    (void*)bytes.data(),       /// Output buffer.
    (DWORD)bytes.size_bytes(), /// Size of the input buffer.
    nullptr,                   /// Number of bytes written (optional)
    nullptr                    /// OVERLAPPED struct (optional)
  )) {
    return Error(ErrC::Native, last_error());
  }

  return Result<void>::create();
}

auto IODevice::read_into(WritableBytes& bytes) -> Result<void> {
  ASSERT(!bytes.empty());
  if(!::ReadFile(              ///
    value_,                    /// The input file handle.
    (void*)bytes.data(),       /// The input buffer.
    (DWORD)bytes.size_bytes(), /// Size of our buffer.
    nullptr,                   /// Number of bytes read from the file (optional)
    nullptr                    /// OVERLAPPED struct (optional)
  )) {
    return Error(ErrC::Native, last_error());
  }

  return Result<void>::create();
}

auto IODevice::create_pipe() -> Result<std::array<IODevice, 2>> {
  SECURITY_ATTRIBUTES sa{};
  sa.nLength              = sizeof(sa);
  sa.lpSecurityDescriptor = nullptr;
  sa.bInheritHandle       = TRUE;

  std::array<IODevice, 2> arr{};
  if(!::CreatePipe(&arr[0].value_, &arr[1].value_, &sa, 0)) {
    return Error(ErrC::Native, last_error());
  }

  arr[ 0 ].perms_ = Read;
  arr[ 1 ].perms_ = Write;
  return Result<decltype(arr)>{ std::move(arr) };
}

auto IODevice::from_stderr() -> IODevice {
  IODevice device;
  device.value_ = ::GetStdHandle(STD_ERROR_HANDLE);
  device.perms_ = Write;
  return device;
}

auto IODevice::from_stdout() -> IODevice {
  IODevice device;
  device.value_ = ::GetStdHandle(STD_OUTPUT_HANDLE);
  device.perms_ = Write;
  return device;
}

auto IODevice::from_stdin() -> IODevice {
  IODevice device;
  device.value_ = ::GetStdHandle(STD_INPUT_HANDLE);
  device.perms_ = Read;
  return device;
}

auto IODevice::from(ValueType vt, uint8_t perms) -> IODevice {
  IODevice device;
  device.value_ = vt;
  device.perms_ = perms;
  return device;
}

#endif
END_NAMESPACE(n19::sys);
