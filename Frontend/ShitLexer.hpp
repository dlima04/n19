/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef LEXER_HPP
#define LEXER_HPP
#include <Core/FileRef.hpp>
#include <Core/Result.hpp>
#include <Frontend/Token.hpp>
#include <cstdint>
#include <vector>

namespace n19 {
  class Lexer;
}

class n19::Lexer {
public:
  auto current() const        -> const Token&;
  auto advance(uint32_t amnt) -> Lexer&;
  auto peek(uint32_t amnt)    -> Token;

  //
  // Error handling. Raise an error or warning at a
  // specific position in the file buffer owned by the Lexer.
  //
  auto error(const std::string &msg, size_t pos, uint32_t line) -> Lexer&;
  auto warn(const std::string &msg, size_t pos, uint32_t line)  -> Lexer&;
  auto error(const std::string &msg) -> Lexer&;
  auto warn(const std::string &msg)  -> Lexer&;

  //
  // Throws an assertion if the current token is not the
  // one passed as a parameter here.
  //
  auto expect(TokenCategory cat, const std::string&, bool = true)  -> Result<None>;
  auto expect(TokenType type, const std::string&, bool = true)     -> Result<None>;

  //
  // Used to create the Lexer.
  // No public constructor is available.
  //
  static auto create(const FileRef& file)      -> Result<Lexer>;
  static auto create(const std::string& file)  -> Result<Lexer>;
private:
  Lexer() = default;

  //
  // For checking/manipulating characters.
  // Used internally by the lexer.
  //
  [[nodiscard]] auto _curr_char_is_reserved()      const -> bool;
  [[nodiscard]] auto _at_utf8_begin()              const -> bool;
  [[nodiscard]] auto _peek_char(uint32_t amnt = 1) const -> char;
  [[nodiscard]] auto _current_char()               const -> char;

  //
  // For safely iterating over the character stream.
  // Uses bounds checking to prevent OOB read/write.
  //
  auto _advance_impl()          -> void;
  auto _advance_char(uint32_t)  -> void;
  auto _advance_line()          -> void;
  auto _skip_utf8_sequence()    -> void;

  //
  // The token generators.
  // Each one of these manipulates the
  // current token.
  //
  auto _token_skip()            -> void;
  auto _token_newline()         -> void;
  auto _token_hyphen()          -> void;
  auto _token_plus()            -> void;
  auto _token_asterisk()        -> void;
  auto _token_fwdslash()        -> void;
  auto _token_percent()         -> void;
  auto _token_equals()          -> void;
  auto _token_lessthan()        -> void;
  auto _token_greaterthan()     -> void;
  auto _token_ampersand()       -> void;
  auto _token_verticalline()    -> void;
  auto _token_bang()            -> void;
  auto _token_uparrow()         -> void;
  auto _token_quote()           -> void;
  auto _token_singlequote()     -> void;
  auto _token_colon()           -> void;
  auto _token_dot()             -> void;
  auto _token_null()            -> void;
  auto _token_ambiguous()       -> void;
  auto _token_tilde()           -> void;
  auto _token_at()              -> void;
  auto _token_money()           -> void;
  auto _token_lsqbracket()      -> void;
  auto _token_rsqbracket()      -> void;
  auto _token_semicolon()       -> void;
  auto _token_lparen()          -> void;
  auto _token_rparen()          -> void;
  auto _token_lbrace()          -> void;
  auto _token_rbrace()          -> void;
  auto _token_comma()           -> void;
  auto _token_illegal()         -> void;

  //
  // For creating numeric literals:
  // - octal
  // - hex
  // - integer or floating point
  //
  auto _token_hex_literal()      -> Token;
  auto _token_numeric_literal()  -> Token;
  auto _token_octal_literal()    -> Token;

  //
  // For creating single character/unambiguous tokens.
  //
  auto _token_single_char(TokenType type, TokenCategory cat) -> void;

  //
  // The private members of a n19::Lexer.
  // We use a unique_ptr for the buffer here,
  // to make it easier to transfer byte buffers
  // into the Lexer from elswehere.
  //
  std::unique_ptr< std::vector<char> > src_;
  Token curr_tok_;
  std::string file_name_;
  size_t index_  = 0;
  uint32_t line_ = 1;
};

#endif //LEXER_HPP
