/*
* Copyright (c) 2025 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef N19_TEST_SUITE_HPP
#define N19_TEST_SUITE_HPP
#include <Sys/String.hpp>
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
#endif //N19_TEST_SUITE_HPP
