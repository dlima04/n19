/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Core/Console.hpp>
BEGIN_NAMESPACE(n19);

namespace detail_ {
  /// If false, all uses of ASCII colours are ignored.
  constinit bool allow_con_colours_ = true;
}

#if defined(N19_WIN32)
auto win32_are_vsequences_enabled() -> bool {
  DWORD mode_stdout = 0, mode_stderr = 0;
  DWORD flag_value  = ENABLE_VIRTUAL_TERMINAL_PROCESSING;

  ::GetConsoleMode(::GetStdHandle(STD_OUTPUT_HANDLE), &mode_stdout);
  ::GetConsoleMode(::GetStdHandle(STD_ERROR_HANDLE), &mode_stderr);
  return (mode_stdout & flag_value) && (mode_stderr & flag_value);
}

auto win32_enable_vsequences() -> void {
  HANDLE h_stdout    = ::GetStdHandle(STD_OUTPUT_HANDLE);
  HANDLE h_stderr    = ::GetStdHandle(STD_ERROR_HANDLE);
  DWORD mode_stdout  = 0;
  DWORD mode_stderr  = 0;

  ::GetConsoleMode(h_stdout, &mode_stdout);
  ::GetConsoleMode(h_stderr, &mode_stderr);
  ::SetConsoleMode(h_stdout, mode_stdout | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
  ::SetConsoleMode(h_stderr, mode_stderr | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

auto win32_init_console() -> void {
  ::SetConsoleOutputCP(CP_UTF8); /// change cp for stdout.
  ::SetConsoleCP(CP_UTF8);       /// change cp for stdin.
  win32_enable_vsequences();     /// enable ASCII escapes.
}

#endif // #if defined(N19_WIN32)
END_NAMESPACE(n19);
