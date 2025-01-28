/*
* Copyright (c) 2025 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef N19_TEST_REGISTRY_HPP
#define N19_TEST_REGISTRY_HPP
#include <Bulwark/Case.hpp>
#include <Bulwark/Suite.hpp>
#include <vector>
#include <string_view>
#include <cstdint>
BEGIN_NAMESPACE(n19::test);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global singleton for holding test suites.
// constructor MUST be kept constexpr so constinit semantics are possible,
// avoiding the static initialization order "fiasco".

class Registry {
public:
  auto add_case(
    const Case::__FuncType& case_func,
    const Case::__NameType& case_name,
    const std::string_view& suite_name
  ) noexcept -> bool;

  std::vector<Suite> suites_;

  constexpr Registry() = default;
  ~Registry() = default;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global objects

constinit extern size_t g_total_passed;
constinit extern size_t g_total_failed;
constinit extern Registry g_registry;

END_NAMESPACE(n19::test);
#endif //N19_TEST_REGISTRY_HPP
