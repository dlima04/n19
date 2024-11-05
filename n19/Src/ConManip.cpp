#include <ConManip.h>
#include <print>
#include <string>

#if defined(N19_WIN32)
#include <Windows.h>

auto n19::are_vsequences_enabled() -> bool {
  DWORD mode_stdout = 0;
  DWORD mode_stderr = 0;
  GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &mode_stdout);
  GetConsoleMode(GetStdHandle(STD_ERROR_HANDLE), &mode_stderr);
  return (mode_stdout & ENABLE_VIRTUAL_TERMINAL_PROCESSING)
      && (mode_stderr & ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

auto n19::enable_vsequences() -> void {
  HANDLE h_stdout    = GetStdHandle(STD_OUTPUT_HANDLE);
  HANDLE h_stderr    = GetStdHandle(STD_ERROR_HANDLE);
  DWORD mode_stdout  = 0;
  DWORD mode_stderr  = 0;
  if(h_stdout == INVALID_HANDLE_VALUE || h_stderr == INVALID_HANDLE_VALUE) {
    return;
  }

  GetConsoleMode(h_stdout, &mode_stdout);
  GetConsoleMode(h_stderr, &mode_stderr);
  SetConsoleMode(h_stdout, mode_stdout | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
  SetConsoleMode(h_stderr, mode_stderr | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

auto n19::maybe_enable_vsequences() -> void {
  if(!are_vsequences_enabled())
    enable_vsequences();
}

#endif // #if defined(N19_WIN32)

auto n19::reset_console() -> void {
#if defined(N19_WIN32)
  maybe_enable_vsequences();
#endif
  std::print("\x1b[m");
}

auto n19::set_console(const ConStyle cs) -> void {
#if defined(N19_WIN32)
  maybe_enable_vsequences();
#endif
  std::print("\x1b[{}m", std::to_string((uint16_t)cs));
}

auto n19::set_console(const ConFg fg) -> void {
#if defined(N19_WIN32)
  maybe_enable_vsequences();
#endif
  std::print("\x1b[{}m", std::to_string((uint16_t)fg));
}
