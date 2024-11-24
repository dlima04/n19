/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef FILEREF_H
#define FILEREF_H
#include <Core/Result.h>
#include <filesystem>
#include <cstdint>
#include <memory>
#include <vector>

namespace fs = std::filesystem;
namespace n19 {
  class FileRef;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class n19::FileRef {
public:
  [[nodiscard]] auto size() const                      -> Result<uintmax_t>;
  [[nodiscard]] auto get_shared(uintmax_t amnt) const  -> Result<std::shared_ptr<std::vector<char>>>;
  [[nodiscard]] auto get_flat(uintmax_t amnt) const    -> Result<std::vector<char>>;
  [[nodiscard]] auto name() const                      -> std::string;
  [[nodiscard]] auto absolute() const                  -> std::string;

  static auto create(const FileRef &other)          -> Result<FileRef>;
  static auto create(const std::wstring &file_name) -> Result<FileRef>;
  static auto create(const std::string &file_name)  -> Result<FileRef>;
private:
  explicit FileRef(const fs::path& path) : path_(path) {}
  fs::path path_;
};

#endif //FILEREF_H
