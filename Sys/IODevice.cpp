/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#include <Sys/IODevice.hpp>
#include <Sys/LastError.hpp>

BEGIN_NAMESPACE(n19::sys);
#if defined(N19_POSIX)

auto IODevice::write(const Bytes& bytes) const -> Result<void> {
  ASSERT(!bytes.empty());
  pollfd fds[1] = { 0 };
  fds[0].fd     = value_;
  fds[0].events = POLLOUT;

  if(::poll(fds, 1, -1) == -1 || !(fds[0].revents & POLLOUT)) {
    return make_error(ErrC::Native, last_error());
  } if(::write(value_, bytes.data(), bytes.size_bytes()) == -1) {
    return make_error(ErrC::Native, last_error());
  }

  return make_result<void>();
}

auto IODevice::read_into(WritableBytes& bytes) const -> Result<void> {
  ASSERT(!bytes.empty());
  pollfd fds[1] = { 0 };
  fds[0].fd     = value_;
  fds[0].events = POLLIN;

  if(::poll(fds, 1, -1) == -1 || !(fds[0].revents & POLLIN)) {
    return make_error(ErrC::Native, last_error());
  } if(::read(value_, bytes.data(), bytes.size_bytes()) == -1) {
    return make_error(ErrC::Native, last_error());
  }

  return make_result<void>();
}

auto IODevice::create_pipe() -> Result<std::array<IODevice, 2>> {
  int pipefds[ 2 ] = { 0 };
  std::array<IODevice, 2> arr = { };
  if(::pipe(pipefds) == -1) {
    return make_error(ErrC::Native, last_error());
  }

  arr[ 0 ].value_ = pipefds[ 0 ];
  arr[ 1 ].value_ = pipefds[ 1 ];
  arr[ 0 ].perms_ = Read;
  arr[ 1 ].perms_ = Write;

  return make_result<decltype(arr)>(arr);
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

#else // IF WINDOWS

auto IODevice::write(
  const Bytes &bytes ) const -> Result<void>
{
  ASSERT(!bytes.empty());
  if(!WriteFile(
    value_,
    (void*)bytes.data(),
    (DWORD)bytes.size_bytes(),
    nullptr,
    nullptr
  )) {
    return make_error(ErrC::Native, last_error());
  }

  return make_result<void>();
}

auto IODevice::read_into(
  WritableBytes& bytes ) const -> Result<void>
{
  ASSERT(!bytes.empty());
  if(!ReadFile(
    value_,
    (void*)bytes.data(),
    (DWORD)bytes.size_bytes(),
    nullptr,
    nullptr
  )) {
    return make_error(ErrC::Native, last_error());
  }

  return make_result<void>();
}

auto IODevice::create_pipe() -> Result<std::array<IODevice, 2>> {
  SECURITY_ATTRIBUTES sa  = { 0 };
  sa.nLength              = sizeof(sa);
  sa.lpSecurityDescriptor = nullptr;
  sa.bInheritHandle       = TRUE;

  std::array<IODevice, 2> arr{};
  if(!CreatePipe(&arr[0].value_, &arr[1].value_, &sa, 0)) {
    return make_error(ErrC::Native, last_error());
  }

  arr[ 0 ].perms_ = Read;
  arr[ 1 ].perms_ = Write;
  return arr;
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

#endif
END_NAMESPACE(n19::sys);