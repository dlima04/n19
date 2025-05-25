/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef ERRORCOLLECTOR_HPP
#define ERRORCOLLECTOR_HPP
#include <Core/ClassTraits.hpp>
#include <Core/Platform.hpp>
#include <System/File.hpp>
#include <IO/Stream.hpp>
#include <Core/Bytes.hpp>
#include <System/String.hpp>
#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>

#define N19_MAX_ERRORS 40
BEGIN_NAMESPACE(n19);

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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FORCEINLINE_ auto ErrorCollector::has_errors() const -> bool {
  return error_count_ > 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

END_NAMESPACE(n19);
#endif //ERRORCOLLECTOR_HPP
