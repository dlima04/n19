/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef FILEREF_HPP
#define FILEREF_HPP
#include <Core/Result.hpp>
#include <Core/Platform.hpp>
#include <Core/Maybe.hpp>
#include <Core/Bytes.hpp>
#include <Sys/String.hpp>
#include <filesystem>
#include <cstdint>
#include <utility>

namespace fs = std::filesystem;
BEGIN_NAMESPACE(n19);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// n19::FileRef is very similar to n19::IODevice, except
// that it specifically deals with files, and uses higher level
// STL features to accomplish its tasks rather than native
// API functions.

class FileRef {
public:
  template<typename T> auto operator<<(const T& val) -> FileRef&;
  template<typename T> auto operator>>(T& val)       -> FileRef&;

  // Used for reading to/writing to the file, called by
  // operator<< and operator>> as well.
  auto write(const Bytes& bytes, bool app = false) const -> Result<void>;
  auto read_into(const WritableBytes& bytes) const -> Result<void>;

  NODISCARD_ auto nstr() const -> sys::String;
  NODISCARD_ auto size() const -> Result<uintmax_t>;

  auto operator->(this auto&& self) -> auto*;
  auto path(this auto&& self)       -> auto&;
  auto operator*(this auto&& self)  -> auto&;

  // Creates a new file, ONLY if the specified file did not
  // exist beforehand. If the file already exists, the
  // call fails and an error value is returned.
  static auto create(const std::string& fname)  -> Result<FileRef>;
  static auto create(const std::wstring& fname) -> Result<FileRef>;

  // Creates or opens the specified file, as long as
  // the file path is lexically valid. If the file does not exist,
  // one will be created.
  static auto create_or_open(const std::wstring& fname) -> Result<FileRef>;
  static auto create_or_open(const std::string& fname)  -> Result<FileRef>;

  static auto open(const std::wstring& fname) -> Result<FileRef>;
  static auto open(const std::string& fname)  -> Result<FileRef>;

  FileRef() = default;
  explicit FileRef(const fs::path& path) : path_(path) {}
private:
  fs::path path_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
auto FileRef::operator<<(const T& val) -> FileRef& {
  if constexpr(std::ranges::contiguous_range<T>) {
    auto bytes = as_bytes(val);
    write(bytes);
  } else if constexpr (std::is_trivially_constructible_v<T>){
    auto copy = as_bytecopy(val);
    write(copy.bytes());
  } else {
    static_assert(
    "FileRef::operator<< must be called with "
    "a type easily convertible to n19::Bytes.");
  }

  return *this;
}

template<typename T>
auto FileRef::operator>>(T& val) -> FileRef& {
  static_assert(std::ranges::contiguous_range<T>);
  auto bytes = as_writable_bytes(val);
  read_into(bytes);
  return *this;
}

inline auto FileRef::nstr() const -> sys::String {
#if defined(N19_WIN32)
  return path_.wstring();
#else
  return path_.string();
#endif
}

FORCEINLINE_ auto FileRef::operator*(this auto &&self) -> auto& {
  return std::forward<decltype(self)>(self).path_;
}

FORCEINLINE_ auto FileRef::operator->(this auto &&self) -> auto* {
  return &(std::forward<decltype(self)>(self).path_);
}

FORCEINLINE_ auto FileRef::path(this auto &&self) -> auto& {
  return std::forward<decltype(self)>(self).path_;
}

END_NAMESPACE(n19);
#endif //FILEREF_HPP
