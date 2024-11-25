/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#include <Native/IODevice.h>
#include <Native/LastError.h>

#if defined(N19_POSIX)
auto n19::native::IODevice::read_into(
  ByteView &bytes ) -> IODevice&
{
  return *this; // TODO
}

auto n19::native::IODevice::write(
  const ByteView& bytes ) -> IODevice&
{
  return *this; // TODO
}

auto n19::native::IODevice::create_pipe()
-> Result<std::array<IODevice, 2>>
{
  int pipefds[ 2 ] = { 0 };
  std::array<IODevice, 2> arr = { };

  if(::pipe(pipefds) == -1) {
    return make_error(ErrC::Native, "{}", last_error());
  }

  arr[ 0 ].value_ = pipefds[ 0 ];
  arr[ 1 ].value_ = pipefds[ 1 ];
  return make_result<decltype(arr)>(arr);
}

auto n19::native::IODevice::from_stderr()
-> Result<IODevice>
{
  IODevice device;
  device.value_ = STDERR_FILENO;
  return make_result<IODevice>(device);
}

auto n19::native::IODevice::from_stdout()
-> Result<IODevice>
{
  IODevice device;
  device.value_ = STDOUT_FILENO;
  return make_result<IODevice>(device);
}

auto n19::native::IODevice::from_stdin()
-> Result<IODevice>
{
  IODevice device;
  device.value_ = STDIN_FILENO;
  return make_result<IODevice>(device);
}

#else

#endif