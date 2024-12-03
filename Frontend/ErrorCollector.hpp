/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef ERRORCOLLECTOR_HPP
#define ERRORCOLLECTOR_HPP
#include <Core/ClassTraits.hpp>
#include <Core/Platform.hpp>
#include <Core/FileRef.hpp>
#include <Core/Bytes.hpp>
#include <Native/String.hpp>
#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>

#define N19_MAX_ERRORS 40

namespace n19 {
  class ErrorCollector;
  struct ErrorLocation;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct n19::ErrorLocation {
  std::string message;
  size_t file_pos;
  uint32_t line;
  bool is_warning;
};

class n19::ErrorCollector {
N19_MAKE_NONCOPYABLE(ErrorCollector);
public:
  static auto display_error(
    const std::string &msg,
    const FileRef &file,
    const size_t pos,
    const uint32_t line,
    bool is_warn
  ) -> void;

  static auto display_error(
    const std::string& msg,
    const native::String& fname,
    const std::vector<char8_t>& buff,
    const size_t pos,
    const uint32_t line,
    bool is_warn
  ) -> void;

  auto store_warning(
    const std::string &msg,
    const native::String& file_name,
    const size_t pos,
    const uint32_t line
  ) -> ErrorCollector&;

  auto store_error(
    const std::string &msg,
    const native::String& file_name,
    const size_t pos,
    const uint32_t line
  ) -> ErrorCollector&;

  auto store_error_or_warning(
    const native::String& file_name,
    const ErrorLocation& err
  ) -> ErrorCollector&;

  auto emit()        const -> Result<None>;
  auto max_err_chk() const -> void;
  auto has_errors()  const -> bool;

  ErrorCollector() = default;
  ~ErrorCollector() = default;
private:
  std::unordered_map<
    native::String,
    std::vector<ErrorLocation>
  > errs_; // The stored errors.
  uint32_t warning_count_ = 0;
  uint8_t error_count_    = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

N19_FORCEINLINE auto n19::ErrorCollector::max_err_chk() const -> void {
  if(error_count_ + 1 < N19_MAX_ERRORS) [[likely]]
    return;
  [[maybe_unused]] const auto _ = emit();
  FATAL("Maximum amount of permitted errors reached. Aborting compilation now.");
}

N19_FORCEINLINE auto n19::ErrorCollector::has_errors() const -> bool {
  return error_count_ > 0;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //ERRORCOLLECTOR_HPP
