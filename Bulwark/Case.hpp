/*
* Copyright (c) 2025 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef N19_TEST_CASE_HPP
#define N19_TEST_CASE_HPP
#include <string_view>
#include <functional>
#include <cstdint>
BEGIN_NAMESPACE(n19::test);

enum class Result : uint8_t {
  Failed    = 0x00,
  Passed    = 0x01,
  Exception = 0x02,
  Crashed   = 0x03,
  Skipped   = 0x04
};

struct ExecutionContext {
  Result result = Result::Passed;
  std::string curr_section;
};

class Case {
public:
  using __NameType = std::string_view;
  using __FuncType = std::function<void(ExecutionContext&)>;

  __FuncType fn_;
  __NameType name_;

 ~Case() = default;
  Case(const __FuncType& fn, const __NameType &name)
    : fn_(fn), name_(name) {}
};

END_NAMESPACE(n19::test);
#endif //N19_TEST_CASE_HPP
