/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef LEXER_HPP
#define LEXER_HPP
#include <Core/Bytes.hpp>
#include <Core/Platform.hpp>
#include <Core/ClassTraits.hpp>
#include <Core/FileRef.hpp>
#include <Core/RingQueue.hpp>
#include <Frontend/Token.hpp>
#include <Native/String.hpp>
#include <memory>
#include <thread>
#include <vector>
#include <functional>
#include <string_view>
#include <unordered_map>
#include <cctype>
#include <cstdint>

#define UTF8_LEADING(CH) (((uint8_t)CH) >= 0x80)
#define CH_IS_XDIGIT(CH) (std::isxdigit((uint8_t)CH))
#define CH_IS_CTRL(CH)   (std::iscntrl((uint8_t)CH))
#define CH_IS_SPACE(CH)  (std::isspace((uint8_t)CH))
#define CH_IS_DIGIT(CH)  (std::isdigit((uint8_t)CH))

namespace n19 {
  class Lexer;
  struct Keyword;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// n19::Lexer uses a Single-Producer Single-Consumer relationship
// to feed tokens into a ringbuffer that can then be readily consumed by
// the parser. It holds the source buffer itself, a ringbuffer for the tokens,
// and a producer thread which runs in the background.

class n19::Lexer final : public std::enable_shared_from_this<Lexer> {
N19_MAKE_NONCOPYABLE(Lexer);
N19_MAKE_COMPARABLE_MEMBER(Lexer, file_name_);
public:
  auto current()              -> Token;
  auto consume(uint32_t amnt) -> Token;
  auto produce()              -> void;
  auto get_bytes() const      -> Bytes;
  auto peek(uint32_t amnt)    -> Token;
  auto testfuck() -> void;

  // Error handling.
  auto error(const std::string &msg, uint32_t line, uint32_t pos) const -> void;
  auto warn(const std::string &msg, uint32_t line, uint32_t pos)  const -> void;
  auto expect(TokenCategory cat, bool = true) -> Result<None>;
  auto expect(TokenType type, bool = true)    -> Result<None>;

  static auto create(const FileRef& ref)  -> Result<std::shared_ptr<Lexer>>;
  static auto keywords()                  -> const std::unordered_map<std::u8string_view, Keyword>&;
  static auto is_reserved_byte(char8_t c) -> bool;

  Lexer() = default;
  ~Lexer() = default;
private:
  // Internal character manipulation
  // methods for consuming/iterating over
  // raw bytes.
  auto _current_char() const -> char8_t;
  auto _consume_char(uint32_t) -> void;
  auto _advance_line() -> void;
  auto _skip_comment() -> void;
  auto _advance_consume_line() -> void;
  auto _skip_chars_until(std::function<bool(char8_t)> cb) -> bool;
  auto _skip_utf8_sequence() -> bool;
  auto _peek_char(uint32_t amnt = 1) const -> char8_t;

  // The token generators.
  // Each one of these is called by _consume_impl
  // for tokens longer than a single character
  auto _produce_impl()    -> Token;
  auto _token_hyphen()    -> Token;
  auto _token_plus()      -> Token;
  auto _token_asterisk()  -> Token;
  auto _token_fwdslash()  -> Token;
  auto _token_percent()   -> Token;
  auto _token_equals()    -> Token;
  auto _token_lthan()     -> Token;
  auto _token_null()      -> Token;
  auto _token_tilde()     -> Token;
  auto _token_at()        -> Token;
  auto _token_money()     -> Token;
  auto _token_lsqbrckt()  -> Token;
  auto _token_rsqbrckt()  -> Token;
  auto _token_semicolon() -> Token;
  auto _token_lparen()    -> Token;
  auto _token_rparen()    -> Token;
  auto _token_lbrace()    -> Token;
  auto _token_rbrace()    -> Token;
  auto _token_comma()     -> Token;
  auto _token_gthan()     -> Token;
  auto _token_ampersand() -> Token;
  auto _token_pipe()      -> Token;
  auto _token_bang()      -> Token;
  auto _token_uparrow()   -> Token;
  auto _token_quote()     -> Token;
  auto _token_squote()    -> Token;
  auto _token_colon()     -> Token;
  auto _token_dot()       -> Token;
  auto _token_ambiguous() -> Token;
  auto _token_hex_lit()   -> Token;
  auto _token_num_lit()   -> Token;
  auto _token_oct_lit()   -> Token;

  std::vector<char8_t> src_;  // Source file buffer.
  RingQueue<Token, 64> toks_; // Token ringbuffer.
  native::String file_name_;  // For error handling.
  uint32_t index_  = 0;       // Current source index.
  uint32_t line_ = 1;         // current line number.
};

struct n19::Keyword {
  TokenType type;
  TokenCategory cat;
  constexpr Keyword() = default;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// very short functions, inlined here for perf.

N19_FORCEINLINE auto n19::Lexer::peek(const uint32_t amnt) -> Token {
  if(const auto curr = current(); curr == TokenType::EndOfFile) {
    return curr;
  }
  return toks_.peek(amnt);
}

N19_FORCEINLINE auto n19::Lexer::_skip_comment() -> void {
  _skip_chars_until([](const char8_t ch) {
    return ch == '\n';
  });
}

N19_FORCEINLINE auto n19::Lexer::_current_char() const -> char8_t {
  return index_ >= src_.size()
    ? u8'\0'
    : src_[index_];
}

N19_FORCEINLINE auto n19::Lexer::_peek_char(const uint32_t amnt) const -> char8_t {
  return (index_ + amnt) >= src_.size()
    ? u8'\0'
    : src_[index_ + amnt];
}

N19_FORCEINLINE auto n19::Lexer::_consume_char(const uint32_t amnt) -> void {
  if(index_ < src_.size()) [[likely]] index_ += amnt;
}

N19_FORCEINLINE auto n19::Lexer::_advance_line() -> void {
  if(index_ < src_.size()) ++line_;
}

N19_FORCEINLINE auto n19::Lexer::_advance_consume_line() -> void {
  if(index_ < src_.size()) ++line_;
  _consume_char(1);
}

N19_FORCEINLINE auto n19::Lexer::get_bytes() const -> Bytes {
  return n19::as_bytes(src_);
}

N19_FORCEINLINE auto n19::Lexer::current() -> Token {
  return toks_.current();
}

N19_FORCEINLINE auto n19::Lexer::produce() -> void {
  toks_.enqueue(_produce_impl());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif //LEXER_HPP
