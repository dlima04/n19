/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Core/Try.hpp>
#include <Core/Murmur3.hpp>
#include <Frontend/Lexer.hpp>
#include <Frontend/ErrorCollector.hpp>
#include <Frontend/Keywords.hpp>
#include <filesystem>
#include <algorithm>
#include <limits>
#include <functional>
#include <array>
BEGIN_NAMESPACE(n19);

inline auto Lexer::token_hyphen_() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.line_ = line_;

  switch(peek_char_()) {
  case u8'=': // '-='
    curr_tok.type_ = TokenType::SubEq;
    curr_tok.cat_  = TokenCategory::BinaryOp;
    curr_tok.cat_ |= TokenCategory::ArithAssignOp;
    curr_tok.cat_ |= TokenCategory::PointerArithOp;
    curr_tok.cat_ |= TokenCategory::ArithmeticOp;
    curr_tok.len_  = 2;
    consume_char_(2);
    break;
  case u8'-': // '--'
    curr_tok.type_ = TokenType::Dec;
    curr_tok.cat_  = TokenCategory::UnaryOp;
    curr_tok.cat_ |= TokenCategory::PointerArithOp;
    curr_tok.cat_ |= TokenCategory::ValidPostfix;
    curr_tok.cat_ |= TokenCategory::ValidPrefix;
    curr_tok.cat_ |= TokenCategory::ArithAssignOp;
    curr_tok.cat_ |= TokenCategory::ArithmeticOp;
    curr_tok.len_  = 2;
    consume_char_(2);
    break;
  case u8'>': // '->'
    curr_tok.type_ = TokenType::SkinnyArrow;
    curr_tok.cat_  = TokenCategory::ValidPostfix;
    curr_tok.cat_ |= TokenCategory::BinaryOp;
    curr_tok.len_  = 2;
    consume_char_(2);
  default: // '-'
    curr_tok.type_ = TokenType::Sub;
    curr_tok.cat_  = TokenCategory::BinaryOp;
    curr_tok.cat_ |= TokenCategory::ArithmeticOp;
    curr_tok.cat_ |= TokenCategory::PointerArithOp;
    curr_tok.len_  = 1;
    consume_char_(1);
    break;
  }

  return curr_tok;
}

inline auto Lexer::token_ampersand_() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.line_ = line_;
  curr_tok.cat_  = TokenCategory::BinaryOp;

  switch(peek_char_()) {
  case u8'=': // '&='
    curr_tok.type_ = TokenType::BitwiseAndEq;
    curr_tok.cat_ |= TokenCategory::BitwiseAssignOp;
    curr_tok.cat_ |= TokenCategory::BitwiseOp;
    curr_tok.len_  = 2;
    consume_char_(2);
    break;
  case u8'&': // '&&'
    curr_tok.type_ = TokenType::LogicalAnd;
    curr_tok.cat_ |= TokenCategory::LogicalOp;
    curr_tok.len_  = 2;
    consume_char_(2);
    break;
  default: // '&'
    curr_tok.type_  = TokenType::BitwiseAnd;
    curr_tok.cat_  |= TokenCategory::BitwiseOp;
    curr_tok.cat_  |= TokenCategory::UnaryOp;
    curr_tok.cat_  |= TokenCategory::ValidPrefix;
    curr_tok.len_  = 1;
    consume_char_(1);
    break;
  }

  return curr_tok;
}

inline auto Lexer::token_dot_() -> Token {
  Token curr_tok;
  curr_tok.line_ = line_;
  curr_tok.cat_  = TokenCategory::NonCategorical;
  curr_tok.pos_  = index_;

  if(peek_char_(1) == u8'.' && peek_char_(2) == u8'.') { // '...'
    curr_tok.type_ = TokenType::DotThree;
    curr_tok.len_  = 3;
    consume_char_(3);
  } else if(peek_char_(1) == u8'.') { // '..'
    curr_tok.type_ = TokenType::DotTwo;
    curr_tok.len_  = 2;
    consume_char_(2);
  } else { // '.'
    curr_tok.type_ = TokenType::Dot;
    curr_tok.cat_  = TokenCategory::ValidPostfix | TokenCategory::BinaryOp;
    curr_tok.len_  = 1;
    consume_char_(1);
  }

  return curr_tok;
}

