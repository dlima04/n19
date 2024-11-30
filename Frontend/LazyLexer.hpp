/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef LAZYLEXER_HPP
#define LAZYLEXER_HPP
#include <Core/ClassTraits.hpp>
#include <Core/Platform.hpp>
#include <Core/FileRef.hpp>
#include <Native/String.hpp>
#include <Frontend/LexerBase.hpp>
#include <memory>
#include <vector>

namespace n19 {
  class LazyLexer;
}

class n19::LazyLexer final : public LexerBase {
N19_MAKE_NONCOPYABLE(LazyLexer);
public:
  // overriden base class members.
  auto current() const          -> const Token& override;
  auto consume(uint32_t amnt)   -> const Token& override;
  auto peek(uint32_t amnt)      -> const Token& override;
  auto consume_u(TokenCategory) -> const Token& override;
  auto consume_u(TokenType)     -> const Token& override;
  auto get_bytes() const        -> Bytes override;

  // Error handling methods.
  auto error(const std::string &msg, size_t pos, uint32_t line)   -> void;
  auto warn(const std::string &msg, size_t pos, uint32_t line)    -> void;
  auto expect(TokenCategory cat, const std::string&, bool = true) -> Result<None>;
  auto expect(TokenType type, const std::string&, bool = true)    -> Result<None>;

  static auto create(const FileRef& ref)     -> Result<LazyLexer>;
  static auto create(const std::string& str) -> Result<LazyLexer>;

  ~LazyLexer() override = default;
  LazyLexer() = default;
private:
  // Internal character manipulation
  // methods for consuming/iterating over
  // raw bytes.
  auto _current_char() const   -> char;
  auto _consume_impl()         -> void;
  auto _consume_char(uint32_t) -> void;
  auto _advance_line()         -> void;
  auto _skip_utf8_sequence()   -> void;

  // The token generators.
  // Each one of these manipulates the
  // current token.
  auto _token_skip()       -> void;
  auto _token_newline()    -> void;
  auto _token_hyphen()     -> void;
  auto _token_plus()       -> void;
  auto _token_asterisk()   -> void;
  auto _token_fwdslash()   -> void;
  auto _token_percent()    -> void;
  auto _token_equals()     -> void;
  auto _token_lthan()      -> void;
  auto _token_gthan()      -> void;
  auto _token_ampersand()  -> void;
  auto _token_pipe()       -> void;
  auto _token_bang()       -> void;
  auto _token_uparrow()    -> void;
  auto _token_quote()      -> void;
  auto _token_squote()     -> void;
  auto _token_colon()      -> void;
  auto _token_dot()        -> void;
  auto _token_null()       -> void;
  auto _token_ambiguous()  -> void;
  auto _token_tilde()      -> void;
  auto _token_at()         -> void;
  auto _token_money()      -> void;
  auto _token_lsqbracket() -> void;
  auto _token_rsqbracket() -> void;
  auto _token_semicolon()  -> void;
  auto _token_lparen()     -> void;
  auto _token_rparen()     -> void;
  auto _token_lbrace()     -> void;
  auto _token_rbrace()     -> void;
  auto _token_comma()      -> void;
  auto _token_illegal()    -> void;
  auto _token_hex_lit()    -> void;
  auto _token_num_lit()    -> void;
  auto _token_oct_lit()    -> void;

  auto _token_single_char(TokenType type, TokenCategory cat) -> void;
  auto _peek_char(uint32_t amnt = 1) const -> char;

  Token curr_tok_;
  std::unique_ptr<std::vector<char8_t>> src_;
  native::String file_name_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

N19_FORCEINLINE auto n19::LazyLexer::_consume_char(const uint32_t amnt) -> void {
  if(curr_tok_.type_ == TokenType::EndOfFile || index_ >= src_->size()) {
    return;
  } else [[likely]]
    index_ += amnt;
}

N19_FORCEINLINE auto n19::LazyLexer::_current_char() const -> char {
  return index_ >= src_->size() ? '\0' : src_->at(index_);
}

N19_FORCEINLINE auto n19::LazyLexer::_peek_char(const uint32_t amnt) const -> char {
  return (index_ + amnt) >= src_->size() ? '\0' : src_->at(index_ + amnt);
}

N19_FORCEINLINE auto n19::LazyLexer::_advance_line() -> void {
  if(index_ < src_->size()) ++line_;
}

N19_FORCEINLINE auto n19::LazyLexer::current() const -> const Token& {
  return curr_tok_;
}

#endif //LAZYLEXER_HPP
