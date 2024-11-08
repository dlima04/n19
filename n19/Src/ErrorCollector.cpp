#include <ErrorCollector.h>
#include <FileRef.h>
#include <Result.h>
#include <ResultMacros.h>
#include <Panic.h>
#include <algorithm>

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
  if(pos >= buff.size()) {
    pos = buff.size() - 1;
  } if(buff[pos] == '\n') {
    pos = !pos ? 1 : pos - 1;
  } if(buff.empty()) {
    return;
  }

  //
  // Calculate the length of the line that "pos" is on.
  // Get the entire line as a string.
  //
  size_t line_start = pos;
  size_t line_end   = pos;
  try {
    while(line_start != 0 && buff.at(line_start) != '\n') {
      --line_start;
    }
    while(line_end < buff.size() - 1 && buff.at(line_end) != '\n') {
      ++line_end;
    }
  } catch(const std::exception& e) {
    PANIC(fmt("ErrorCollector::display_error: {}", e.what()));
  }

  const size_t arrow_off = pos - line_start;
  ASSERT(line_end < buff.size());
  ASSERT(line_start < buff.size());
  ASSERT(arrow_off < buff.size());

  //
  // Create text filler below full line with arrow.
  //
  std::string filler;
  std::string whitespace;
  std::string full_line;
  try {
    for(size_t i = line_start; i < line_end; i++)
      if(const auto& ch = buff.at(i); ch != '\n') {
        filler += '~';
        full_line += ch;
      }
    if(!filler.empty() && arrow_off < filler.size()) {
      filler.at(arrow_off) = '^';
    } else {
      filler += '^';
    }
  } catch(const std::exception& e) {
    PANIC(fmt("ErrorCollector::display_error: {}", e.what()));
  }

  whitespace.resize(arrow_off ? arrow_off : 1);
  std::ranges::fill(whitespace, ' ');

  //
  // Display the error message.
  //
  set_console(Con::Bold);
  std::println("In {}{}", file_name, !line
    ? std::string("") : ':' + std::to_string(line));

  set_console(Con::Reset);
  std::println("{}", full_line);
  std::println("{}", filler);

  set_console(is_warn ? Con::Yellow : Con::Red);
  std::println("{}{}\n", whitespace, msg);
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



