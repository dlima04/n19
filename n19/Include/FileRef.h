#ifndef FILEREF_H
#define FILEREF_H
#include <Result.h>
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
  [[nodiscard]] Result<uintmax_t> size() const;
  [[nodiscard]] Result<std::vector<char> > get_flat(uintmax_t amnt) const;
  [[nodiscard]] Result<std::shared_ptr<std::vector<char>>> get_shared(uintmax_t amnt) const;
  [[nodiscard]] Result<std::string> name() const;

  static Result<FileRef> create(const FileRef& other);
  static Result<FileRef> create(const std::wstring& file_name);
  static Result<FileRef> create(const std::string& file_name);
private:
  explicit FileRef(const fs::path& path) : path_(path) {}
  fs::path path_;
};

#endif //FILEREF_H
