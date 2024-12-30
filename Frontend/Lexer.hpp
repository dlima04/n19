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
#include <Core/Result.hpp>
#include <Core/Maybe.hpp>
#include <Frontend/Token.hpp>
#include <Sys/String.hpp>
#include <memory>
#include <vector>
#include <functional>
#include <string_view>
#include <cctype>
#include <cstdint>

#define UTF8_LEADING(CH) (((uint8_t)CH) >= 0x80)
#define CH_IS_XDIGIT(CH) (std::isxdigit((uint8_t)CH))
#define CH_IS_CTRL(CH)   (std::iscntrl((uint8_t)CH))
#define CH_IS_SPACE(CH)  (std::isspace((uint8_t)CH))
#define CH_IS_DIGIT(CH)  (std::isdigit((uint8_t)CH))
BEGIN_NAMESPACE(n19);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// n19::Lexer is the main lexer implementation. It's a "lazy" lexer,
// which means it can easily support different lexing "modes"
// for very context sensitive tokens.

class Lexer final : public std::enable_shared_from_this<Lexer> {
N19_MAKE_NONCOPYABLE(Lexer);
N19_MAKE_COMPARABLE_MEMBER(Lexer, file_name_);
public:
  auto current() const        -> const Token&;
  auto consume(uint32_t amnt) -> const Token&;
  auto get_bytes() const      -> Bytes;
  auto peek(uint32_t amnt)    -> Token;
  auto dump()                 -> void;

  auto expect(TokenCategory cat, bool = true) -> Result<void>;
  auto expect(TokenType type, bool = true)    -> Result<void>;

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Begin static methods.

  static auto create_shared(const FileRef& ref)          -> Result<std::shared_ptr<Lexer>>;
  static auto get_keyword(const std::u8string_view& str) -> Maybe<struct Keyword>;
  static auto is_reserved_byte(char8_t c)                -> bool;

  Lexer() = default;
  ~Lexer() = default;
private:
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Begin internal, character manipulation.

  auto _current_char() const                               -> char8_t;
  auto _consume_char(uint32_t)                             -> void;
  auto _advance_line()                                     -> void;
  auto _skip_comment()                                     -> void;
  auto _advance_consume_line()                             -> void;
  auto _skip_chars_until(std::function<bool(char8_t)> cb)  -> bool;
  auto _skip_utf8_sequence()                               -> bool;
  auto _peek_char(uint32_t amnt = 1) const                 -> char8_t;

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Begin token generation methods.

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

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Begin fields.
public:
  std::vector<char8_t> src_;  /// Source file buffer.
  Token curr_;                /// The one we're sitting on.
  sys::String file_name_;     /// For error handling.
  uint32_t index_  = 0;       /// Current source index.
  uint32_t line_   = 1;       /// current line number.
};

struct Keyword {              /// Only used for Lexer::get_keyword().
  TokenType type;             /// The TokenType of the keyword.
  TokenCategory cat;          /// The category of the keyword.
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// very short functions, inlined here for perf.

inline auto Lexer::peek(const uint32_t amnt) -> Token {
  const uint32_t line_tmp  = this->line_;
  const size_t   index_tmp = this->index_;
  const Token    tok_tmp   = this->curr_;

  consume(amnt);
  const Token peeked = curr_;

  line_  = line_tmp;          /// Restore line
  index_ = index_tmp;         /// Restore index
  curr_  = tok_tmp;           /// Restore current token
  return peeked;
}

inline auto Lexer::_skip_comment() -> void {
  _skip_chars_until([](const char8_t ch) {
    return ch == '\n' || ch == '\0';
  });
}

inline auto Lexer::_current_char() const -> char8_t {
  return index_ >= src_.size()
    ? u8'\0'
    : src_[index_];
}

inline auto Lexer::_peek_char(const uint32_t amnt) const -> char8_t {
  return (index_ + amnt) >= src_.size()
    ? u8'\0'
    : src_[index_ + amnt];
}

inline auto Lexer::_consume_char(const uint32_t amnt) -> void {
  if(index_ < src_.size()) index_ += amnt;
}

inline auto Lexer::_advance_line() -> void {
  if(index_ < src_.size()) ++line_;
}

inline auto Lexer::_advance_consume_line() -> void {
  if(index_ < src_.size()) ++line_;
  _consume_char(1);
}

inline auto Lexer::get_bytes() const -> Bytes {
  return as_bytes(src_);
}

inline auto Lexer::current() const -> const Token& {
  return curr_;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

END_NAMESPACE(n19);
#endif //LEXER_HPP
