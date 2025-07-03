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

// Global registry object
constinit Registry g_registry;

auto Registry::add_case(
  const Case::FuncType_& case_func,
  const Case::NameType_& case_name,
  const sys::StringView& suite_name ) noexcept -> bool
{
  ASSERT(case_func);
  ASSERT(!case_name.empty());
  ASSERT(!suite_name.empty());
  
  if(suites_ == nullptr) {
    suites_ = std::make_unique<std::list<Suite>>();
  }

  auto exists = std::ranges::find_if(*suites_, [&](const Suite& s) {
    return s.name_ == suite_name;
  });

  if(exists != suites_->end()) {
    exists->cases_.emplace_back(case_func, case_name);
  } else {
    auto& new_suite = suites_->emplace_back();
    new_suite.name_ = suite_name;
    new_suite.cases_.emplace_back(case_func, case_name);
  }

  return true;
}

auto Registry::run_all(OStream &stream) -> void {
  if(suites_ == nullptr) {
    suites_ = std::make_unique<std::list<Suite>>();
  }

  for(Suite& suite : *suites_) {
    if(Context::the().should_skip(suite.name_)) {
      ++g_total_suites_skipped;
      g_total_cases_skipped += suite.cases_.size(); /// Skip test suite
      continue;
    } if(!Context::the().suites_to_run_.empty() && !Context::the().should_run(suite.name_)) {
      ++g_total_suites_skipped;
      g_total_cases_skipped += suite.cases_.size(); /// Skip test suite
      continue;
    }

    ++g_total_suites_ran;
    report(suite, stream);
    suite.run_all(stream);
  }

  /// Add up the total cases ran.
  const TallyType total = g_total_cases_exc
    + g_total_cases_failed
    + g_total_cases_passed
    + g_total_cases_skipped;

  /// If we're not tallying up test case results via IPC, just dump them to stdout.
  if(Context::the().shared_region_.is_invalid()) {
    stream
      << "\nRan "
      << g_total_suites_ran
      << " out of "
      << g_total_suites_ran + g_total_suites_skipped
      << " suites.\n"
      << total
      << " cases total,\n  "
      << g_total_cases_passed  << " passed,\n  "
      << g_total_cases_failed  << " failed,\n  "
      << g_total_cases_exc     << " interrupted by exceptions,\n  "
      << g_total_cases_skipped << " skipped.\n";
  }

  /// Otherwise we're in "IPC mode" and we need to report our results to the parent process.
  /// TODO: make these operations atomic in the future once we parallelize everything
  else {
    auto tally = static_cast<volatile TallyBox*>(Context::the().shared_region_.get());
    tally->total_cases_passed   += g_total_cases_passed;
    tally->total_cases_failed   += g_total_cases_failed;
    tally->total_cases_exc      += g_total_cases_exc;
    tally->total_cases_skipped  += g_total_cases_skipped;
    tally->total_suites_ran     += g_total_suites_ran;
    tally->total_suites_skipped += g_total_suites_skipped;
    tally->total_cases_ran      += total;
  }
}

auto Registry::find(const sys::StringView& sv) -> Suite* {
  for(Suite& suite : *suites_) {
    if(suite.name_ == sv) return &suite;
  }
  return nullptr;
}

END_NAMESPACE(n19::test);
