/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef N19_TEST_SUITE_HPP
#define N19_TEST_SUITE_HPP
#include <Sys/String.hpp>
#include <Bulwark/Case.hpp>
#include <deque>
BEGIN_NAMESPACE(n19::test);

class Suite {
public:
  sys::StringView name_ = _nstr("<UNNAMED>");
  std::deque<Case> cases_;

  auto run_all(OStream& s) -> void;
 ~Suite() = default;
  constexpr Suite() = default;
};

END_NAMESPACE(n19::test);
#endif //N19_TEST_SUITE_HPP
