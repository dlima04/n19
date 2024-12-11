/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef CONIO_HPP
#define CONIO_HPP
#include <Sys/IODevice.hpp>
#include <Core/ClassTraits.hpp>
#include <Core/Fmt.hpp>
#include <Core/Platform.hpp>
#include <Core/Concepts.hpp>
#include <string>
#include <cstdint>
#include <print>

#if defined(N19_WIN32)
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#else //POSIX
#  include <locale.h>
#endif

#if defined(N19_LARGE_OSTREAM_BUFFERS)
#  define N19_COSTREAM_BUFFER_SIZE 8192
#  define N19_CISTREAM_BUFFER_SIZE 8192
#else
#  define N19_COSTREAM_BUFFER_SIZE 4096
#  define N19_CISTREAM_BUFFER_SIZE 4096
#endif

BEGIN_NAMESPACE(n19);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

#if defined(N19_WIN32)
  auto win32_init_console()            -> void;
  auto win32_are_vsequences_enabled()  -> bool;
  auto win32_enable_vsequences()       -> void;
#endif

class COStream { // TODO: finish
public:
  static auto from_stdout() -> Result<COStream>;
  static auto from_stderr() -> Result<COStream>;

protected:
  COStream() = default;
private:
  sys::IODevice fd_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Utility functions

template<AreAll<Con> ...Args>
N19_FORCEINLINE auto set_console(Args... values) -> void {
  (std::print("\x1b[{}m", static_cast<uint16_t>(values)), ...);
}

template<AreAll<Con> ...Args>
N19_FORCEINLINE auto manip_string(Args... values) -> std::string {
  std::string buff;
  ((buff += fmt( "\x1b[{}m", static_cast<uint16_t>(values) )), ...);
  return buff;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

END_NAMESPACE(n19);
#endif //CONIO_HPP