inline auto Lexer::token_equals_() -> Token {
  Token curr_tok;
  curr_tok.line_ = line_;
  curr_tok.pos_ = index_;

  switch(peek_char_()) {
  case '=': // '=='
    curr_tok.type_ = TokenType::Eq;
    curr_tok.cat_  = TokenCategory::BinaryOp;
    curr_tok.cat_ |= TokenCategory::LogicalOp;
    curr_tok.cat_ |= TokenCategory::ComparisonOp;
    curr_tok.len_  = 2;
    consume_char_(2);
    break;
  case '>': // '=>'
    curr_tok.type_ = TokenType::FatArrow;
    curr_tok.cat_  = TokenCategory::NonCategorical;
    curr_tok.len_  = 2;
    consume_char_(2);
    break;
  default: // '='
    curr_tok.type_ = TokenType::ValueAssignment;
    curr_tok.cat_  = TokenCategory::BinaryOp;
    curr_tok.len_  = 1;
    consume_char_(1);
    break;
  }

  return curr_tok;
}

inline auto Lexer::token_plus_() -> Token {
  Token curr_tok;
  curr_tok.line_ = line_;
  curr_tok.pos_ = index_;

  switch(peek_char_()) {
  case '=': // '+='
    curr_tok.type_ = TokenType::PlusEq;
    curr_tok.cat_  = TokenCategory::BinaryOp;
    curr_tok.cat_ |= TokenCategory::ArithAssignOp;
    curr_tok.cat_ |= TokenCategory::ArithmeticOp;
    curr_tok.cat_ |= TokenCategory::PointerArithOp;
    curr_tok.len_  = 2;
    consume_char_(2);
    break;
  case '+': // '++'
    curr_tok.type_ = TokenType::Inc;
    curr_tok.cat_  = TokenCategory::UnaryOp;
    curr_tok.cat_ |= TokenCategory::ArithAssignOp;
    curr_tok.cat_ |= TokenCategory::ArithmeticOp;
    curr_tok.cat_ |= TokenCategory::PointerArithOp;
    curr_tok.cat_ |= TokenCategory::ValidPostfix;
    curr_tok.cat_ |= TokenCategory::ValidPrefix;
    curr_tok.len_  = 2;
    consume_char_(2);
    break;
  default: // '+'
    curr_tok.type_ = TokenType::Plus;
    curr_tok.cat_  = TokenCategory::BinaryOp;
    curr_tok.cat_ |= TokenCategory::UnaryOp;
    curr_tok.cat_ |= TokenCategory::ArithmeticOp;
    curr_tok.cat_ |= TokenCategory::PointerArithOp;
    curr_tok.cat_ |= TokenCategory::ValidPrefix;
    curr_tok.len_  = 1;
    consume_char_(1);
    break;
  }

  return curr_tok;
}

inline auto Lexer::token_gthan_() -> Token {
  Token curr_tok;
  curr_tok.cat_  = TokenCategory::BinaryOp;
  curr_tok.line_ = line_;
  curr_tok.pos_  = index_;

  if(peek_char_(1) == '=') { // '>='
    curr_tok.type_ = TokenType::Gte;
    curr_tok.cat_ |= TokenCategory::LogicalOp | TokenCategory::ComparisonOp;
    curr_tok.len_  = 2;
    consume_char_(2);
  } else if(peek_char_(1) == '>' && peek_char_(2) == '=') { // '>>='
    curr_tok.type_ = TokenType::RshiftEq;
    curr_tok.cat_ |= TokenCategory::BitwiseOp | TokenCategory::BitwiseAssignOp;
    curr_tok.len_  = 3;
    consume_char_(3);
  } else if(peek_char_(1) == '>') { // '>>'
    curr_tok.type_ = TokenType::Rshift;
    curr_tok.cat_ |= TokenCategory::BitwiseOp;
    curr_tok.len_  = 2;
    consume_char_(2);
  } else { // '>'
    curr_tok.type_ = TokenType::Gt;
    curr_tok.cat_ |= TokenCategory::LogicalOp | TokenCategory::ComparisonOp;
    curr_tok.len_  = 1;
    consume_char_(1);
  }

  return curr_tok;
}

