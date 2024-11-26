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

namespace n19 {
  template<typename ... Args>
  auto fmt(std::format_string<Args...> fmt, Args... args) -> std::string;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename ... Args>
auto n19::fmt(const std::format_string<Args...> fmt, Args... args)
-> std::string {
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

#endif //FMT_HPP
