/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Core/FileRef.hpp>
#include <Core/Callback.hpp>
#include <Core/Try.hpp>
#include <fstream>

auto n19::FileRef::open(const std::string &fname) -> Result<FileRef> {
  ERROR_IF(!fs::exists(fname), ErrC::InvalidArg, "Does not exist");
  ERROR_IF(!fs::is_regular_file(fname), ErrC::InvalidArg);
  ERROR_IF(fs::is_directory(fname), ErrC::InvalidArg, "Directory");
  return Result<FileRef>::create(fname);
}

auto n19::FileRef::open(const std::wstring &fname) -> Result<FileRef> {
  ERROR_IF(!fs::exists(fname), ErrC::InvalidArg, "Does not exist");
  ERROR_IF(!fs::is_regular_file(fname), ErrC::InvalidArg);
  ERROR_IF(fs::is_directory(fname), ErrC::InvalidArg, "Directory");
  return Result<FileRef>::create(fname);
}

auto n19::FileRef::create_or_open(const std::wstring& fname) -> Result<FileRef> {
  FileRef ref(fname);
  std::ofstream file;
  const bool exists = fs::exists(fname);

  ERROR_IF(exists && !fs::is_regular_file(fname), ErrC::InvalidArg);
  ERROR_IF(exists && fs::is_directory(fname), ErrC::InvalidArg, "Directory");
  if(exists) return ref;

  file.open(fs::path(fname), std::ios::binary | std::ios::in | std::ios::out);
  if(!file.is_open()) {
    return Error(ErrC::FileIO, "Failed to create file.");
  }

  file.close();
  return Result<FileRef>::create(std::move(ref));
}

auto n19::FileRef::create_or_open(const std::string& fname) -> Result<FileRef> {
  FileRef ref(fname);
  std::ofstream file;
  const bool exists = fs::exists(fname);

  ERROR_IF(exists && !fs::is_regular_file(fname), ErrC::InvalidArg);
  ERROR_IF(exists && fs::is_directory(fname), ErrC::InvalidArg, "Directory");
  if(exists) return ref;

  file.open(fname, std::ios::binary | std::ios::in | std::ios::out);
  if(!file.is_open()) {
    return Error(ErrC::FileIO, "File could not be created/opened.");
  }

  file.close();
  return Result<FileRef>::create(std::move(ref));
}

auto n19::FileRef::create(const std::string& fname) -> Result<FileRef> {
  FileRef ref(fname);
  std::ofstream file;

  if(fs::exists(fname)) {
    return Error(ErrC::InvalidArg, "The file already exists.");
  }

  file.open(fs::path(fname), std::ios::binary | std::ios::in | std::ios::out);
  if(!file.is_open()) {
    return Error(ErrC::FileIO, "Failed to create/open file.");
  }

  file.close();
  return Result<FileRef>::create(std::move(ref));
}

auto n19::FileRef::create(const std::wstring& fname) -> Result<FileRef> {
  FileRef ref(fname);
  std::ofstream file;

  if(fs::exists(fname)) {
    return Error(ErrC::InvalidArg, "File already exists.");
  }

  file.open(fs::path(fname), std::ios::binary | std::ios::in | std::ios::out);
  if(!file.is_open()) {
    return Error(ErrC::FileIO, "Could not create the file.");
  }

  file.close();
  return Result<FileRef>::create(std::move(ref));
}

auto n19::FileRef::size() const -> Result<uintmax_t> try {
  auto fsize = fs::file_size(path_);
  return fsize;
} catch(const std::exception& e) {
  return Error(ErrC::FileIO, e.what());
}

auto n19::FileRef::write(
  const Bytes& bytes, const bool app ) const -> Result<void>
{
  ASSERT(!bytes.empty());
  auto flags = std::ios::binary;
  if(app) {
    flags |= std::ios::app;
  }

  std::ofstream stream(path_, flags);
  if(!stream.is_open()) {
    return Error(ErrC::FileIO, "Could not open file.");
  }

  stream.write(
    reinterpret_cast<const std::ostream::char_type*>(bytes.data()),
    static_cast<std::streamsize>(bytes.size_bytes())
  );

  stream.close();
  return Result<void>::create();
}

auto n19::FileRef::read_into(const WritableBytes& bytes) const -> Result<void> {
  ASSERT(!bytes.empty());
  std::ifstream stream(path_, std::ios::binary);
  DEFER_IF(stream.is_open(), stream.close());

  if(!stream.is_open()) {
    return Error(ErrC::FileIO, "Could not open file.");
  } if(const auto _size = size(); !_size || bytes.size_bytes() < *_size) {
    return Error(ErrC::Internal, "Invalid file size.");
  }

  stream.read(
    reinterpret_cast<std::istream::char_type*>(bytes.data()),
    static_cast<std::streamsize>(bytes.size_bytes())
  );

  return Result<void>::create();
}