inline auto Lexer::token_squote_() -> Token {
  Token curr_tok;
  curr_tok.line_ = line_;
  curr_tok.pos_  = index_;

  //
  // Check what's INSIDE the quotes first.
  // TODO: not sure if this code is fully correct...
  //

  consume_char_(1);
  if(current_char_() == '\\') {
    consume_char_(1);
    if(UTF8_LEADING(current_char_())) {
      curr_tok.len_  = index_ - curr_tok.pos_;
      curr_tok.type_ = TokenType::Illegal;
      return curr_tok;
    }
    consume_char_(1);
  } else if(current_char_() == '\'') {
    consume_char_(1);
    curr_tok.type_ = TokenType::ByteLiteral;
    curr_tok.cat_  = TokenCategory::Literal;
    curr_tok.len_  = index_ - curr_tok.pos_;
    return curr_tok;
  } else if(UTF8_LEADING(current_char_()) || current_char_() == '\n') {
    curr_tok.type_ = TokenType::Illegal;
    curr_tok.cat_  = TokenCategory::NonCategorical;
    curr_tok.len_  = index_ - curr_tok.pos_;
    return curr_tok;
  } else {
    consume_char_(1);
  }

  //
  // Ensure the following character is a closing quote, token == illegal otherwise.
  //

  if(current_char_() == '\0') {
    curr_tok = Token::eof(src_.size() - 1, line_);
  } else if(current_char_() == '\'') [[likely]] {
    consume_char_(1);
    curr_tok.type_ = TokenType::ByteLiteral;
    curr_tok.cat_  = TokenCategory::Literal;
    curr_tok.len_  = index_ - curr_tok.pos_;
  } else {
    curr_tok.type_ = TokenType::Illegal;
    curr_tok.cat_  = TokenCategory::NonCategorical;
    curr_tok.len_  = index_ - curr_tok.pos_;
  }

  return curr_tok;
}

inline auto Lexer::token_asterisk_() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;

  if(peek_char_() == u8'=') { // '*='
    curr_tok.type_ = TokenType::MulEq;
    curr_tok.cat_  = TokenCategory::BinaryOp;
    curr_tok.cat_ |= TokenCategory::ArithAssignOp;
    curr_tok.cat_ |= TokenCategory::ArithmeticOp;
    curr_tok.len_ = 2;
    consume_char_(2);
  } else [[likely]] { // '*'
    curr_tok.type_ = TokenType::Mul;
    curr_tok.cat_  = TokenCategory::BinaryOp;
    curr_tok.cat_ |= TokenCategory::UnaryOp;
    curr_tok.cat_ |= TokenCategory::ArithmeticOp;
    curr_tok.cat_ |= TokenCategory::ValidPrefix;
    consume_char_(1);
  }

  return curr_tok;
}

inline auto Lexer::token_colon_() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.line_ = line_;

  if(peek_char_() == u8':') { // '::'
    curr_tok.type_  = TokenType::NamespaceOperator;
    curr_tok.cat_   = TokenCategory::ValidPrefix;
    curr_tok.len_   = 2;
    consume_char_(2);
  } else { // ':'
    curr_tok.type_  = TokenType::TypeAssignment;
    curr_tok.cat_   = TokenCategory::NonCategorical;
    curr_tok.len_   = 1;
    consume_char_(1);
  }

  return curr_tok;
}

auto Lexer::token_bang_() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.line_ = line_;

  if(peek_char_() == u8'=') { // '!='
    curr_tok.type_ = TokenType::Neq;
    curr_tok.cat_  = TokenCategory::BinaryOp;
    curr_tok.cat_ |= TokenCategory::LogicalOp;
    curr_tok.cat_ |= TokenCategory::ComparisonOp;
    curr_tok.len_  = 2;
    consume_char_(2);
  } else { // '!'
    curr_tok.type_ = TokenType::LogicalNot;
    curr_tok.cat_  = TokenCategory::UnaryOp;
    curr_tok.cat_ |= TokenCategory::ValidPrefix;
    curr_tok.cat_ |= TokenCategory::LogicalOp;
    curr_tok.len_  = 1;
    consume_char_(1);
  }

  return curr_tok;
}

