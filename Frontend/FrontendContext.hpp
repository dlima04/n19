/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <Core/ClassTraits.hpp>
#include <Core/Platform.hpp>
#include <Core/Maybe.hpp>
#include <System/String.hpp>
#include <IO/Console.hpp>
#include <vector>
#include <algorithm>
#include <cstdint>

#define N19_INVALID_INFILE_ID  0
#define N19_INVALID_OUTFILE_ID 0

BEGIN_NAMESPACE(n19);
struct VersionInfo {
  uint16_t major{};  /// Major version number.
  uint16_t minor{};  /// Minor version number.
  uint16_t patch{};  /// Patch number.
  std::string arch;  /// Target architecture - i.e. x86_64, ARM64, etc.
  std::string os;    /// Indicates the target operating system.
};

enum class InputFileState : uint8_t {
  Pending  = 0,      /// Needs to be parsed.
  Finished = 1,      /// File has been parsed.
};

enum class InputFileKind : uint8_t {
  CoreUnit = 0,      /// This is a core source file.
  Included = 1,      /// This was included with the "@include" directive.
};

struct InputFile {
  using ID = uint32_t;
  sys::String name;
  InputFileState state = InputFileState::Pending;
  InputFileKind kind = InputFileKind::CoreUnit;
  ID id = N19_INVALID_INFILE_ID;

  InputFile() = default;
  InputFile(sys::String&& n);
};

struct OutputFile {
  using ID = uint32_t;
  sys::String name;
  ID id = N19_INVALID_OUTFILE_ID;

  OutputFile() = default;
  OutputFile(sys::String&& n);
};

#define N19_FRONTEND_CONTEXT_FLAG_LIST                            \
  X(None,     0x00 << 0) /* Default flag value.               */  \
  X(Verbose,  0x01 << 0) /* Enable verbose output.            */  \
  X(Colours,  0x01 << 1) /* Pretty colours!                   */  \
  X(DumpIR,   0x01 << 2) /* Dump intermediate representation. */  \
  X(DumpAST,  0x01 << 3) /* Dump the abstract syntax tree.    */  \
  X(DumpEnts, 0x01 << 4) /* Dump the entity table.            */  \
  X(DumpToks, 0x01 << 5) /* Dump tokens, do not compile.      */  \
  X(DumpCtx,  0x01 << 6) /* Dump the frontend context object. */  \

class Context {
  N19_MAKE_NONMOVABLE(Context);
  N19_MAKE_NONCOPYABLE(Context);
public:
  #define X(NAME, VALUE) NAME = VALUE,
  enum Flags : uint32_t {
    N19_FRONTEND_CONTEXT_FLAG_LIST
  };
  #undef X

  static auto get_version_info()   -> VersionInfo;
  static auto get_next_output_id() -> OutputFile::ID;
  static auto get_next_input_id()  -> InputFile::ID;
  auto dump(OStream& stream = outs()) -> void;

  FORCEINLINE_ static auto the() -> Context& {
    static Context the_context;
    return the_context;
  }

  FORCEINLINE_ auto get_input_by_id(InputFile::ID id) -> Maybe<InputFile&> {
    auto it = std::ranges::find_if(inputs_, [id](const InputFile& f) {
      return f.id == id;
    });

    if(it == inputs_.end()) return Nothing;
    return Maybe<InputFile&>::create(static_cast<InputFile&>(*it));
  }

  FORCEINLINE_ auto get_output_by_id(OutputFile::ID id) -> Maybe<OutputFile&> {
    auto it = std::ranges::find_if(outputs_, [id](const OutputFile& f) {
      return f.id == id;
    });

    if(it == outputs_.end()) return Nothing;
    return Maybe<OutputFile&>::create(static_cast<OutputFile&>(*it));
  }

  std::underlying_type_t<Flags> flags_{};
  std::vector<InputFile> inputs_{};
  std::vector<OutputFile> outputs_{};

 ~Context() = default;
private:
  Context() = default;
};

END_NAMESPACE(n19);