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
    const Case::FuncType_& case_func,
    const Case::NameType_& case_name,
    const std::string_view& suite_name
  ) noexcept -> bool;

  auto run_all(OStream& stream = outs()) -> void;
  auto find(const sys::StringView& sv) -> Suite*;

  std::vector<Suite> suites_;
  constexpr Registry() = default;
  ~Registry() = default;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global registry object

constinit extern Registry g_registry;

END_NAMESPACE(n19::test);
#endif //N19_TEST_REGISTRY_HPP
