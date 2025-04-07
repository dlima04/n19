/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Bulwark/Bulwark.hpp>
#include <Core/Tuple.hpp>
#include <string>
using namespace n19;

struct NonCopyable {
  NonCopyable() = default;
  NonCopyable(const NonCopyable&) = delete;
  NonCopyable& operator=(const NonCopyable&) = delete;
  NonCopyable(NonCopyable&&) = default;
  NonCopyable& operator=(NonCopyable&&) = default;
};

TEST_CASE(Tuple, BasicFunctionality) {
  SECTION(Construction, {
    auto t1 = n19::make_tuple(42, std::string("hello"), 3.14);
    REQUIRE(t1.get<0>() == 42);
    REQUIRE(t1.get<1>() == "hello");
    REQUIRE(t1.get<2>() == 3.14);
  });

  SECTION(EmptyTuple, {
    Tuple<> t;
    REQUIRE(std::tuple_size<decltype(t)>::value == 0);
  });

  SECTION(SingleElement, {
    auto t = n19::make_tuple(42);
    REQUIRE(t.get<0>() == 42);
  });
}

TEST_CASE(Tuple, StructuredBindings) {
  SECTION(BasicBindings, {
    auto t = n19::make_tuple(42, std::string("hello"), 3.14);
    auto [x, y, z] = t;
    REQUIRE(x == 42);
    REQUIRE(y == "hello");
    REQUIRE(z == 3.14);
  });

  SECTION(ConstBindings, {
    const auto t = n19::make_tuple(42, std::string("hello"), 3.14);
    const auto [x, y, z] = t;
    REQUIRE(x == 42);
    REQUIRE(y == "hello");
    REQUIRE(z == 3.14);
  });

  SECTION(ReferenceBindings, {
    auto t = n19::make_tuple(42, std::string("hello"), 3.14);
    auto& [x, y, z] = t;
    x = 100;
    REQUIRE(t.get<0>() == 100);
  });
}

TEST_CASE(Tuple, CopySemantics) {
  SECTION(CopyConstruction, {
    auto t1 = n19::make_tuple(42, std::string("hello"), 3.14);
    auto t2 = t1;
    REQUIRE(t2.get<0>() == 42);
    REQUIRE(t2.get<1>() == "hello");
    REQUIRE(t2.get<2>() == 3.14);
  });

  SECTION(CopyAssignment, {
    auto t1 = n19::make_tuple(42, std::string("hello"), 3.14);
    Tuple<int, std::string, double> t2;
    t2 = t1;
    REQUIRE(t2.get<0>() == 42);
    REQUIRE(t2.get<1>() == "hello");
    REQUIRE(t2.get<2>() == 3.14);
  });
}

TEST_CASE(Tuple, MoveSemantics) {
  SECTION(MoveConstruction, {
    auto t1 = n19::make_tuple(42, std::string("hello"), 3.14);
    auto t2 = std::move(t1);
    REQUIRE(t2.get<0>() == 42);
    REQUIRE(t2.get<1>() == "hello");
    REQUIRE(t2.get<2>() == 3.14);
  });

  SECTION(MoveAssignment, {
    auto t1 = n19::make_tuple(42, std::string("hello"), 3.14);
    Tuple<int, std::string, double> t2;
    t2 = std::move(t1);
    REQUIRE(t2.get<0>() == 42);
    REQUIRE(t2.get<1>() == "hello");
    REQUIRE(t2.get<2>() == 3.14);
  });

  SECTION(NonCopyableTypes, {
    auto t1 = n19::make_tuple(NonCopyable{}, 42);
    auto t2 = std::move(t1);
    REQUIRE(t2.get<1>() == 42);
  });
}

TEST_CASE(Tuple, HelperFunctions) {
  SECTION(MakeTuple, {
    auto t = n19::make_tuple(42, std::string("hello"), 3.14);
    REQUIRE(t.get<0>() == 42);
    REQUIRE(t.get<1>() == "hello");
    REQUIRE(t.get<2>() == 3.14);
  });

  SECTION(TupleAccessor, {
    auto t = n19::make_tuple(42, std::string("hello"), 3.14);
    REQUIRE(tuple_accessor<0>(t) == 42);
    REQUIRE(tuple_accessor<1>(t) == "hello");
    REQUIRE(tuple_accessor<2>(t) == 3.14);
  });

  SECTION(TupleAccessorMove, {
    auto t = n19::make_tuple(42, std::string("hello"), 3.14);
    REQUIRE(tuple_accessor<0>(std::move(t)) == 42);
  });
}

TEST_CASE(Tuple, DifferentTypes) {
  SECTION(PrimitiveTypes, {
    auto t = n19::make_tuple(42, 3.14, true, 'a');
    REQUIRE(t.get<0>() == 42);
    REQUIRE(t.get<1>() == 3.14);
    REQUIRE(t.get<2>() == true);
    REQUIRE(t.get<3>() == 'a');
  });

  SECTION(ComplexTypes, {
    std::string str = "hello";
    std::vector<int> vec = {1, 2, 3};
    auto t = n19::make_tuple(str, vec);
    REQUIRE(t.get<0>() == "hello");
    REQUIRE(t.get<1>() == vec);
  });

  SECTION(MixedTypes, {
    auto t = n19::make_tuple(42, std::string("hello"), 3.14, true);
    REQUIRE(t.get<0>() == 42);
    REQUIRE(t.get<1>() == "hello");
    REQUIRE(t.get<2>() == 3.14);
    REQUIRE(t.get<3>() == true);
  });
}

TEST_CASE(Tuple, EdgeCases) {
  SECTION(LargeTuple, {
    auto t = n19::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    REQUIRE(t.get<0>() == 1);
    REQUIRE(t.get<9>() == 10);
  });

  SECTION(NestedTuples, {
    auto inner = n19::make_tuple(1, 2);
    auto outer = n19::make_tuple(inner, 3);
    REQUIRE(outer.get<0>().get<0>() == 1);
    REQUIRE(outer.get<0>().get<1>() == 2);
    REQUIRE(outer.get<1>() == 3);
  });

  SECTION(ConstTuple, {
    const auto t = n19::make_tuple(42, std::string("hello"), 3.14);
    REQUIRE(t.get<0>() == 42);
    REQUIRE(t.get<1>() == "hello");
    REQUIRE(t.get<2>() == 3.14);
  });
}