/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Bulwark/Suite.hpp>
#include <Bulwark/Reporting.hpp>
#include <Bulwark/BulwarkContext.hpp>
BEGIN_NAMESPACE(n19::test);

auto Suite::run_all(OStream& s) -> void {
  const auto verbose  = Context::the().flags_ & Context::Verbose;
  const auto stopfail = Context::the().flags_ & Context::StopFail;

  for(Case& case_ : cases_) { /// Iterate through all cases.
    ExecutionContext ctx{s};  /// Create execution context.
    if(verbose) {
      outs() << "Begin Case " << case_.name_ << ":\n";
    }
                              ///
    case_(ctx);               /// Report the test case result.
    report(case_, ctx.result, s);

    switch(ctx.result.val_) {
    case Result::Failed:    ++g_total_failed;  break;
    case Result::Passed:    ++g_total_passed;  break;
    case Result::Exception: ++g_total_exc;     break;
    case Result::Skipped:   ++g_total_skipped; break;
    default: UNREACHABLE_ASSERTION;
    }

    if(stopfail && ctx.result.val_ != Result::Passed)
      break;
  }

  s << Flush;
}

END_NAMESPACE(n19::test);
