/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Bulwark/Bulwark.hpp>
#include <Core/Maybe.hpp>
#include <Core/TypeTraits.hpp>
using namespace n19;

struct CTORHelper1 {
  int& ref_;
  CTORHelper1(int& ref) : ref_(ref) { ref_ += 1; }
};

struct CTORHelper2 {
  int x_{};
  int y_{};
  CTORHelper2() = default;
  CTORHelper2(int x, int y) : x_(x), y_(y) {}
};

struct DTORHelper1 {
  int& ref_;
  DTORHelper1(int& ref) : ref_(ref) {}
 ~DTORHelper1(){ ref_ += 1; }
};

TEST_CASE(Maybe, Construct) {
  int num = 20;
  Maybe<CTORHelper1> obj1(static_cast<int&>(num));
  Maybe<CTORHelper2> obj2;

  REQUIRE(obj1.has_value());
  REQUIRE(num == 21);
  REQUIRE(!obj2.has_value());

  REF_SECTION(EmplaceValues, {
    obj2.emplace(420, 69);
    REQUIRE(obj2.has_value());
    REQUIRE(obj2->x_ == 420);
    REQUIRE(obj2->y_ == 69);
  });

  Maybe<CTORHelper2> obj3;
  REF_SECTION(CopyConstruction, {
    obj3 = obj2;
    REQUIRE(obj3.has_value());
    REQUIRE(obj3->x_ == obj2->x_);
    REQUIRE(obj3->y_ == obj2->y_);
  });

  Maybe<CTORHelper2> obj4;
  SECTION(MoveConstruction, {
    obj4 = obj3.release_value();
    REQUIRE(obj4.has_value());
  });

  if(obj4.has_value()) {
    TEST_FATAL("This wasn't supposed to happen...");
  }

  auto temp = obj4.value_or(CTORHelper2{ 999, 888 });
  REQUIRE(temp.x_ == 999 && temp.y_ == 888);
}

TEST_CASE(Maybe, Destroy) {
  int num1 = 20;
  Maybe<DTORHelper1> obj1(static_cast<int&>(num1));
  Maybe<DTORHelper1> obj2 = Nothing;
  Maybe<DTORHelper1> obj3(static_cast<int&>(num1));

  REQUIRE(obj1.has_value() && obj3.has_value());
  REQUIRE(!obj2.has_value());

  obj1.clear();
  obj3 = obj2;
  REQUIRE(num1 >= 22);
  REQUIRE(!obj3.has_value());
}

TEST_CASE(Maybe, Operators) {
  const Maybe<CTORHelper2> obj1(200, 400);
  Maybe<CTORHelper2> obj2(200, 400);

  constexpr bool has_cref = IsSame<decltype(*obj1), const CTORHelper2&>;
  constexpr bool has_ref  = IsSame<decltype(*obj2), CTORHelper2&>;
  constexpr bool has_cptr = IsSame<decltype(obj1.operator->()), const CTORHelper2*>;
  constexpr bool has_ptr  = IsSame<decltype(obj2.operator->()), CTORHelper2*>;

  REQUIRE(obj1.has_value() && obj2.has_value());
  REQUIRE(has_cref);
  REQUIRE(has_ref);
  REQUIRE(has_cptr);
  REQUIRE(has_ptr);
}

