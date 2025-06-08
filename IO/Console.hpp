/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef CORE_CONIO_HPP
#define CORE_CONIO_HPP
#include <IO/Stream.hpp>
#include <IO/Fmt.hpp>
BEGIN_NAMESPACE(n19);

namespace detail_ {
  constinit extern bool allow_con_colours_;
}

///
/// Windows specific initialization.
#if defined(N19_WIN32)
auto win32_init_console()            -> void;
auto win32_are_vsequences_enabled()  -> bool;
auto win32_enable_vsequences()       -> void;
#endif

///
/// Ascii colour values
enum class Con : uint16_t {
  Reset      = 0,
  Bold       = 1,
  Underline  = 4,
  GreenFG    = 32,
  YellowFG   = 33,
  BlueFG     = 34,
  MagentaFG  = 35,
  CyanFG     = 36,
  WhiteFG    = 37,
  DefaultFG  = 39,
  DefaultBG  = 49,
  RedFG      = 91,
};

///
/// Global stream objects
inline auto outs() -> OStream& {
  static auto outs_ = BufferedOStream<>::from_stdout();
  return outs_;
}

inline auto errs() -> OStream& {
  static auto errs_ = BufferedOStream<>::from_stderr();
  return errs_;
}

inline auto nulls() -> OStream& {
  static auto nulls_ = NullOStream();
  return nulls_;
}

inline auto ins() -> IStream& {
  static auto ins_ = IStream::from_stdin();
  return ins_;
}

inline auto operator<<(OStream& stream, const Con code) -> OStream& {
  if(detail_::allow_con_colours_) {
    stream << "\x1b[" << static_cast<uint16_t>(code) << 'm';
  }
  return stream;
}

template<AreAll<Con> ...Args>
inline auto manip_string(Args... values) -> std::string {
  std::string buff;
  if(detail_::allow_con_colours_) {
    ((buff += fmt( "\x1b[{}m", static_cast<uint16_t>(values) )), ...);
  }
  return buff;
}

END_NAMESPACE(n19);
#endif //CONIO_HPP