inline auto Lexer::token_percent_() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.line_ = line_;
  curr_tok.cat_  = TokenCategory::BinaryOp | TokenCategory::ArithmeticOp;

  if(peek_char_() == u8'=') { // '%='
    curr_tok.type_ = TokenType::ModEq;
    curr_tok.cat_ |= TokenCategory::ArithAssignOp;
    curr_tok.len_  = 2;
    consume_char_(2);
  } else { // '%'
    curr_tok.type_ = TokenType::Mod;
    curr_tok.len_  = 1;
    consume_char_(1);
  }

  return curr_tok;
}

inline auto Lexer::token_uparrow_() -> Token {
  Token curr_tok;
  curr_tok.cat_  = TokenCategory::BinaryOp | TokenCategory::BitwiseOp;
  curr_tok.pos_  = index_;
  curr_tok.line_ = line_;

  if(peek_char_() == u8'=') { // '^='
    curr_tok.type_ = TokenType::XorEq;
    curr_tok.cat_ |= TokenCategory::BitwiseAssignOp;
    curr_tok.len_  = 2;
    consume_char_(2);
  } else { // '^'
    curr_tok.type_ = TokenType::Xor;
    curr_tok.len_  = 1;
    consume_char_(1);
  }

  return curr_tok;
}

inline auto Lexer::token_pipe_() -> Token {
  Token curr_tok {
    .pos_  = index_,
    .len_  = 2,
    .line_ = line_,
    .cat_  = TokenCategory::BinaryOp
  };

  switch(peek_char_()) {
  case u8'|': // '||'
    curr_tok.type_ = TokenType::LogicalOr;
    curr_tok.cat_ |= TokenCategory::LogicalOp;
    consume_char_(2);
    break;
  case u8'=': // '|='
    curr_tok.type_  = TokenType::BitwiseOrEq;
    curr_tok.cat_  |= TokenCategory::BitwiseOp | TokenCategory::BitwiseAssignOp;
    consume_char_(2);
    break;
  default: // '|'
    curr_tok.type_ = TokenType::BitwiseOr;
    curr_tok.cat_  = TokenCategory::BitwiseOp;
    curr_tok.len_  = 1;
    consume_char_(1);
    break;
  }

  return curr_tok;
}

inline auto Lexer::token_lthan_() -> Token {
  Token curr_tok {
    .pos_  = index_,
    .line_ = line_,
    .cat_  = TokenCategory::BinaryOp
  };

  if(peek_char_(1) == u8'=') { // '<='
    curr_tok.type_ = TokenType::Lte;
    curr_tok.cat_ |= TokenCategory::LogicalOp | TokenCategory::ComparisonOp;
    curr_tok.len_  = 2;
    consume_char_(2);
  } else if(peek_char_(1) == u8'<' && peek_char_(2) == u8'=') [[unlikely]] { // <<=
    curr_tok.type_ = TokenType::LshiftEq;
    curr_tok.cat_ |= TokenCategory::BitwiseOp | TokenCategory::BitwiseAssignOp;
    curr_tok.len_  = 3;
    consume_char_(3);
  } else if(peek_char_(1) == u8'<') { // '<<'
    curr_tok.type_ = TokenType::Lshift;
    curr_tok.cat_ |= TokenCategory::BitwiseOp;
    curr_tok.len_  = 2;
    consume_char_(2);
  } else { // '<'
    curr_tok.type_ = TokenType::Lt;
    curr_tok.cat_ |= TokenCategory::LogicalOp | TokenCategory::ComparisonOp;
    curr_tok.len_  = 1;
    consume_char_(1);
  }

  return curr_tok;
}

FORCEINLINE_ auto Lexer::token_null_()  -> Token {
  Token curr_tok;
  curr_tok.type_ = TokenType::EndOfFile;
  curr_tok.cat_  = TokenCategory::NonCategorical;
  curr_tok.len_  = 0;
  curr_tok.pos_  = src_.size() - 1;
  return curr_tok;
}

