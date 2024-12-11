/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef SYS_STREAM_HPP
#define SYS_STREAM_HPP
#include <iostream>

#if defined(N19_WIN32)
  static_assert(sizeof(wchar_t) == 2);
#endif

namespace n19::sys {
#if defined(N19_WIN32)
  using OStream = decltype(std::wcout);
  using IStream = decltype(std::wcin);
#else
  using OStream = decltype(std::cout);
  using IStream = decltype(std::cin);
#endif
  auto outs() -> OStream&;
  auto errs() -> OStream&;
  auto ins()  -> IStream&;
}

inline auto n19::sys::outs() -> OStream& {
#if defined(N19_WIN32)
  return std::wcout;
#else
  return std::cout;
#endif
}

inline auto n19::sys::errs() -> OStream& {
#if defined(N19_WIN32)
  return std::wcerr;
#else
  return std::cerr;
#endif
}

inline auto n19::sys::ins() -> IStream& {
#if defined(N19_WIN32)
  return std::wcin;
#else
  return std::cin;
#endif
}

#endif //SYS_STREAM_HPP
