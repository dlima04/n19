/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#include <Core/FileRef.hpp>
#include <Core/Result.hpp>
#include <Core/Bytes.hpp>
#include <Core/ResultMacros.hpp>
#include <Core/Panic.hpp>
#include <Core/Except.hpp>
#include <Native/Stream.hpp>
#include <Frontend/ErrorCollector.hpp>
#include <algorithm>
#include <stdexcept>
#include <cctype>

auto n19::ErrorCollector::store_error(
  const std::string& msg,
  const native::String& file_name,
  const size_t pos,
  const uint32_t line ) -> ErrorCollector&
{
  ASSERT(line);
  max_err_chk();
  ++error_count_;
  errs_[file_name].emplace_back( msg, pos, line, false );
  return *this;
}

auto n19::ErrorCollector::store_warning(
  const std::string& msg,
  const native::String& file_name,
  const size_t pos,
  const uint32_t line ) -> ErrorCollector&
{
  ASSERT(line);
  ++warning_count_;
  errs_[file_name].emplace_back( msg, pos, line, true );
  return *this;
}

auto n19::ErrorCollector::store_error_or_warning(
  const native::String& file_name,
  const ErrorLocation& err ) -> ErrorCollector&
{
  ASSERT(err.line);
  if(!err.is_warning) {
    max_err_chk();
    ++error_count_;
  } else {
    ++warning_count_;
  }

  errs_[file_name].emplace_back(err);
  return *this;
}

auto n19::ErrorCollector::display_error(
  const std::string& msg,       // The error/warning message.
  const FileRef& file,          // The FileRef we need to read from.
  const size_t pos,             // File buffer offset.
  const uint32_t line,          // Line number, optional.
  const bool is_warn ) -> void  // Red/yellow error text
{
  const auto fsize = file.size();
  if(!fsize) {
    return;
  }
  std::vector<char8_t> buff(*fsize);
  auto view = n19::as_writable_bytes(buff);

  if(file.read_into(view))
    display_error(
      msg,
      file.nstr(),
      buff,
      pos,
      line,
      is_warn);
}

auto n19::ErrorCollector::display_error(
  const std::string& msg,           // The error/warning message.
  const native::String& fname,      // The name given to this file.
  const std::vector<char8_t>& buff, // File buffer.
  size_t pos,                       // File buffer offset.
  const uint32_t line,              // Line number, optional
  const bool is_warn ) -> void      // Red/yellow error text
{
  ASSERT(!buff.empty());
  std::string before;  // The bytes that appear before "pos"
  std::string after;   // The byte at "pos", and the ones after it.
  std::string filler;  // The squiggly lines and pointy arrow.
  std::string spaces;  // The spaces to the left of the message.

  if(pos >= buff.size()) {
    pos = buff.size() - 1;
  }

  IGNORE_EXCEPT(
  for(size_t i = pos - 1; buff.at(i) != '\n'; i--) {
    const char ch = buff.at(i);
    if(!std::isprint(static_cast<uint8_t>(ch)))
      continue;
    before += ch;
    filler += '~';
  });

  IGNORE_EXCEPT(
  for(size_t i = pos; buff.at(i) != '\n'; i++) {
    const char ch = buff.at(i);
    if(!std::isprint(static_cast<uint8_t>(ch)))
      continue;
    after += ch;
    filler += i == pos ? '^' : '~';
  });

  std::ranges::reverse(before);
  before += after;
  for(const auto ch : filler) {
    if(ch == '^') break;
    spaces += ' ';
  }

  set_console(Con::Bold);
  native::outs()
    << _nstr("In ")
    << fname;
  if(line != 0) {
    std::println(":{}", line);
  }

  set_console(Con::Reset);
  std::println("{}", before);
  std::println("{}", filler);

  set_console(is_warn ? Con::Yellow : Con::Red);
  std::println("{}{}\n", spaces, msg);
  set_console(Con::Reset);
}

auto n19::ErrorCollector::emit() const -> Result<None> {
  std::vector<char8_t> buff;
  for(const auto &[file_name, errs] : errs_) {
    const auto file = TRY(FileRef::open(file_name));
    const auto size = TRY(file->size());

    buff.resize(*size);
    file->read_into(n19::as_writable_bytes(buff)).OR_RETURN();

    // Print the error using the file buffer
    for(const auto &err : errs)
      display_error(
        err.message,
        file_name,
        buff,
        err.file_pos,
        err.line,
        err.is_warning
      );
  }

  return make_result<None>();
}



