/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <string>
#include <string_view>

#if defined(N19_WIN32)
  static_assert(sizeof(wchar_t) == 2);
#endif

namespace n19::sys {
#if defined(N19_WIN32)
#define _nstr(STR) L##STR
#define _nchr(CHR) L##CHR
  using String     = std::wstring;
  using StringView = std::wstring_view;
  using Char       = wchar_t;
#else
#define _nstr(STR) STR
#define _nchr(CHR) CHR
  using String     = std::string;
  using StringView = std::string_view;
  using Char       = char;
#endif
}