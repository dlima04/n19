/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <catch2/catch_test_macros.hpp>
#include <Core/Defer.hpp>
#include <string>
#include <vector>
using namespace n19;

TEST_CASE("BasicFunctionality", "[Core.Defer]") {
  std::vector<int> execution_order;

  SECTION("SimpleDefer") {
    {
      DEFER({
        execution_order.push_back(2);
      });
      execution_order.push_back(1);
    }

    REQUIRE(execution_order.size() == 2);
    REQUIRE(execution_order[0] == 1);
    REQUIRE(execution_order[1] == 2);
  }

  SECTION("MultipleDefers") {
    {
      DEFER({
        execution_order.push_back(3);
      });
      DEFER({
        execution_order.push_back(2);
      });
      execution_order.push_back(1);
    }

    REQUIRE(execution_order.size() == 3);
    REQUIRE(execution_order[0] == 1);

    const bool expr1 = execution_order[1] == 2 || execution_order[1] == 3;
    const bool expr2 = execution_order[2] == 3 || execution_order[2] == 2;

    REQUIRE(expr1);
    REQUIRE(expr2);
  }
}

TEST_CASE("VariableCapture", "[Core.Defer]") {
  SECTION("ReferenceCapture") {
    int value = 42;
    {
      DEFER({
        value = 100;
      });
      REQUIRE(value == 42);
    }
    REQUIRE(value == 100);
  }

  SECTION("MultipleVariables") {
    std::string str = "hello";
    int num = 42;
    {
      DEFER({
        str = "world";
        num = 100;
      });
      REQUIRE(str == "hello");
      REQUIRE(num == 42);
    }
    REQUIRE(str == "world");
    REQUIRE(num == 100);
  }
}

TEST_CASE("ConditionalExecution", "[Core.Defer]") {
  std::vector<int> execution_order;

  SECTION("TrueCondition") {
    {
      DEFER_IF(true, {
        execution_order.push_back(2);
      });
      execution_order.push_back(1);
    }

    REQUIRE(execution_order.size() == 2);
    REQUIRE(execution_order[0] == 1);
    REQUIRE(execution_order[1] == 2);
  }

  SECTION("FalseCondition") {
    {
      DEFER_IF(false, {
        execution_order.push_back(2);
      });
      execution_order.push_back(1);
    }

    REQUIRE(execution_order.size() == 1);
    REQUIRE(execution_order[0] == 1);
  }
}

