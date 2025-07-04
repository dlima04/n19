/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <Core/Common.hpp>
#include <Core/Platform.hpp>
#include <Bulwark/Case.hpp>
#include <Bulwark/Suite.hpp>
#include <Core/Console.hpp>
#include <Core/ClassTraits.hpp>
#include <cstdint>
BEGIN_NAMESPACE(n19::test);

struct Diagnostic {
  N19_MAKE_COMPARABLE_MEMBER(Diagnostic, val_);
  enum Value_ : uint8_t {
    Warn   = 0x00,           /// Warning diagnostic.
    Info   = 0x01,           /// Info only.
    Fatal  = 0x02,           /// A fatal error, should end the test case.
    Debug  = 0x03,           /// Should only be displayed in debug mode.
  };                         ///

  Value_ val_ = Info;
  auto to_string() const -> std::string;
  auto to_colour() const -> Con;

  constexpr Diagnostic() = default;
  constexpr Diagnostic(const Value_ val) : val_(val) {}
};

using TallyType = uint32_t;
struct TallyBox {
  TallyType total_cases_passed;
  TallyType total_cases_failed;
  TallyType total_cases_exc;
  TallyType total_cases_skipped;
  TallyType total_suites_ran;
  TallyType total_suites_skipped;
  TallyType total_cases_ran;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////

auto diagnostic(             /// Display a diagnostic message.
  const std::string_view& m, /// Diagnostic message to be displayed.
  Diagnostic diag,           /// Diagonostic type.
  OStream& stream = outs(),  /// Output stream to send the diagnostic to
  size_t indent = 1          /// ...
) -> void;                   ///

auto report(                 /// Report the result of a test case when not in verbose mode.
  const Case& c,             /// Case to report, will display as 'Case......[PASS/FAIL]'
  Result r,                  /// The status of this test case.
  OStream& stream = outs(),  /// The output stream to display this to.
  size_t indent = 0          /// Indentation level
) -> void;                   ///

auto report(                 /// Report an indivual "REQUIRE" expression
  const std::string_view& e, /// Expression to report
  Result r,                  /// Result of the expression (passed/failed/etc)
  OStream& stream = outs(),  /// ...
  size_t indent = 1          /// ...
) -> void;                   ///

auto report(                 /// For reporting that a section is being entered (verbose mode only)
  const std::string_view& s, /// The name of the section.
  OStream& stream = outs(),  /// ...
  size_t indent = 1          /// ...
) -> void;                   ///

auto report(                 /// Report that a test suite is about to be ran.
  const Suite& suite,        /// The suite.
  OStream& stream = outs()   /// ...
) -> void;                   ///

/////////////////////////////////////////////////////////////////////////////////////////////////////

constinit extern TallyType g_total_cases_passed;
constinit extern TallyType g_total_cases_failed;
constinit extern TallyType g_total_cases_exc;
constinit extern TallyType g_total_cases_skipped;
constinit extern TallyType g_total_suites_skipped;
constinit extern TallyType g_total_suites_ran;

END_NAMESPACE(n19::test);

