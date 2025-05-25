/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef LEXER_HPP
#define LEXER_HPP
#include <Core/Bytes.hpp>
#include <Core/Platform.hpp>
#include <Core/ClassTraits.hpp>
#include <System/File.hpp>
#include <Core/Result.hpp>
#include <Core/Maybe.hpp>
#include <IO/Stream.hpp>
#include <Frontend/Token.hpp>
#include <System/String.hpp>
#include <memory>
#include <vector>
#include <array>
#include <functional>
#include <string_view>
#include <cctype>
#include <cstdint>

#define UTF8_LEADING(CH) (static_cast<uint8_t>(CH) >= 0x80)
#define CH_IS_XDIGIT(CH) (std::isxdigit((uint8_t)CH))
#define CH_IS_CTRL(CH)   (std::iscntrl((uint8_t)CH))
#define CH_IS_SPACE(CH)  (std::isspace((uint8_t)CH))
#define CH_IS_DIGIT(CH)  (std::isdigit((uint8_t)CH))

BEGIN_NAMESPACE(n19);
class Lexer final : public std::enable_shared_from_this<Lexer> {
  N19_MAKE_NONCOPYABLE(Lexer);
  N19_MAKE_COMPARABLE_MEMBER(Lexer, file_name_);
public:
  auto current() const        -> const Token&;
  auto consume(uint32_t amnt) -> const Token&;
  auto get_bytes() const      -> Bytes;
  auto dump(OStream& stream)  -> void;
  auto revert_before(const Token&) -> void;

  template<size_t sz_>
  auto batched_peek()         -> std::array<Token, sz_>;
  auto peek(uint32_t amnt)    -> Token;

  auto reset(sys::File& ref) -> Result<void>;
  auto expect(TokenCategory cat, bool = true)   -> Result<void>;
  auto expect_type(TokenType type, bool = true) -> Result<void>;

  static auto create_shared(sys::File& ref)              -> Result<std::shared_ptr<Lexer>>;
  static auto create_shared(std::vector<char8_t>&& buf)  -> Result<std::shared_ptr<Lexer>>;
  static auto get_keyword(const std::u8string_view& str) -> Maybe<struct Keyword>;
  static auto is_reserved_byte(char8_t c)                -> bool;

  Lexer() = default;
  ~Lexer() = default;
private:
  char8_t current_char_() const;
  void consume_char_(uint32_t);
  void advance_line_();
  void skip_comment_();
  void advance_consume_line_();
  bool skip_chars_until_(std::function<bool(char8_t)> cb);
  bool skip_utf8_sequence_();
  char8_t peek_char_(uint32_t amnt = 1) const;

  auto produce_impl_()    -> Token;
  auto token_hyphen_()    -> Token;
  auto token_plus_()      -> Token;
  auto token_asterisk_()  -> Token;
  auto token_fwdslash_()  -> Token;
  auto token_percent_()   -> Token;
  auto token_equals_()    -> Token;
  auto token_lthan_()     -> Token;
  auto token_null_()      -> Token;
  auto token_tilde_()     -> Token;
  auto token_at_()        -> Token;
  auto token_money_()     -> Token;
  auto token_lsqbrckt_()  -> Token;
  auto token_rsqbrckt_()  -> Token;
  auto token_semicolon_() -> Token;
  auto token_lparen_()    -> Token;
  auto token_rparen_()    -> Token;
  auto token_lbrace_()    -> Token;
  auto token_rbrace_()    -> Token;
  auto token_comma_()     -> Token;
  auto token_gthan_()     -> Token;
  auto token_ampersand_() -> Token;
  auto token_pipe_()      -> Token;
  auto token_bang_()      -> Token;
  auto token_uparrow_()   -> Token;
  auto token_quote_()     -> Token;
  auto token_squote_()    -> Token;
  auto token_colon_()     -> Token;
  auto token_dot_()       -> Token;
  auto token_ambiguous_() -> Token;
  auto token_hex_lit_()   -> Token;
  auto token_num_lit_()   -> Token;
  auto token_oct_lit_()   -> Token;
public:
  std::vector<char8_t> src_;
  Token curr_;
  sys::String file_name_;
  uint32_t index_  = 0;
  uint32_t line_   = 1;
};

struct Keyword {
  TokenType type;
  TokenCategory cat;
};

FORCEINLINE_ auto Lexer::peek(const uint32_t amnt) -> Token {
  const uint32_t line_tmp  = this->line_;
  const size_t   index_tmp = this->index_;
  const Token    tok_tmp   = this->curr_;

  consume(amnt);
  const Token peeked = curr_;

  this->line_  = line_tmp;   /// Restore line
  this->index_ = index_tmp;  /// Restore index
  this->curr_  = tok_tmp;    /// Restore current token
  return peeked;             ///
}

template<size_t sz_>
FORCEINLINE_ auto Lexer::batched_peek() -> std::array<Token, sz_> {
  const uint32_t line_tmp  = this->line_;
  const size_t   index_tmp = this->index_;
  const Token    tok_tmp   = this->curr_;

  std::array<Token, sz_> toks{};
  for(size_t i = 0; i < toks.size(); i++) {
    toks[i] = consume(1);
  }

  this->line_  = line_tmp;   /// Restore line
  this->index_ = index_tmp;  /// Restore index
  this->curr_  = tok_tmp;    /// Restore current token
  return toks;               /// possibly expensive copy
}

inline auto Lexer::revert_before(const Token& tok) -> void {
  this->curr_  = tok;
  this->line_  = tok.line_;
  this->index_ = tok.pos_;
}

inline auto Lexer::skip_comment_() -> void {
  skip_chars_until_([](const char8_t ch) {
    return ch == '\n' || ch == '\0';
  });
}

inline auto Lexer::current_char_() const -> char8_t {
  return index_ >= src_.size()
    ? u8'\0' : src_[index_];
}

inline auto Lexer::peek_char_(const uint32_t amnt) const -> char8_t {
  return (index_ + amnt) >= src_.size()
    ? u8'\0' : src_[index_ + amnt];
}

inline auto Lexer::consume_char_(const uint32_t amnt) -> void {
  if(index_ < src_.size()) index_ += amnt;
}

inline auto Lexer::advance_line_() -> void {
  if(index_ < src_.size()) ++line_;
}

inline auto Lexer::advance_consume_line_() -> void {
  if(index_ < src_.size()) ++line_;
  consume_char_(1);
}

inline auto Lexer::get_bytes() const -> Bytes {
  return as_bytes(src_);
}

inline auto Lexer::current() const -> const Token& {
  return curr_;
}

END_NAMESPACE(n19);
#endif //LEXER_HPP
