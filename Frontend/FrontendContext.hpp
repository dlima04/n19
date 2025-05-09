/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef N19_FRONTENDCONTEXT_HPP
#define N19_FRONTENDCONTEXT_HPP
#include <Core/ClassTraits.hpp>
#include <Core/Platform.hpp>
#include <Core/ArgParse.hpp>
#include <Sys/String.hpp>
#include <vector>
#include <type_traits>
#include <algorithm>
#include <cstdint>
BEGIN_NAMESPACE(n19);

struct VersionInfo {
  uint16_t major{};
  uint16_t minor{};
  uint16_t patch{};
  std::string msg;
  std::string arch;
  std::string os;
};

class Context {
  N19_MAKE_NONMOVABLE(Context);
  N19_MAKE_NONCOPYABLE(Context);
public:
  enum Flags : uint32_t {
    None     = 0x00,      /// Default flag value.
    Verbose  = 0x01,      /// Enable verbose output.
    Colours  = 0x01 << 1, /// Pretty colours!
    DumpIR   = 0x01 << 2, /// Dump internal IR repr
    DumpAST  = 0x01 << 3, /// Dump the AST
    DumpEnts = 0x01 << 4, /// Dump the entity table
  };

  static auto get_version_info() -> VersionInfo;
  
  FORCEINLINE_ static auto the() -> Context& {
    static Context the_context;
    return the_context;
  }

  FORCEINLINE_ auto file_was_given(const sys::String& s) -> bool {
    return std::ranges::find(inputs_, s) != inputs_.end();
  }

  std::underlying_type_t<Flags> flags_{};
  argp::PackType inputs_{};
  argp::PackType outputs_{};

 ~Context() = default;
private:
  Context() = default;
};

END_NAMESPACE(n19);
#endif //N19_FRONTENDCONTEXT_HPP
