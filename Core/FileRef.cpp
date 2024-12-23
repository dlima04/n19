/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#include <Core/FileRef.hpp>
#include <Core/Callback.hpp>
#include <fstream>

auto n19::FileRef::open(const std::string &fname) -> Result<FileRef> {
  if(!fs::exists(fname)) {
    return make_error(ErrC::InvalidArg, "File does not exist.");
  } if(fs::is_directory(fname)) {
    return make_error(ErrC::InvalidArg, "File is a directory.");
  } if(!fs::is_regular_file(fname)) {
    return make_error(ErrC::InvalidArg, "File is irregular.");
  }

  return make_result<FileRef>(fname);
}

auto n19::FileRef::open(const std::wstring &fname) -> Result<FileRef> {
  if(!fs::exists(fname)) {
    return make_error(ErrC::InvalidArg, "File does not exist.");
  } if(fs::is_directory(fname)) {
    return make_error(ErrC::InvalidArg, "Path is a directory.");
  } if(!fs::is_regular_file(fname)) {
    return make_error(ErrC::InvalidArg, "File is not regular.");
  }

  return make_result<FileRef>(fname);
}

auto n19::FileRef::create_or_open(const std::wstring& fname) -> Result<FileRef> {
  FileRef ref(fname);
  std::ofstream file;
  const bool exists = fs::exists(fname);

  if(exists && fs::is_directory(fname)) {
    return make_error(ErrC::InvalidArg, "Path is a directory.");
  } if(exists && !fs::is_regular_file(fname)) {
    return make_error(ErrC::InvalidArg, "File is not regular.");
  } if(exists) {
    return ref;
  }

  file.open(fs::path(fname), std::ios::binary | std::ios::in | std::ios::out);
  if(!file.is_open()) {
    return make_error(ErrC::FileIO, "Failed to create file.");
  }

  file.close();
  return make_result<FileRef>(std::move(ref));
}

auto n19::FileRef::create_or_open(const std::string& fname) -> Result<FileRef> {
  FileRef ref(fname);
  std::ofstream file;
  const bool exists = fs::exists(fname);

  if(exists && fs::is_directory(fname)) {
    return make_error(ErrC::InvalidArg, "Path is a directory.");
  } if(exists && !fs::is_regular_file(fname)) {
    return make_error(ErrC::InvalidArg, "File is irregular.");
  } if(exists) {
    return ref;
  }

  file.open(fname, std::ios::binary | std::ios::in | std::ios::out);
  if(!file.is_open()) {
    return make_error(ErrC::FileIO, "File could not be created/opened.");
  }

  file.close();
  return make_result<FileRef>(std::move(ref));
}

auto n19::FileRef::create(const std::string& fname) -> Result<FileRef> {
  FileRef ref(fname);
  std::ofstream file;

  if(fs::exists(fname)) {
    return make_error(ErrC::InvalidArg, "The file already exists.");
  }
  file.open(fs::path(fname), std::ios::binary | std::ios::in | std::ios::out);
  if(!file.is_open()) {
    return make_error(ErrC::FileIO, "Failed to create/open file.");
  }

  file.close();
  return make_result<FileRef>(std::move(ref));
}

auto n19::FileRef::create(const std::wstring& fname) -> Result<FileRef> {
  FileRef ref(fname);
  std::ofstream file;

  if(fs::exists(fname)) {
    return make_error(ErrC::InvalidArg, "File already exists.");
  }

  file.open(fs::path(fname), std::ios::binary | std::ios::in | std::ios::out);
  if(!file.is_open()) {
    return make_error(ErrC::FileIO, "Could not create the file.");
  }

  file.close();
  return make_result<FileRef>(std::move(ref));
}

auto n19::FileRef::size() const -> Result<uintmax_t> try {
  const auto fsize = fs::file_size(path_);
  return make_result<uintmax_t>(fsize);
} catch(const std::exception& e) {
  return make_error(ErrC::FileIO, e.what());
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
    return make_error(ErrC::FileIO, "Could not open file.");
  }

  stream.write(
    reinterpret_cast<const std::ostream::char_type*>(bytes.data()),
    static_cast<std::streamsize>(bytes.size_bytes())
  );

  stream.close();
  return make_result<void>();
}

auto n19::FileRef::read_into(const WritableBytes& bytes) const -> Result<void> {
  ASSERT(!bytes.empty());
  std::ifstream stream(path_, std::ios::binary);
  DEFER_IF(stream.is_open(), stream.close());

  if(!stream.is_open()) {
    return make_error(ErrC::FileIO, "Could not open file.");
  } if(const auto _size = size(); !_size || bytes.size_bytes() < *_size) {
    return make_error(ErrC::Internal, "Invalid file size.");
  }

  stream.read(
    reinterpret_cast<std::istream::char_type*>(bytes.data()),
    static_cast<std::streamsize>(bytes.size_bytes())
  );

  return make_result<void>();
}