FORCEINLINE_ auto Lexer::token_tilde_() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::BitwiseNot;
  curr_tok.cat_  = TokenCategory::UnaryOp | TokenCategory::BitwiseOp | TokenCategory::ValidPrefix;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  consume_char_(1);
  return curr_tok;
}

FORCEINLINE_ auto Lexer::token_at_() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::At;
  curr_tok.cat_  = TokenCategory::NonCategorical;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  consume_char_(1);
  return curr_tok;
}

FORCEINLINE_ auto Lexer::token_money_() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::Money;
  curr_tok.cat_  = TokenCategory::NonCategorical;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  consume_char_(1);
  return curr_tok;
}

FORCEINLINE_ auto Lexer::token_lsqbrckt_() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::LeftSqBracket;
  curr_tok.cat_  = TokenCategory::Punctuator | TokenCategory::ValidPostfix;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  consume_char_(1);
  return curr_tok;
}

FORCEINLINE_ auto Lexer::token_rsqbrckt_() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::RightSqBracket;
  curr_tok.cat_  = TokenCategory::Punctuator;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  consume_char_(1);
  return curr_tok;
}

FORCEINLINE_ auto Lexer::token_semicolon_() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::Semicolon;
  curr_tok.cat_  = TokenCategory::Punctuator;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  consume_char_(1);
  return curr_tok;
}

FORCEINLINE_ auto Lexer::token_lparen_() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::LeftParen;
  curr_tok.cat_  = TokenCategory::Punctuator | TokenCategory::ValidPostfix;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  consume_char_(1);
  return curr_tok;
}

FORCEINLINE_ auto Lexer::token_rparen_() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::RightParen;
  curr_tok.cat_  = TokenCategory::Punctuator;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  consume_char_(1);
  return curr_tok;
}

FORCEINLINE_ auto Lexer::token_lbrace_() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::LeftBrace;
  curr_tok.cat_  = TokenCategory::Punctuator;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  consume_char_(1);
  return curr_tok;
}

FORCEINLINE_ auto Lexer::token_rbrace_() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::RightBrace;
  curr_tok.cat_  = TokenCategory::Punctuator;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  consume_char_(1);
  return curr_tok;
}

FORCEINLINE_ auto Lexer::token_comma_() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::Comma;
  curr_tok.cat_  = TokenCategory::Punctuator | TokenCategory::Terminator;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  consume_char_(1);
  return curr_tok;
}

auto Lexer::produce_impl_() -> Token {
  if(index_ >= src_.size()) {
    return Token::eof(src_.size() - 1, line_);
  }

SWITCH_BEGIN:
  switch(current_char_()) {
    case u8'\\' : FALLTHROUGH_; // illegal character! fallthrough.
    case u8'?'  : return Token::illegal(index_, 1, line_);
    case u8'#'  : skip_comment_();         goto SWITCH_BEGIN;
    case u8'\n' : advance_consume_line_(); goto SWITCH_BEGIN;
    case u8' '  : FALLTHROUGH_; // skip character.
    case u8'\r' : FALLTHROUGH_; // skip character.
    case u8'\b' : FALLTHROUGH_; // skip character.
    case u8'\a' : FALLTHROUGH_; // skip character.
    case u8'\v' : FALLTHROUGH_; // skip character.
    case u8'\t' : consume_char_(1);        goto SWITCH_BEGIN;
    case u8'/'  : return token_fwdslash_();
    case u8'\0' : return token_null_();
    case u8'~'  : return token_tilde_();
    case u8'@'  : return token_at_();
    case u8'$'  : return token_money_();
    case u8'['  : return token_lsqbrckt_();
    case u8']'  : return token_rsqbrckt_();
    case u8';'  : return token_semicolon_();
    case u8'('  : return token_lparen_();
    case u8')'  : return token_rparen_();
    case u8'{'  : return token_lbrace_();
    case u8'}'  : return token_rbrace_();
    case u8','  : return token_comma_();
    case u8'-'  : return token_hyphen_();
    case u8'&'  : return token_ampersand_();
    case u8'.'  : return token_dot_();
    case u8'!'  : return token_bang_();
    case u8'<'  : return token_lthan_();
    case u8'>'  : return token_gthan_();
    case u8'+'  : return token_plus_();
    case u8'*'  : return token_asterisk_();
    case u8'%'  : return token_percent_();
    case u8'='  : return token_equals_();
    case u8'|'  : return token_pipe_();
    case u8'^'  : return token_uparrow_();
    case u8'\'' : return token_squote_();
    case u8'"'  : return token_quote_();
    case u8'`'  : return token_quote_();
    case u8':'  : return token_colon_();
    default     : return token_ambiguous_();
  }

  UNREACHABLE_ASSERTION; // assertion
}

