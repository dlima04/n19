/*
* Copyright (c) 2025 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef N19_TEST_CONTEXT_HPP
#define N19_TEST_CONTEXT_HPP
#include <Sys/String.hpp>
#include <Core/Platform.hpp>
#include <Core/ArgParse.hpp>
#include <vector>
#include <string>
#include <cstdint>
#include <algorithm>
#include <type_traits>
BEGIN_NAMESPACE(n19::test);

class Context {
  Context() = default;
public:
  enum Flags : uint16_t {
    None     = 0x00,      /// Default flag value.
    Verbose  = 0x01,      /// Enable verbose output.
    StopFail = 0x01 << 1, /// Stop execution of suites on the first failed case.
    Debug    = 0x01 << 2, /// Display debug messages inside test cases.
    Colours  = 0x01 << 3, /// Pretty colours!
  };

  N19_FORCEINLINE auto should_skip(const sys::StringView& s) -> bool {
    return std::ranges::find_if(suites_to_skip_, [s](const auto& o) {
      return o == s;
    }) != suites_to_skip_.end();
  }

  N19_FORCEINLINE auto should_run(const sys::StringView& s) -> bool {
    return std::ranges::find_if(suites_to_run_, [s](const auto& o) {
      return o == s;
    }) != suites_to_skip_.end();
  }

  N19_FORCEINLINE static auto the() -> Context& {
    static Context ctx_;  /// TODO:
    return ctx_;          /// maybe just make this a static global?
  }

  std::underlying_type_t<Flags> flags_ = None;
  std::vector<sys::String> suites_to_run_;
  std::vector<sys::String> suites_to_skip_;
};

END_NAMESPACE(n19::test);
#endif //N19_TEST_CONTEXT_HPP
