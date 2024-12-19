/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef CORE_CONIO_HPP
#define CORE_CONIO_HPP
#include <Core/Stream.hpp>
#include <Core/Fmt.hpp>
BEGIN_NAMESPACE(n19);

#if defined(N19_WIN32)
auto win32_init_console()            -> void;
auto win32_are_vsequences_enabled()  -> bool;
auto win32_enable_vsequences()       -> void;
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Each member of the Con enumeration is a value that
// corresponds to an ASCII escape code, to be used in colour formatting.

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// inlined header functions

inline auto outs() -> OStream& {
  static auto _outs = OStream::from_stdout();
  return _outs;
}

inline auto errs() -> OStream& {
  static auto _errs = OStream::from_stderr();
  return _errs;
}

inline auto nulls() -> OStream& {
  static auto _nulls = NullOStream();
  return _nulls;
}

inline auto ins() -> IStream& {
  static auto _ins = IStream::from_stdin();
  return _ins;
}

inline auto operator<<(OStream& stream, const Con code) -> OStream& {
  stream << "\x1b[" << static_cast<uint16_t>(code) << 'm';
  return stream;
}

template<AreAll<Con> ...Args>
inline auto set_console(Args... values) -> void {
  ((outs() << values), ...);
}

template<AreAll<Con> ...Args>
inline auto manip_string(Args... values) -> std::string {
  std::string buff;
  ((buff += fmt( "\x1b[{}m", static_cast<uint16_t>(values) )), ...);
  return buff;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

END_NAMESPACE(n19);
#endif //CONIO_HPP
