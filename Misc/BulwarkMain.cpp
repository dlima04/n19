/*
* Copyright (c) 2025 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#include <Core/ConIO.hpp>
#include <Bulwark/All.hpp>

TEST_CASE(TestSuite, HelloWorld) {
  __ctx.curr_section = "fucker";
  n19::outs() << "Hello from test!!\n";
}

int main() {
  using namespace n19;

  test::ExecutionContext ctx;
  for(const auto& suite : test::g_registry.suites_) {
    outs() << suite.name_ << '\n';
    suite.cases_.front().fn_(ctx);
    outs() << ctx.curr_section << '\n';
  }

  outs().flush();
  return 0;
}