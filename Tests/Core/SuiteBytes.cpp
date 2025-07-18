/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <catch2/catch_test_macros.hpp>
#include <n19/Core/Bytes.hpp>
#include <string>
#include <vector>
using namespace n19;

struct TestStruct {
  int x;
  std::string str;
  
  bool operator==(const TestStruct& other) const {
    return x == other.x && str == other.str;
  }
};

TEST_CASE("BasicFunctionality", "[Core.ByteCopy]") {
  SECTION("Construction") {
    ByteCopy<int> bc1(42);
    REQUIRE(bc1.alive());
    REQUIRE(bc1.value() == 42);

    ByteCopy<std::string> bc2("hello");
    REQUIRE(bc2.alive());
    REQUIRE(bc2.value() == "hello");
  }

  SECTION("ValueAccess") {
    ByteCopy<int> bc(42);
    REQUIRE(bc.value() == 42);
    REQUIRE(*bc == 42);
    REQUIRE(bc.value() == 42);
  }

  SECTION("EmptyConstruction") {
    ByteCopy<int> bc;
    REQUIRE(!bc.alive());
  }
}

TEST_CASE("CopySemantics", "[Core.ByteCopy]") {
  SECTION("CopyConstruction") {
    ByteCopy<int> bc1(42);
    ByteCopy<int> bc2(bc1);
    REQUIRE(bc1.alive());
    REQUIRE(bc2.alive());
    REQUIRE(bc1.value() == 42);
    REQUIRE(bc2.value() == 42);
  }

  SECTION("CopyAssignment") {
    ByteCopy<int> bc1(42);
    ByteCopy<int> bc2;
    bc2 = bc1;
    REQUIRE(bc1.alive());
    REQUIRE(bc2.alive());
    REQUIRE(bc1.value() == 42);
    REQUIRE(bc2.value() == 42);
  }

  SECTION("CopyComplexType") {
    TestStruct ts{42, "hello"};
    ByteCopy<TestStruct> bc1(ts);
    ByteCopy<TestStruct> bc2(bc1);
    REQUIRE(bc1.value().x == 42);
    REQUIRE(bc1.value().str == "hello");
    REQUIRE(bc2.value().x == 42);
    REQUIRE(bc2.value().str == "hello");
  }
}

TEST_CASE("MoveSemantics", "[Core.ByteCopy]") {
  SECTION("MoveConstruction") {
    ByteCopy<int> bc1(42);
    ByteCopy<int> bc2(std::move(bc1));
    REQUIRE(!bc1.alive());
    REQUIRE(bc2.alive());
    REQUIRE(bc2.value() == 42);
  }

  SECTION("MoveAssignment") {
    ByteCopy<int> bc1(42);
    ByteCopy<int> bc2;
    bc2 = std::move(bc1);
    REQUIRE(!bc1.alive());
    REQUIRE(bc2.alive());
    REQUIRE(bc2.value() == 42);
  }

  SECTION("MoveComplexType") {
    TestStruct ts{42, "hello"};
    ByteCopy<TestStruct> bc1(ts);
    ByteCopy<TestStruct> bc2(std::move(bc1));
    REQUIRE(!bc1.alive());
    REQUIRE(bc2.value().x == 42);
    REQUIRE(bc2.value().str == "hello");
  }
}

TEST_CASE("ValueModification", "[Core.ByteCopy]") {
  SECTION("ModifyValue") {
    ByteCopy<int> bc(42);
    bc.value() = 100;
    REQUIRE(bc.value() == 100);
  }

  SECTION("ReleaseValue") {
    ByteCopy<int> bc(42);
    int val = bc.release();
    REQUIRE(val == 42);
    REQUIRE(!bc.alive());
  }

  SECTION("ClearValue") {
    ByteCopy<int> bc(42);
    bc.clear();
    REQUIRE(!bc.alive());
  }
}

TEST_CASE("ByteRepresentation", "[Core.ByteCopy]") {
  SECTION("BasicBytes") {
    ByteCopy<int> bc(42);
    auto bytes = bc.bytes();
    REQUIRE(bytes.size() == sizeof(int));
  }

  SECTION("ComplexTypeBytes") {
    TestStruct ts{42, "hello"};
    ByteCopy<TestStruct> bc(ts);
    auto bytes = bc.bytes();
    REQUIRE(bytes.size() == sizeof(TestStruct));
  }
}

TEST_CASE("HelperFunctions", "[Core.ByteCopy]") {
  SECTION("AsByteCopy") {
    int val = 42;
    auto bc = as_bytecopy(val);
    REQUIRE(bc.alive());
    REQUIRE(bc.value() == 42);
  }

  SECTION("ConstructByteCopy") {
    auto bc = construct_bytecopy<TestStruct>(42, "hello");
    REQUIRE(bc.alive());
    REQUIRE(bc.value().x == 42);
    REQUIRE(bc.value().str == "hello");
  }
}

TEST_CASE("EdgeCases", "[Core.ByteCopy]") {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wself-assign"
  SECTION("SelfAssignment") {
    ByteCopy<int> bc(42);
    bc = bc;
    REQUIRE(bc.alive());
    REQUIRE(bc.value() == 42);
  }
#pragma GCC diagnostic pop

  SECTION("EmptyCopy") {
    ByteCopy<int> bc1;
    ByteCopy<int> bc2(bc1);
    REQUIRE(!bc1.alive());
    REQUIRE(!bc2.alive());
  }

  SECTION("EmptyMove") {
    ByteCopy<int> bc1;
    ByteCopy<int> bc2(std::move(bc1));
    REQUIRE(!bc1.alive());
    REQUIRE(!bc2.alive());
  }
}
