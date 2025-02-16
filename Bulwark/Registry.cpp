/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Bulwark/Registry.hpp>
#include <Bulwark/Reporting.hpp>
#include <Bulwark/BulwarkContext.hpp>
#include <Core/Panic.hpp>
#include <algorithm>
BEGIN_NAMESPACE(n19::test);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global registry object

constinit Registry g_registry;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Begin methods

auto Registry::add_case(
  const Case::FuncType_& case_func,
  const Case::NameType_& case_name,
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

auto Registry::run_all(OStream &stream) -> void {
  for(Suite& suite : suites_) {
    if(Context::the().should_skip(suite.name_)) {
      g_total_skipped += suite.cases_.size();
      continue;
    } if(!Context::the().suites_to_run_.empty() && !Context::the().should_run(suite.name_)) {
      g_total_skipped += suite.cases_.size();
      continue;
    }

    report(suite, stream);         /// Report current suite if not skipped.
    suite.run_all(stream);         ///
  }

  const size_t total = g_total_exc /// Add up total cases.
    + g_total_failed               ///
    + g_total_passed
    + g_total_skipped;

  stream << "\nRan " << suites_.size() << " suites.\n";
  stream << total << " cases total,\n";
  stream << "  "  << g_total_passed  << " passed,\n";
  stream << "  "  << g_total_failed  << " failed,\n";
  stream << "  "  << g_total_exc     << " interrupted by exceptions,\n";
  stream << "  "  << g_total_skipped << " skipped.\n";
}

auto Registry::find(const sys::StringView& sv) -> Suite* {
  for(Suite& suite : suites_) {
    if(suite.name_ == sv) return &suite;
  }
  return nullptr;
}

END_NAMESPACE(n19::test);
