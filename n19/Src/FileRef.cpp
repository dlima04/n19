#include <FileRef.h>

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
  return make_result<FileRef>(FileRef(other.path_));
}

auto n19::FileRef::name() const -> Result<std::string> {
  return make_result<std::string>(path_.string());
}

auto n19::FileRef::size() const -> Result<uintmax_t> try {
  const auto fsize = fs::file_size(path_);
  return make_result<uintmax_t>(fsize);
  // unfortunately std::filesystem::file_size may throw...
} catch(const std::exception& e) {
  return make_error(ErrC::FileIO, "{}", e.what());
}


