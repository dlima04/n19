/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#include <Core/ConIO.hpp>
#include <Core/Try.hpp>
#include <print>
#include <utility>
BEGIN_NAMESPACE(n19);

auto COStream::from_stderr() -> Result<COStream> {
  auto the_stream = COStream();
  auto stream_res = TRY(sys::IODevice::from_stderr());
  the_stream.fd_  = stream_res.value();
  return make_result<COStream>(std::move(the_stream));
}

auto COStream::from_stdout() -> Result<COStream> {
  auto the_stream = COStream();
  auto stream_res = TRY(sys::IODevice::from_stdout());
  the_stream.fd_  = stream_res.value();
  return make_result<COStream>(std::move(the_stream));
}

#if defined(N19_WIN32)
auto win32_are_vsequences_enabled() -> bool {
  DWORD mode_stdout = 0, mode_stderr = 0;
  DWORD flag_value  = ENABLE_VIRTUAL_TERMINAL_PROCESSING;

  GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &mode_stdout);
  GetConsoleMode(GetStdHandle(STD_ERROR_HANDLE), &mode_stderr);
  return (mode_stdout & flag_value) && (mode_stderr & flag_value);
}

auto win32_enable_vsequences() -> void {
  HANDLE h_stdout    = GetStdHandle(STD_OUTPUT_HANDLE);
  HANDLE h_stderr    = GetStdHandle(STD_ERROR_HANDLE);
  DWORD mode_stdout  = 0;
  DWORD mode_stderr  = 0;

  GetConsoleMode(h_stdout, &mode_stdout);
  GetConsoleMode(h_stderr, &mode_stderr);
  SetConsoleMode(h_stdout, mode_stdout | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
  SetConsoleMode(h_stderr, mode_stderr | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

auto win32_init_console() -> void {
  SetConsoleOutputCP(CP_UTF8);  // change cp for stdin.
  SetConsoleCP(CP_UTF8);        // change cp for stdout.
  win32_enable_vsequences();    // enable ASCII escapes.
}

#endif // #if defined(N19_WIN32)
END_NAMESPACE(n19);