#ifndef PANIC_H
#define PANIC_H
#include <ConManip.h>
#include <cstdlib>
#include <string>
#include <print>

#define PANIC(MSG) ::n19::panic_impl_(__FILE__, __LINE__, MSG)
#define FATAL(MSG) ::n19::exit_impl_(MSG)
#define UNREACHABLE PANIC("Default assertion - unreachable branch.")
#define ASSERT(COND) if(!(COND)) PANIC("Assertion \"" #COND "\" failed!")

namespace n19 {
  [[noreturn]] auto exit_impl_(const std::string &msg) -> void;
  [[noreturn]] auto panic_impl_(const std::string &file, int line, const std::string &msg) -> void;
}

[[noreturn]] inline auto
n19::panic_impl_(const std::string &file, const int line, const std::string &msg) -> void {
  // Red bold header
  set_console(ConFg::Red);
  set_console(ConStyle::Bold);
  std::println("PANIC :: {}", msg);

  // Location details
  reset_console();
  std::println("In file \"{}\" at line {}.", file, line);
  ::exit(EXIT_FAILURE);
}

[[noreturn]] inline auto
n19::exit_impl_(const std::string &msg) -> void {
  set_console(ConFg::Red);
  set_console(ConStyle::Bold);
  std::println("FATAL :: {}", msg);

  // Kill the program.
  ::exit(1);
}

#endif //PANIC_H
