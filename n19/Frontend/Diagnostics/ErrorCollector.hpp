/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <n19/Core/Common.hpp>
#include <n19/Core/ClassTraits.hpp>
#include <n19/Core/Platform.hpp>
#include <n19/System/File.hpp>
#include <n19/Core/Stream.hpp>
#include <n19/Core/Bytes.hpp>
#include <n19/System/String.hpp>
#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>

#define N19_MAX_ERRORS 40
BEGIN_NAMESPACE(rl);

using namespace n19;

struct ErrorLocation {
  std::string message;
  size_t file_pos;
  uint32_t line;
  bool is_warning;
};

class ErrorCollector {
  N19_MAKE_NONCOPYABLE(ErrorCollector);
public:
  static auto display_error(
    const std::string &msg,
    sys::File &file,
    OStream& stream,
    const size_t pos,
    const uint32_t line,
    bool is_warn = false
  ) -> void;

  static auto display_error(
    const std::string& msg,
    const sys::String& fname,
    const std::vector<char8_t>& buff,
    OStream& stream,
    const size_t pos,
    const uint32_t line,
    bool is_warn = false
  ) -> void;

  static auto display_error(
    const std::string& msg,
    class Lexer const& lxr,
    OStream& stream,
    bool is_warn = false
  ) -> void;

  static auto display_error(
    const std::string& msg,
    class Lexer const& lxr,
    class Token const& tok,
    OStream& stream,
    bool is_warn = false
  ) -> void;

  auto store_warning(
    const std::string &msg,
    const sys::String& file_name,
    const size_t pos,
    const uint32_t line
  ) -> ErrorCollector&;

  auto store_error(
    const std::string &msg,
    const sys::String& file_name,
    const size_t pos,
    const uint32_t line
  ) -> ErrorCollector&;

  auto store_error_or_warning(
    const sys::String& file_name,
    const ErrorLocation& err
  ) -> ErrorCollector&;

  auto emit(OStream& stream) const -> Result<void>;
  auto has_errors()  const -> bool;

  ErrorCollector() = default;
  ~ErrorCollector() = default;
private:
  std::unordered_map<
    sys::String,
    std::vector<ErrorLocation>
  > errs_; // The stored errors.
  uint32_t warning_count_ = 0;
  uint8_t error_count_    = 0;
};

/////////////////////////////////////////////////////////////////////

FORCEINLINE_ auto ErrorCollector::has_errors() const -> bool {
  return error_count_ > 0;
}

/////////////////////////////////////////////////////////////////////

END_NAMESPACE(rl);