inline auto Lexer::token_fwdslash_() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.line_ = line_;

  if(peek_char_() == u8'=') { // '/='
    curr_tok.type_ = TokenType::DivEq;
    curr_tok.cat_  = TokenCategory::BinaryOp;
    curr_tok.cat_ |= TokenCategory::ArithAssignOp;
    curr_tok.cat_ |= TokenCategory::ArithmeticOp;
    curr_tok.len_  = 2;
    consume_char_(2);
  } else { // '/'
    curr_tok.type_ = TokenType::Div;
    curr_tok.cat_  = TokenCategory::BinaryOp;
    curr_tok.cat_ |= TokenCategory::UnaryOp;
    curr_tok.cat_ |= TokenCategory::ArithmeticOp;
    curr_tok.cat_ |= TokenCategory::ValidPrefix;
    curr_tok.len_  = 1;
    consume_char_(1);
  }

  return curr_tok;
}

inline auto Lexer::token_quote_() -> Token {
  Token curr_tok;
  const auto opening_quote = current_char_();
  const auto string_start  = index_;

  consume_char_(1);
  while(true) {
    if(current_char_() == '\0' || current_char_() == '\n') {
      curr_tok.type_ = TokenType::Illegal;
      curr_tok.cat_  = TokenCategory::NonCategorical;
      curr_tok.pos_  = string_start;
      break;
    } if(current_char_() == opening_quote) {
      consume_char_(1);
      curr_tok.type_ = TokenType::StringLiteral;
      curr_tok.cat_  = TokenCategory::Literal;
      curr_tok.pos_  = string_start;
      curr_tok.len_  = index_ - string_start;
      break;
    }

    /// skip escaped quote.
    if(current_char_() == '\\' && peek_char_() == opening_quote) {
      consume_char_(2);
    } else if(UTF8_LEADING(current_char_())) {
      skip_utf8_sequence_();
    } else {
      consume_char_(1);
    }
  }

  return curr_tok;
}

inline auto Lexer::token_hex_lit_() -> Token {
  ASSERT(current_char_() == '0');
  ASSERT(peek_char_() == 'x' || peek_char_() == 'X');

  const auto start = index_;
  consume_char_(2); // move past "0x"
  skip_chars_until_([](const char8_t ch) {
    return !CH_IS_XDIGIT(ch);
  });

  Token token;
  token.pos_  = start;
  token.line_ = line_;
  token.len_  = index_ - start;

  if(index_ - start < 3) {
    token.type_ = TokenType::Illegal;
    token.cat_  = TokenCategory::NonCategorical;
  } else {
    token.type_ = TokenType::HexLiteral;
    token.cat_  = TokenCategory::Literal;
  }

  return token;
}

