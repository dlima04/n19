/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" License.
* See the LICENSE file located at this project's root directory for
* more information.
*/

#ifndef ERRORCOLLECTOR_H
#define ERRORCOLLECTOR_H
#define N19_MAX_ERRORS 40
#include <FileRef.h>
#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>

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
public:
  static auto display_error(
    const std::string &msg,
    const FileRef &file,
    size_t pos,
    uint32_t line,
    bool is_warn
  ) -> void;

  static auto display_error(
    const std::string &msg,
    const std::string &file_name,
    const std::vector<char> &buff,
    size_t pos,
    uint32_t line,
    bool is_warn
  ) -> void;

  auto store_warning(
    const std::string &msg,
    const std::string &file_name,
    size_t pos,
    uint32_t line
  ) -> ErrorCollector&;

  auto store_error(
    const std::string &msg,
    const std::string &file_name,
    size_t pos,
    uint32_t line
  ) -> ErrorCollector&;

  auto store_error_or_warning(
    const std::string &file_name,
    const ErrorLocation& err
  ) -> ErrorCollector&;

  auto emit()        const -> Result<None>;
  auto max_err_chk() const -> void;
  auto has_errors()  const -> bool;

  ErrorCollector() = default;
  ~ErrorCollector() = default;
private:
  std::unordered_map<
    std::string,
    std::vector<ErrorLocation>
  > errs_; // The stored errors.
  uint32_t warning_count_ = 0;
  uint8_t error_count_    = 0;
};

#endif //ERRORCOLLECTOR_H
