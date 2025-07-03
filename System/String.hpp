/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <Core/Common.hpp>
#include <Core/Platform.hpp>
#include <string>
#include <string_view>
BEGIN_NAMESPACE(n19::sys);

#ifdef N19_WIN32
  static_assert(sizeof(wchar_t) == 2);
#endif

#ifdef N19_WIN32
#  define _nstr(STR) L##STR
#  define _nchr(CHR) L##CHR
   using String     = std::wstring;
   using StringView = std::wstring_view;
   using Char       = wchar_t;
#else
#  define _nstr(STR) STR
#  define _nchr(CHR) CHR
   using String     = std::string;
   using StringView = std::string_view;
   using Char       = char;
#endif

#ifdef N19_WIN32
template<typename T>
auto to_native_string(T val) -> String {
  return std::to_wstring(val);
}

#else //POSIX
template<typename T>
auto to_native_string(T val) -> String {
  return std::to_string(val);
}

#endif
END_NAMESPACE(n19::sys);