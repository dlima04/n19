#include <ErrorCollector.h>
#include <FileRef.h>
#include <Panic.h>
#include <algorithm>
#include <Result.h>
#include <ResultMacros.h>

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
  const std::string &msg,        // The error/warning message.
  const std::string &file_name,  // The name given to this file.
  const std::vector<char> &buff, // File buffer.
  size_t pos,                    // File buffer offset.
  const uint32_t line,           // Line number, optional
  const bool is_warn ) -> void   // Red/yellow error text
{
  std::string filler;
  std::string whitespace;

  if(pos >= buff.size()) {
    pos = buff.size() - 1;
  }

  //
  // Calculate the length of the line that "pos" is on.
  // Get the entire line as a string.
  //
  auto line_start = pos;
  auto line_end   = pos;

  while(line_start-- != 0 && buff[line_start] != '\n');
  while(line_end++ < buff.size() && buff[line_end] != '\n');

  const auto offset = pos - (line_start + 1);
  auto full_line    = std::string(&buff[line_start], line_end - line_start);

  if(full_line.empty() || offset >= full_line.size()) {
    FATAL("Unexpected end of file.");
  }
  if(full_line.front() == '\n') {
    full_line.erase(0,1);
  }

  //
  // Create text filler below full line with arrow.
  //
  filler.resize(full_line.size());
  whitespace.resize(offset);

  std::ranges::fill(filler, '~');
  std::ranges::fill(whitespace, ' ');

  ASSERT(offset < filler.size());
  filler[offset] = '^';

  //
  // File header: bold. Error message: red/yellow.
  //
  set_console(ConStyle::Bold);
  std::println("In {}{}", file_name, !line ? std::string("") : ':' + std::to_string(line));

  reset_console();
  std::println("{}", full_line);
  std::println("{}", filler);

  set_console(is_warn ? ConFg::Yellow : ConFg::Red);
  std::println("{}{}\n", whitespace, msg);
  reset_console();
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



