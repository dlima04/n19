/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" License.
* See the LICENSE file located at this project's root directory for
* more information.
*/

#ifndef FMT_H
#define FMT_H
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

#endif //FMT_H
