/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <Core/Common.hpp>
#include <Core/Platform.hpp>
#include <System/String.hpp>
#include <Bulwark/Case.hpp>
#include <vector>
BEGIN_NAMESPACE(n19::test);

class Suite {
public:
  sys::StringView name_ = _nstr("<UNNAMED>");
  std::vector<Case> cases_;

  auto run_all(OStream& s) -> void;
 ~Suite() = default;
  constexpr Suite() = default;
};

END_NAMESPACE(n19::test);