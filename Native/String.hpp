/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef NATIVE_STRING_HPP
#define NATIVE_STRING_HPP
#include <string>
#include <string_view>

#if defined(N19_WIN32)
  static_assert(sizeof(wchar_t) == 2);
#endif

namespace n19::native {
#if defined(N19_WIN32)
#define _nstr( STR ) L##STR
#define _nchr( CHR ) L##CHR
  using String       = std::wstring;
  using StringView   = std::wstring_view;
  using Char         = wchar_t;
#else
#define _nstr( STR ) (const char*)u8##STR
#define _nchr( CHR ) (const char)u8##CHR
  using String       = std::string;
  using StringView   = std::string_view;
  using Char         = char;
#endif
}

#endif //NATIVE_STRING_HPP
