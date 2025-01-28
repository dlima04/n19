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
#include <Bulwark/Suite.hpp>
#include <Bulwark/Case.hpp>
#include <Bulwark/Registry.hpp>
#include <Bulwark/Reporting.hpp>
#include <Bulwark/Benchmark.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This header includes all unit testing related functionality.
// It also provides the necessary macros for setting up unit tests
// and their respective suites.

#define __TESTCASE_FUNC(SUITE, NAME) __n19_test_##SUITE##_##NAME
#define __TESTCASE_TYPE(SUITE, NAME) __n19_TestType##SUITE##_##NAME

#define TEST_CASE(SUITE, NAME)                                                               \
  static void __TESTCASE_FUNC(SUITE, NAME)(::n19::test::ExecutionContext& __ctx);            \
  struct __TESTCASE_TYPE(SUITE, NAME) {                                                      \
    __TESTCASE_TYPE(SUITE, NAME)() {                                                         \
      ::n19::test::g_registry.add_case(__TESTCASE_FUNC(SUITE, NAME), #NAME, #SUITE);         \
  }};                                                                                        \
  static struct __TESTCASE_TYPE(SUITE, NAME) N19_UNIQUE_NAME(__TESTCASE_TYPE(SUITE, NAME));  \
  static void __TESTCASE_FUNC(SUITE, NAME)(::n19::test::ExecutionContext& __ctx)             \

#define REQUIRE()

#endif //N19_TEST_ALL_HPP
