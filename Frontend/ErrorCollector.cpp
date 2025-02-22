/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Core/FileRef.hpp>
#include <Core/Maybe.hpp>
#include <Core/Result.hpp>
#include <Core/Bytes.hpp>
#include <Core/Try.hpp>
#include <Core/Panic.hpp>
#include <Core/ConIO.hpp>
#include <Frontend/ErrorCollector.hpp>
#include <Frontend/Lexer.hpp>
#include <algorithm>
#include <cctype>

auto n19::ErrorCollector::store_error(
  const std::string& msg,
  const sys::String& file_name,
  const size_t pos,
  const uint32_t line ) -> ErrorCollector&
{
  ASSERT(line);
  ++error_count_;
  errs_[file_name].emplace_back( msg, pos, line, false );
  return *this;
}

auto n19::ErrorCollector::store_warning(
  const std::string& msg,
  const sys::String& file_name,
  const size_t pos,
  const uint32_t line ) -> ErrorCollector&
{
  ASSERT(line);
  ++warning_count_;
  errs_[file_name].emplace_back( msg, pos, line, true );
  return *this;
}

auto n19::ErrorCollector::store_error_or_warning(
  const sys::String& file_name,
  const ErrorLocation& err ) -> ErrorCollector&
{
  ASSERT(err.line);
  if(!err.is_warning) {
    ++error_count_;
  } else {
    ++warning_count_;
  }

  errs_[file_name].emplace_back(err);
  return *this;
}

auto n19::ErrorCollector::display_error(
  const std::string& msg,
  const Lexer &lxr,
  OStream& stream,
  const bool is_warn ) -> void
{
  const auto current = lxr.current();
  display_error(
    msg,             /// forward message
    lxr.file_name_,  ///
    lxr.src_,        ///
    stream,          /// Forward provided output stream.
    current.pos_,    /// assume current token's position.
    current.line_,   /// assume current token's line.
    is_warn);        ///
}

auto n19::ErrorCollector::display_error(
  const std::string& msg,
  const Lexer &lxr,
  const Token& tok,
  OStream& stream,
  const bool is_warn ) -> void
{
  display_error(
    msg,             /// forward message
    lxr.file_name_,  ///
    lxr.src_,        ///
    stream,          /// Forward provided output stream.
    tok.pos_,        /// error on passed token's position.
    tok.line_,       /// error on passed token's line #.
    is_warn);        ///
}

auto n19::ErrorCollector::display_error(
  const std::string& msg,
  const FileRef& file,
  OStream& stream,
  const size_t pos,
  const uint32_t line,
  const bool is_warn ) -> void
{
  const auto fsize = file.size();
  if(!fsize) return;

  std::vector<char8_t> buff(*fsize);
  auto view = as_writable_bytes(buff);

  if(file.read_into(view))
    display_error(
      msg,file.nstr(),
      buff,
      stream,
      pos,
      line,
      is_warn);
}

auto n19::ErrorCollector::display_error(
  const std::string& msg,
  const sys::String& fname,
  const std::vector<char8_t>& buff,
  OStream& stream,
  size_t pos,
  const uint32_t line,
  const bool is_warn ) -> void
{
  ASSERT(!buff.empty());
  std::string before;        /// The bytes that appear before "pos"
  std::string after;         /// The byte at "pos", and the ones after it.
  std::string filler;        /// The squiggly lines and pointy arrow.
  std::string spaces;        /// The spaces to the left of the message.

  if(pos >= buff.size()) {   /// Sanity check the position.
    pos = buff.size() - 1;   ///
  }

  Maybe<char> ch = '\0';
  auto access = [&buff](size_t index) -> Maybe<char> {
    if(index < buff.size()) return buff[index];
    return Nothing;
  };

  for(size_t i = pos - 1; ch && *ch != '\n'; i--) {
    ch = access(i);
    if(!ch.has_value()) break;
    if(std::iscntrl(static_cast<uint8_t>(*ch))) continue;
    before += ch.value();
    filler += '~';
  }

  ch = '\0';
  for(size_t i = pos; ch && *ch != '\n'; i++) {
    ch = access(i);
    if(!ch.has_value()) break;
    if(std::iscntrl(static_cast<uint8_t>(*ch))) continue;
    after += ch.value();
    filler += i == pos ? '^' : '~';
  }

  std::ranges::reverse(before);
  before += after;
  for(const auto character : filler) {
    if(character == '^') break;
    spaces += ' ';
  }

  stream
    << Con::Bold  << _nstr("In ") << fname << ':' << line
    << '\n'  << Con::Reset << before << '\n'
    << filler        /// Filler contents, tildes below line.
    << '\n'          ///
    << (is_warn ? Con::YellowFG : Con::RedFG)
    << spaces        ///
    << msg           /// Display user-provided message in red or yellow.
    << Con::Reset    /// Reset console.
    << Endl;         ///
}

auto n19::ErrorCollector::emit(OStream& stream) const -> Result<void> {
  std::vector<char8_t> buff;
  for(const auto &[file_name, errs] : errs_) {
    const auto file = TRY(FileRef::open(file_name));
    const auto size = TRY(file.size());

    buff.resize(size);
    TRY(file.read_into(as_writable_bytes(buff)));

    /// Emit the error using the file buffer
    for(const auto& err : errs)
      display_error(
        err.message,
        file_name,
        buff,
        stream,
        err.file_pos,
        err.line,
        err.is_warning);
  }

  return Result<void>::create();
}
