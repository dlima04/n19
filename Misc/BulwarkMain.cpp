/*
* Copyright (c) 2025 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#include <Core/ConIO.hpp>
#include <Bulwark/Bulwark.hpp>
#include <Bulwark/BulwarkContext.hpp>
#include <algorithm>
using namespace n19;

TEST_CASE(Vector, VectorOfInts) {
  std::vector<int> x = {2,4,1,10,88,12,44,11};

  SECTION(Sorting, {
    std::ranges::sort(x);
    REQUIRE(x.front() == 1);
    REQUIRE(x.back() == 88);
  });

  SECTION(Find, {
    auto iter = std::ranges::find(x, 12);
    REQUIRE(iter != x.end());
    REQUIRE(*iter == 12);
  });

  TEST_INFO("Info diagnostic");
  TEST_WARN("Warning diagnostic");
}

int main() {
#ifdef N19_WIN32
  win32_init_console();
#endif

  test::Context::the().flags_ |= test::Context::Colours;
  test::Context::the().flags_ |= test::Context::Verbose;
  test::Context::the().flags_ |= test::Context::Debug;

  test::g_registry.run_all();

  outs().flush();
  return 0;
}