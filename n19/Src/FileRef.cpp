#include <FileRef.h>
#include <Defer.h>
#include <fstream>

auto n19::FileRef::create(const std::string &file_name) -> Result<FileRef> {
  if(!fs::exists(file_name)) {
    return make_error(ErrC::InvalidArg, "\"{}\" does not exist.", file_name);
  } if(fs::is_directory(file_name)) {
    return make_error(ErrC::InvalidArg, "\"{}\" is a directory.", file_name);
  } if(!fs::is_regular_file(file_name)) {
    return make_error(ErrC::InvalidArg, "\"{}\" is irregular.", file_name);
  }

  return make_result<FileRef>(FileRef(file_name));
}

auto n19::FileRef::create(const std::wstring &file_name) -> Result<FileRef> {
#if !defined(N19_WIN32)
  return make_error(ErrC::Platform, "Unexpected wide string on a POSIX platform.");
#else
  // Note: we can't actually format std::wstring.
  // It's not supported by the STL format API (i think).
  // The error messages will suck here. Oh well.
  if(!fs::exists(file_name)) {
    return make_error(ErrC::InvalidArg, "File does not exist.");
  } if(fs::is_directory(file_name)) {
    return make_error(ErrC::InvalidArg, "Path is a directory, not a file.");
  } if(!fs::is_regular_file(file_name)) {
    return make_error(ErrC::InvalidArg, "File is not regular.");
  }
#endif
  return make_result<FileRef>(FileRef(file_name));
}

auto n19::FileRef::create(const FileRef &other) -> Result<FileRef> {
#if defined(N19_WIN32)
  return create(other.path_.wstring());
#else
  return create(other.path_.string());
#endif
}

auto n19::FileRef::size() const -> Result<uintmax_t> try {
  const auto fsize = fs::file_size(path_);
  return make_result<uintmax_t>(fsize);
} catch(const std::exception& e) {
  return make_error(ErrC::FileIO, "{}", e.what());
}

auto n19::FileRef::get_shared(const uintmax_t amnt) const
-> Result<std::shared_ptr<std::vector<char>>>
{
  std::ifstream stream(path_.string(), std::ios::binary);
  DEFER_IF(stream.is_open(), [&] {
    stream.close();
  });

  if(!stream.is_open()) {
    return make_error(ErrC::FileIO, "Could not open file {}.", path_.string());
  } if(const auto _size = size(); !_size || amnt > *_size || !amnt) {
    return make_error(ErrC::Internal, "Invalid file size.");
  }

  auto buff = std::make_shared<std::vector<char>>(amnt);
  stream.read(buff->data(), static_cast<std::streamsize>(amnt));
  return buff;
}

auto n19::FileRef::get_flat(const uintmax_t amnt) const
-> Result<std::vector<char>>
{
  std::ifstream stream(path_.string(), std::ios::binary);
  DEFER_IF(stream.is_open(), [&] {
    stream.close();
  });

  if(!stream.is_open()) {
    return make_error(ErrC::FileIO, "Could not open file {}.", path_.string());
  } if(const auto _size = size(); !_size || amnt > *_size || !amnt) {
    return make_error(ErrC::Internal, "Invalid file size.");
  }

  std::vector<char> buff(amnt);
  stream.read(buff.data(), static_cast<std::streamsize>(amnt));
  return buff;
}

auto n19::FileRef::name() const -> std::string {
  return path_.string();
}

auto n19::FileRef::absolute() const -> std::string {
  return fs::absolute(path_).string();
}