inline auto Lexer::token_num_lit_() -> Token {
  ASSERT( CH_IS_DIGIT(current_char_()) );
  const size_t start = index_;
  bool seen_exponent = false;
  bool seen_dot      = false;

  Token curr_tok;
  curr_tok.line_ = line_;
  curr_tok.pos_  = start;
  curr_tok.cat_  = TokenCategory::Literal;

  while(true) {
    const char8_t curr = current_char_();
    const char8_t next = peek_char_();

    /// Switch to parsing the decimal portion now.
    /// Time to parse it out!!! :)
    if(curr == u8'.') {
      if(!CH_IS_DIGIT(next)) {
        break;
      } if(seen_dot || seen_exponent) {
        curr_tok.type_ = TokenType::Illegal;
        curr_tok.cat_  = TokenCategory::NonCategorical;
        return curr_tok;
      }

      seen_dot = true;
    }

    /// We've reached the exponent, then.
    /// Time to parse it out!!! :) The fog is coming!! :))
    else if(curr == u8'e' || curr == u8'E') {
      if(seen_exponent) {
        curr_tok.type_ = TokenType::Illegal;
        curr_tok.cat_  = TokenCategory::NonCategorical;
        return curr_tok;
      } if((next == u8'-' || next == u8'+') && !CH_IS_DIGIT(peek_char_(2))) {
        curr_tok.type_ = TokenType::Illegal;
        curr_tok.cat_  = TokenCategory::NonCategorical;
        return curr_tok;
      } if((next == u8'-' || next == u8'+') && CH_IS_DIGIT(peek_char_(2))) {
        consume_char_(1);
      }

      seen_exponent = true;
    }

    /// If we don't see '.' or 'e' and it isn't a digit,
    /// we've finished parsing this integer sequence.
    else if(!CH_IS_DIGIT(current_char_())) {
      break;
    }

    /// Otherwise just consume the current character.
    consume_char_(1);
  }

  if(seen_dot || seen_exponent) {
    curr_tok.type_ = TokenType::FloatLiteral;
    curr_tok.len_  = index_ - start;
  } else {
    curr_tok.type_ = TokenType::IntLiteral;
    curr_tok.len_  = index_ - start;
  }

  return curr_tok;
}

inline auto Lexer::token_oct_lit_() -> Token {
  ASSERT(current_char_() == '0');
  ASSERT(CH_IS_DIGIT(peek_char_()));

  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.line_ = line_;
  curr_tok.type_ = TokenType::OctalLiteral;
  curr_tok.cat_  = TokenCategory::Literal;

  auto is_octal_digit = [](const char c) -> bool {
    return c >= '0' && c <= '7';
  };

  while(true) {
    if(!CH_IS_DIGIT(current_char_())) {
      break;
    } if(!is_octal_digit(current_char_())) {
      curr_tok.type_ = TokenType::Illegal;
      curr_tok.cat_  = TokenCategory::NonCategorical;
      return curr_tok;
    }
    consume_char_(1);
  }

  curr_tok.len_ = index_ - curr_tok.pos_;
  return curr_tok;
}

inline auto Lexer::token_ambiguous_() -> Token {
  ASSERT(index_ < src_.size());
  ASSERT(!is_reserved_byte(current_char_()));

  const char8_t next = peek_char_();
  const char8_t curr = current_char_();

  if(curr == u8'0' && (next == u8'x' || next == u8'X')) {
    return token_hex_lit_();
  } if(curr == u8'0' && CH_IS_DIGIT( next )) {
    return token_oct_lit_();
  } if(CH_IS_DIGIT( curr )) {
    return token_num_lit_();
  }

  const auto start = index_;
  skip_chars_until_([](const char8_t ch) {
    return CH_IS_SPACE(ch) || CH_IS_CTRL(ch) || is_reserved_byte(ch);
  });

  Token curr_tok;
  curr_tok.pos_  = start;
  curr_tok.len_  = index_ - start;
  curr_tok.line_ = line_;

  const std::u8string_view val = {&src_[start], index_ - start};
  const auto keyword = Lexer::get_keyword(val);

  if(keyword.has_value()) {
    curr_tok.type_ = keyword->type;
    curr_tok.cat_  = keyword->cat;
  } else {
    curr_tok.type_ = TokenType::Identifier;
    curr_tok.cat_  = TokenCategory::Identifier;
  }

  return curr_tok;
}

inline auto Lexer::get_keyword(const std::u8string_view& str) -> Maybe<Keyword> {
  if(str.size() > 15) return Nothing;
  auto tok_cat  = TokenCategory::from_keyword(str);       /// Try and get the category
  auto tok_type = TokenType::from_keyword(str);           /// Try and get the type
  if(!tok_cat || !tok_type) return Nothing;               /// Verify they actually exist
  return Keyword{.type = *tok_type, .cat = *tok_cat};
}

