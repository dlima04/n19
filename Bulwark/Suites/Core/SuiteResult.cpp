/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Bulwark/Bulwark.hpp>
#include <Core/Result.hpp>
#include <Core/Try.hpp>
using namespace n19;

/// Note: for this test suite we don't need to be
/// very rigorous, because n19::Result relies on the stable and
/// tested std::variant for a lot of the heavy lifting.

struct CTORHelper {
  int x_{};
  int y_{};
  constexpr auto operator<=>(const CTORHelper&) const = default;
  constexpr CTORHelper() = default;
  constexpr CTORHelper(int x, int y) : x_(x), y_(y) {}
};

struct DTORHelper {
  int& ref_;
  DTORHelper(int& ref) : ref_(ref) {}
 ~DTORHelper() { ref_ += 1; }
};

TEST_CASE(Result, Construct) {
  Result<CTORHelper> obj1(300, 400);
  Result<CTORHelper> obj2 = Error{ErrC::Internal};
  auto obj3 = Result<CTORHelper>::create(100, 200);

  REQUIRE(obj1.has_value());
  REQUIRE(obj3.has_value());
  REQUIRE(!obj2.has_value());

  REQUIRE(obj1->x_ == 300);
  REQUIRE(obj1->y_ == 400);
  REQUIRE(obj3->x_ == 100);
  REQUIRE(obj3->y_ == 200);

  auto val = obj3.value_or(420, 69);
  REQUIRE(val == obj3.value());
}

TEST_CASE(Result, Destroy) {
  int num = 20;
  {
    Result<DTORHelper> obj1(static_cast<int&>(num));
    REQUIRE(obj1.has_value());
  }

  REQUIRE(num > 20);
  Result<int> obj2 = 33;
  auto err = obj2.error_or(ErrC::Internal, "blabla");
  REQUIRE(err.msg == "blabla");
}

TEST_CASE(Result, Try) {
  int num = 20;
  auto func1 = []() -> Result<int> {
    return Error{ErrC::Internal};
  };

  auto func2 = [&num, &func1]() -> Result<int> {
    auto val = TRY(func1());
    num += 2;
    return val;
  };

  auto res = func2();
  REQUIRE(num == 20); /// Must not have been altered.
  REQUIRE(!res.has_value());
}