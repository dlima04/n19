/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <Core/Common.hpp>
#include <string>
#include <format>
BEGIN_NAMESPACE(n19);

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
