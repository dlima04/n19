/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Bulwark/Case.hpp>
#include <Core/Panic.hpp>
BEGIN_NAMESPACE(n19::test);

auto Result::to_colour() const -> Con {
  switch(value) {
    case Failed:    return Con::RedFG;
    case Passed:    return Con::GreenFG;
    case Exception: return Con::YellowFG;
    case Skipped:   return Con::CyanFG;
    default: break;
  }

  UNREACHABLE_ASSERTION;
}

auto Result::to_string() const -> std::string {
  switch(value) {
    case Failed:    return "FAIL";
    case Passed:    return "PASS";
    case Exception: return "EXCT";
    case Skipped:   return "SKIP";
    default: break;
  }

  UNREACHABLE_ASSERTION;
}

END_NAMESPACE(n19::test);
