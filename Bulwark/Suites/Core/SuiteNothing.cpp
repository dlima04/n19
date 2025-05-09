/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Bulwark/Bulwark.hpp>
#include <Core/Nothing.hpp>
#include <Core/Maybe.hpp>
using namespace n19;

TEST_CASE(Nothing, BasicFunctionality) {
  SECTION(DefaultConstruction, {
    Nothing_ n1;
    Nothing_ n2;
    REQUIRE(n1.dummy_value_ == 0);
    REQUIRE(n2.dummy_value_ == 0);
  });

  SECTION(GlobalInstance, {
    REQUIRE(Nothing.dummy_value_ == 0);
  });

  SECTION(ConstexprBehavior, {
    (void)TESTCASE_CTX_; /// disable annoying warning
    
    constexpr Nothing_ n1;
    constexpr auto n2 = Nothing;
    static_assert(n1.dummy_value_ == 0);
    static_assert(n2.dummy_value_ == 0);
  });
}

TEST_CASE(Nothing, MaybeIntegration) {
  SECTION(MaybeConstruction, {
    Maybe<int> m1 = Nothing;
    Maybe<std::string> m2 = Nothing;
    REQUIRE(!m1.has_value());
    REQUIRE(!m2.has_value());
  });

  SECTION(MaybeAssignment, {
    Maybe<int> m1(42);
    REQUIRE(m1.has_value());
    m1 = Nothing;
    REQUIRE(!m1.has_value());
  });
} 
