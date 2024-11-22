/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#include <ErrorCollector.h>
#include <FileRef.h>
#include <Result.h>
#include <ResultMacros.h>
#include <Panic.h>
#include <algorithm>
#include <stdexcept>
#include <cctype>

auto n19::ErrorCollector::store_error(
  const std::string &msg,
  const std::string &file_name,
  const size_t pos,
  const uint32_t line ) -> ErrorCollector&
{
  ASSERT(pos);
  ASSERT(line);

  max_err_chk();
  ++error_count_;
  errs_[file_name].emplace_back( msg, pos, line, false );
  return *this;
}

auto n19::ErrorCollector::store_warning(
  const std::string &msg,
  const std::string &file_name,
  const size_t pos,
  const uint32_t line ) -> ErrorCollector&
{
  ASSERT(pos);
  ASSERT(line);

  ++warning_count_;
  errs_[file_name].emplace_back( msg, pos, line, true );
  return *this;
}

auto n19::ErrorCollector::store_error_or_warning(
  const std::string &file_name,
  const ErrorLocation &err ) -> ErrorCollector&
{
  ASSERT(err.file_pos);
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
  const auto fsize = MUST(file.size());
  const auto buff = MUST(file.get_flat(*fsize));
  display_error(msg, file.name(), buff.value(), pos, line, is_warn);
}

auto n19::ErrorCollector::display_error(
  const std::string& msg,        // The error/warning message.
  const std::string& file_name,  // The name given to this file.
  const std::vector<char>& buff, // File buffer.
  size_t pos,                    // File buffer offset.
  const uint32_t line,           // Line number, optional
  const bool is_warn ) -> void   // Red/yellow error text
{
  ASSERT(!buff.empty());
  std::string before;  // The bytes that appear before "pos"
  std::string after;   // The byte at "pos", and the ones after it.
  std::string filler;  // The squiggly lines and pointy arrow.
  std::string spaces;  // The spaces to the left of the message.

  if(pos >= buff.size()) {
    pos = buff.size() - 1;
  }

  try {
    for(size_t i = pos - 1; buff.at(i) != '\n'; i--) {
      const char ch = buff.at(i);
      if(!std::isprint(static_cast<uint8_t>(ch)))
        continue;
      before += ch;
      filler += '~';
    }
  } catch(...) {} // NOLINT(*-empty-catch)

  try {
    for(size_t i = pos; buff.at(i) != '\n'; i++) {
      const char ch = buff.at(i);
      if(!std::isprint(static_cast<uint8_t>(ch)))
        continue;
      after += ch;
      filler += i == pos ? '^' : '~';
    }
  } catch(...) {} // NOLINT(*-empty-catch)

  std::ranges::reverse(before);
  before += after;

  for(const auto ch : filler) {
    if(ch == '^') break;
    spaces += ' ';
  }

  set_console(Con::Bold);
  std::println("In {}{}", file_name, !line
    ? std::string("") : ':' + std::to_string(line));

  set_console(Con::Reset);
  std::println("{}", before);
  std::println("{}", filler);

  set_console(is_warn ? Con::Yellow : Con::Red);
  std::println("{}{}\n", spaces, msg);
  set_console(Con::Reset);
}

auto n19::ErrorCollector::emit() const -> Result<None> {
  for(const auto &[file_name, errs] : errs_) {
    // Load the file
    const auto file = TRY(FileRef::create(file_name));
    const auto size = TRY(file->size());
    const auto buff = TRY(file->get_flat(*size));

    // Print the error using the file buffer
    for(const auto &err : errs)
      display_error(
        err.message,
        file_name,
        *buff,
        err.file_pos,
        err.line,
        err.is_warning
      );
  }

  return make_result<None>();
}

auto n19::ErrorCollector::max_err_chk() const -> void {
  if(error_count_ + 1 >= N19_MAX_ERRORS) {
    [[maybe_unused]] const auto _ = emit();
    FATAL("Maximum amount of permitted errors "
      "reached. Aborting compilation now.");
  }
}

auto n19::ErrorCollector::has_errors() const -> bool {
  return error_count_ > 0;
}



