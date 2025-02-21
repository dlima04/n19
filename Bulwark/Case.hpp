/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef N19_TEST_CASE_HPP
#define N19_TEST_CASE_HPP
#include <Core/ClassTraits.hpp>
#include <Core/ConIO.hpp>
#include <string_view>
#include <string>
#include <functional>
#include <cstdint>
BEGIN_NAMESPACE(n19::test);

struct Result {
N19_MAKE_COMPARABLE_MEMBER(Result, val_);
  enum Value_ : uint8_t {
    Failed    = 0x00,    /// The test has failed.
    Passed    = 0x01,    /// The test passed.
    Exception = 0x02,    /// An uncaught exception interrupted the test case.
    Skipped   = 0x03     /// The test case was skipped.
  };
                         ///
  Value_ val_ = Passed;  /// Underlying value.
  auto to_string() const -> std::string;
  auto to_colour() const -> Con;

  constexpr Result() = default;
  constexpr Result(const Value_ val) : val_(val) {}
};

struct ExecutionContext {
  Result result = Result::Passed;
  std::string_view section;
  OStream& out = outs();

  explicit ExecutionContext(OStream& s) : out(s) {}
  ExecutionContext() = default;
 ~ExecutionContext() = default;
};
                         ///
class Case final {       /// Test case implementation class
public:
  using NameType_ = std::string_view;
  using FuncType_ = void(*)(ExecutionContext&);

  FORCEINLINE_ auto operator()(ExecutionContext& ctx) -> void {
    try { this->fn_(ctx); } catch(...) { ctx.result = Result::Exception; }
  }
                         ///
  FuncType_ fn_;         /// Don't call this invocable object directly. Use operator().
  NameType_ name_;

 ~Case() = default;
  Case(const FuncType_& fn, const NameType_ &name) : fn_(fn), name_(name) {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Begin stream operators

inline auto operator<<(OStream& stream, const Result& r) -> OStream& {
  stream << r.to_string();
  return stream;
}

inline auto operator<<(OStream& stream, const Case& c) -> OStream& {
  stream << c.name_;
  return stream;
}

END_NAMESPACE(n19::test);
#endif //N19_TEST_CASE_HPP
