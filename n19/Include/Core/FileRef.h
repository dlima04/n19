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
#include <Core/Bytes.h>
#include <filesystem>
#include <cstdint>
#include <memory>
#include <vector>

namespace fs = std::filesystem;
namespace n19 {
  class FileRef;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// n19::FileRef is very similar to n19::IODevice, except
// that it specifically deals with files, and uses higher level
// STL features to accomplish its tasks rather than native
// API functions.

class n19::FileRef {
public:
  // Checks if two files' paths are lexically equal.
  auto operator==(const FileRef& other) const -> bool;
  auto operator!=(const FileRef& other) const -> bool;

  // Attempts to convert the type T to a
  // n19::ByteView (AKA std::span<std::byte).
  template<typename T> auto operator<<(const T& val) -> FileRef&;
  template<typename T> auto operator>>(T& val)       -> FileRef&;

  // Used for reading to/writing to the file, called by
  // operator<< and operator>> as well.
  auto write(const ByteView& bytes) -> FileRef&;
  auto read_into(ByteView& bytes)   -> FileRef&;

  [[nodiscard]] auto size() const     -> Result<uintmax_t>;
  [[nodiscard]] auto name() const     -> std::string;
  [[nodiscard]] auto absolute() const -> std::string;

  static auto create(const FileRef &other)          -> Result<FileRef>;
  static auto create(const std::wstring &file_name) -> Result<FileRef>;
  static auto create(const std::string &file_name)  -> Result<FileRef>;
private:
  explicit FileRef(const fs::path& path) : path_(path) {}
  fs::path path_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
auto n19::FileRef::operator<<(const T& val) -> FileRef& {
  if constexpr(std::constructible_from<std::span<T>, T>) {
    auto bytes = n19::as_bytes(val);
    write(bytes);
  } else if constexpr (std::is_trivially_constructible_v<T>){
    auto bytes = as_scalar_bytecopy(val);
    write(bytes);
  } else {
    static_assert(
    "FileRef::operator<< must be called with "
    "a type easily convertible to n19::ByteView.");
  }

  return *this;
}

template<typename T>
auto n19::FileRef::operator>>(T& val) -> FileRef& {
  static_assert(std::constructible_from<std::span<T>, T>);
  auto bv = n19::as_bytes(val);
  return read_into(bv);
}

inline auto n19::FileRef::operator!=(
const FileRef& other ) const -> bool
{
  return other.path_ != this->path_;
}

inline auto n19::FileRef::operator==(
  const FileRef& other ) const -> bool
{
  return other.path_ == this->path_;
}

#endif //FILEREF_H
