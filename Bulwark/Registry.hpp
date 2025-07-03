/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <Core/Common.hpp>
#include <Core/Platform.hpp>
#include <Bulwark/Case.hpp>
#include <Bulwark/Suite.hpp>
#include <System/String.hpp>
#include <list>
#include <memory>
#include <string_view>
#include <cstdint>
BEGIN_NAMESPACE(n19::test);

class Registry {
public:
  /// To be called during auto-registration
  auto add_case(
    const Case::FuncType_& case_func,
    const Case::NameType_& case_name,
    const sys::StringView& suite_name
  ) noexcept -> bool;

  auto run_all(OStream& stream = outs()) -> void;
  auto find(const sys::StringView& sv) -> Suite*;

  /// Explanation:
  /// We need this object to have a constexpr constructor.
  /// We will also be appending a LOT of suites and cases here,
  /// so we'll utilize std::list to avoid excessive copying/relocation. This should
  /// speed up performance once we have a large amount of suites and cases.
  ///
  /// std::list does NOT have a constexpr constructor, so we need to
  /// wrap this in a unique_ptr and initialize it when it's first used (sadly).
  std::unique_ptr<std::list<Suite>> suites_;

  constexpr Registry() = default;
  ~Registry() = default;
};

constinit extern Registry g_registry;

END_NAMESPACE(n19::test);
