/*
* Copyright (c) 2025 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#include <Bulwark/Registry.hpp>
#include <Core/Panic.hpp>
#include <algorithm>
BEGIN_NAMESPACE(n19::test);

constinit size_t g_total_passed = 0;
constinit size_t g_total_failed = 0;
constinit Registry g_registry;

auto Registry::add_case(
  const Case::__FuncType& case_func,
  const Case::__NameType& case_name,
  const std::string_view& suite_name ) noexcept -> bool
{
  ASSERT(case_func);
  ASSERT(!case_name.empty());
  ASSERT(!suite_name.empty());

  auto exists = std::ranges::find_if(suites_, [&](const Suite& s) {
    return s.name_ == suite_name;
  });

  if(exists != suites_.end()) {
    exists->cases_.emplace_back(case_func, case_name);
  } else {
    auto& new_suite = suites_.emplace_back();
    new_suite.name_ = suite_name;
    new_suite.cases_.emplace_back(case_func, case_name);
  }

  return true;
}

END_NAMESPACE(n19::test);