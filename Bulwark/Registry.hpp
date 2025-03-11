/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef N19_TEST_REGISTRY_HPP
#define N19_TEST_REGISTRY_HPP
#include <Bulwark/Case.hpp>
#include <Bulwark/Suite.hpp>
#include <deque>
#include <string_view>
#include <cstdint>
BEGIN_NAMESPACE(n19::test);

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

constinit extern Registry g_registry;

END_NAMESPACE(n19::test);
#endif //N19_TEST_REGISTRY_HPP
