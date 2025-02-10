/*
* Copyright (c) 2025 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef N19_TEST_ALL_HPP
#define N19_TEST_ALL_HPP
#include <Core/MacroUtil.hpp>
#include <Core/Panic.hpp>
#include <Bulwark/Registry.hpp>
#include <Bulwark/Reporting.hpp>
#include <Bulwark/BulwarkContext.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This header includes all unit testing related functionality.
// It also provides the necessary macros for setting up unit tests
// and their respective suites. This header should be included by every
// file that implements a test suite.

#define TESTCASE_FUNC_(SUITE, NAME) n19_test_##SUITE##_##NAME##_
#define TESTCASE_TYPE_(SUITE, NAME) n19_TestType##SUITE##_##NAME##_
#define TESTCASE_CTX_ case_ctx_

#define TEST_CASE(SUITE, NAME)                                                              \
  static void TESTCASE_FUNC_(SUITE, NAME)(::n19::test::ExecutionContext& TESTCASE_CTX_);    \
  struct TESTCASE_TYPE_(SUITE, NAME) {                                                      \
    TESTCASE_TYPE_(SUITE, NAME)() {                                                         \
      ::n19::test::g_registry.add_case(TESTCASE_FUNC_(SUITE, NAME), #NAME, #SUITE);         \
  }};                                                                                       \
  static struct TESTCASE_TYPE_(SUITE, NAME) N19_UNIQUE_NAME(TESTCASE_TYPE_(SUITE, NAME));   \
  static void TESTCASE_FUNC_(SUITE, NAME)(::n19::test::ExecutionContext& TESTCASE_CTX_)     \

#define REQUIRE(EXPR)                                                                       \
  do{ if( !(EXPR) ) {                                                                       \
    TESTCASE_CTX_.result = ::n19::test::Result::Failed;                                     \
    if(::n19::test::Context::the().flags_ & ::n19::test::Context::Verbose) {                \
      ::n19::test::report(#EXPR, ::n19::test::Result::Failed, TESTCASE_CTX_.out);           \
    }                                                                                       \
  } else if(::n19::test::Context::the().flags_ & ::n19::test::Context::Verbose) {           \
    ::n19::test::report(#EXPR, ::n19::test::Result::Passed, TESTCASE_CTX_.out);             \
  }} while(false);                                                                          \

#define SECTION(NAME, ...)                                                                  \
  ASSERT(TESTCASE_CTX_.section.empty(), "Nested case sections are not allowed!");           \
  TESTCASE_CTX_.section = #NAME;                                                            \
  if(::n19::test::Context::the().flags_ & ::n19::test::Context::Verbose) {                  \
    ::n19::test::report(#NAME, TESTCASE_CTX_.out);                                          \
  }                                                                                         \
  [=, &TESTCASE_CTX_]() mutable -> void __VA_ARGS__ ();                                     \
  TESTCASE_CTX_.section = "";                                                               \

#define REF_SECTION(NAME, ...)                                                              \
  ASSERT(TESTCASE_CTX_.section.empty(), "Nested case sections are not allowed!");           \
  TESTCASE_CTX_.section = #NAME;                                                            \
  if(::n19::test::Context::the().flags_ & ::n19::test::Context::Verbose) {                  \
    ::n19::test::report(#NAME, TESTCASE_CTX_.out);                                          \
  }                                                                                         \
  [&]() -> void __VA_ARGS__ ();                                                             \
  TESTCASE_CTX_.section = "";                                                               \

#define TEST_SKIP()                                                                         \
  ASSERT(TESTCASE_CTX_.section.empty(), "Cannot skip test case inside of a section!");      \
  TESTCASE_CTX_.result = ::n19::test::Result::Skipped;                                      \
  return;                                                                                   \

#define TEST_DIE()                                                                          \
  ASSERT(TESTCASE_CTX_.section.empty(), "Cannot use TEST_DIE() inside of a section!");      \
  TESTCASE_CTX_.result = ::n19::test::Result::Failed;                                       \
  return;                                                                                   \

#define TEST_FATAL(MESSAGE)                                                                 \
  ASSERT(TESTCASE_CTX_.section.empty(), "Cannot use TEST_FATAL() inside of a section!");    \
  if(::n19::test::Context::the().flags_ & ::n19::test::Context::Verbose) {                  \
    ::n19::test::diagnostic(MESSAGE, ::n19::test::Diagnostic::Fatal, TESTCASE_CTX_.out);    \
  }                                                                                         \
  TESTCASE_CTX_.result = ::n19::test::Result::Failed;                                       \
  return;                                                                                   \

#define TEST_WARN(MESSAGE)                                                                  \
  if(::n19::test::Context::the().flags_ & ::n19::test::Context::Verbose) {                  \
    ::n19::test::diagnostic(MESSAGE, ::n19::test::Diagnostic::Warn, TESTCASE_CTX_.out);     \
  }                                                                                         \

#define TEST_INFO(MESSAGE)                                                                  \
  if(::n19::test::Context::the().flags_ & ::n19::test::Context::Verbose) {                  \
    ::n19::test::diagnostic(MESSAGE, ::n19::test::Diagnostic::Info, TESTCASE_CTX_.out);     \
  }                                                                                         \

#define TEST_DEBUG(MESSAGE)                                                                 \
  if(auto f_ = ::n19::test::Context::the().flags_;                                          \
    (f_ & ::n19::test::Context::Verbose) && (f_ & ::n19::test::Context::Debug)) {           \
    ::n19::test::diagnostic(MESSAGE, ::n19::test::Diagnostic::Debug, TESTCASE_CTX_.out);    \
  }                                                                                         \

#endif //N19_TEST_ALL_HPP
