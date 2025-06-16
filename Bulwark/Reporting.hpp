/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <Bulwark/Case.hpp>
#include <Bulwark/Suite.hpp>
#include <IO/Console.hpp>
#include <Core/ClassTraits.hpp>
BEGIN_NAMESPACE(n19::test);

struct Diagnostic final {
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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

constinit extern size_t g_total_passed;
constinit extern size_t g_total_failed;
constinit extern size_t g_total_exc;
constinit extern size_t g_total_skipped;
constinit extern size_t g_total_suites;

END_NAMESPACE(n19::test);
