/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef FMT_HPP
#define FMT_HPP
#include <string>
#include <format>
BEGIN_NAMESPACE(n19);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A small header for runtime formatting utility.
// Note, that formatting of this kind using the STL should be done
// very sparingly. It does not support UTF8 encoded strings,
// i.e. std::u8string, yet. calling outs() is always the preferred method
// for printing to the console.

template<typename ... Args>
auto fmt(const std::format_string<Args...> fmt, Args&&... args) -> std::string {
  std::string output;
  try {
    output = std::vformat(fmt.get(), std::make_format_args(args...));
  } catch(const std::format_error& e) {
    output = std::string("!! std::vformat: ") + e.what();
  } catch(...) {
    output = "!! format error";
  }

  return output;
}

template<typename ... Args>
auto fmt(const std::wformat_string<Args...> fmt, Args&&... args) -> std::wstring {
  std::wstring output;
  try {
    output = std::vformat(fmt.get(), std::make_wformat_args(args...));
  } catch(...) {
    output = L"!! format error";
  }

  return output;
}

END_NAMESPACE(n19);
#endif //FMT_HPP