inline auto Lexer::skip_chars_until_(std::function<bool(char8_t)> cb) -> bool {
  while(!cb(current_char_()) && current_char_() != '\0') {
    const char8_t curr = current_char_();

    if(curr == '\n')            advance_consume_line_();  /// line feed
    else if(UTF8_LEADING(curr)) skip_utf8_sequence_();    /// UTF8 codepoint begin
    else consume_char_(1);                                /// ASCII character
  }

  return current_char_() != '\0';
}

auto Lexer::is_reserved_byte(const char8_t c) -> bool {
  constexpr static auto bytes = []() consteval -> auto {
    constexpr char8_t chars[] = {
      u8';', u8'(',  u8')',  u8'{', u8'}', u8',', u8'-', u8'+', u8'*',
      u8'/', u8'%',  u8'=',  u8'<', u8'>', u8'&', u8'|', u8'!', u8'~',
      u8'^', u8'\'', u8'"',  u8'`', u8'[', u8']', u8'?', u8':', u8'#',
      u8'@', u8'.',  u8'\\', u8'$', u8'\0'
    };

    std::array<bool, 256> table = { false };
    for(const char8_t ch : chars) { table[ch] = true; }
    return table;
  }();

  return bytes[ c ] == true;
}

inline auto Lexer::skip_utf8_sequence_() -> bool {
  const auto ch = static_cast<uint8_t>(current_char_());

  if ((ch & 0xE0) == 0xC0)      consume_char_(2); /// 2 byte codepoint.
  else if ((ch & 0xF0) == 0xE0) consume_char_(3); /// 3 byte codepoint.
  else if ((ch & 0xF8) == 0xF0) consume_char_(4); /// 4 byte codepoint.
  else return false;                              /// Something's wrong?

  return index_ - 1 < src_.size();
}

auto Lexer::create_shared(const sys::File& ref) -> Result<std::shared_ptr<Lexer>> {
  ref.seek(0, sys::FSeek::Beg);
  const auto fsize = TRY(ref.size());

  /// Check against maximum allowed file size.
  /// No way this is ever true tbh.
  if(fsize >= std::numeric_limits<uint32_t>::max()) {
    return Error(ErrC::InvalidArg, "File is too large");
  }

  /// Check for an empty file.
  /// TODO: should we really error on this? I don't know if
  /// this even makes sense.
  if(fsize == 0) {
    return Error(ErrC::InvalidArg, "File is empty.");
  }

  auto lxr = std::make_shared<Lexer>();
  lxr->file_name_ = std::filesystem::absolute(ref.name_).string();
  lxr->src_.resize(fsize);

  auto wbytes = as_writable_bytes(lxr->src_);
  TRY(ref.read_into(wbytes));
  lxr->curr_ = lxr->produce_impl_();
  return lxr;
}

auto Lexer::expect(const TokenCategory cat, const bool cons) -> Result<void> {
  if(current().cat_ != cat) {
    const auto errc = ErrC::BadToken;
    const auto msg  = fmt("Expected token of kind \"{}\".", cat.to_string());
    return Error(errc, msg);
  }

  if(cons) consume(1);
  return Result<void>::create();
}

auto Lexer::expect(const TokenType type, const bool cons) -> Result<void> {
  if(current().type_ != type) {
    const auto errc = ErrC::BadToken;
    const auto msg  = fmt("Expected token \"{}\".", type.to_string());
    return Error(errc, msg);
  }

  if(cons) consume(1);
  return Result<void>::create();
}

auto Lexer::consume(const uint32_t amnt) -> const Token& {
  if(curr_ == TokenType::EndOfFile)
    return curr_;

  for(uint32_t i = 0; i < amnt; i++) {
    curr_ = produce_impl_();
    if(curr_ == TokenType::EndOfFile) break;
  }
  
  return curr_;
}

auto Lexer::dump(OStream& stream) -> void {
  do {
    stream << curr_.format(*this);
    consume(1);
  } while(curr_ != TokenType::EndOfFile && curr_ != TokenType::Illegal);

  if(curr_ == TokenType::Illegal)
    ErrorCollector::display_error("Illegal token!", *this, curr_, stream);
}

END_NAMESPACE(n19);
