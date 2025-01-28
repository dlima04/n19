/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#include <Core/Try.hpp>
#include <Core/Murmur3.hpp>
#include <Frontend/Lexer.hpp>
#include <Frontend/ErrorCollector.hpp>
#include <Core/ConIO.hpp>
#include <filesystem>
#include <algorithm>
#include <limits>
#include <functional>
#include <array>
BEGIN_NAMESPACE(n19);

inline auto Lexer::_token_hyphen() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.line_ = line_;

  switch(_peek_char()) {
  case u8'=': // '-='
    curr_tok.type_ = TokenType::SubEq;
    curr_tok.cat_  = TokenCategory::BinaryOp;
    curr_tok.cat_ |= TokenCategory::ArithAssignOp;
    curr_tok.cat_ |= TokenCategory::PointerArithOp;
    curr_tok.cat_ |= TokenCategory::ArithmeticOp;
    curr_tok.len_  = 2;
    _consume_char(2);
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
    _consume_char(2);
    break;
  case u8'>': // '->'
    curr_tok.type_ = TokenType::SkinnyArrow;
    curr_tok.cat_  = TokenCategory::ValidPostfix;
    curr_tok.cat_ |= TokenCategory::BinaryOp;
    curr_tok.len_  = 2;
    _consume_char(2);
  default: // '-'
    curr_tok.type_ = TokenType::Sub;
    curr_tok.cat_  = TokenCategory::BinaryOp;
    curr_tok.cat_ |= TokenCategory::ArithmeticOp;
    curr_tok.cat_ |= TokenCategory::PointerArithOp;
    curr_tok.len_  = 1;
    _consume_char(1);
    break;
  }

  return curr_tok;
}

inline auto Lexer::_token_ampersand() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.line_ = line_;
  curr_tok.cat_  = TokenCategory::BinaryOp;

  switch(_peek_char()) {
  case u8'=': // '&='
    curr_tok.type_ = TokenType::BitwiseAndEq;
    curr_tok.cat_ |= TokenCategory::BitwiseAssignOp;
    curr_tok.cat_ |= TokenCategory::BitwiseOp;
    curr_tok.len_  = 2;
    _consume_char(2);
    break;
  case u8'&': // '&&'
    curr_tok.type_ = TokenType::LogicalAnd;
    curr_tok.cat_ |= TokenCategory::LogicalOp;
    curr_tok.len_  = 2;
    _consume_char(2);
    break;
  default: // '&'
    curr_tok.type_  = TokenType::BitwiseAnd;
    curr_tok.cat_  |= TokenCategory::BitwiseOp;
    curr_tok.cat_  |= TokenCategory::UnaryOp;
    curr_tok.cat_  |= TokenCategory::ValidPrefix;
    curr_tok.len_  = 1;
    _consume_char(1);
    break;
  }

  return curr_tok;
}

inline auto Lexer::_token_dot() -> Token {
  Token curr_tok;
  curr_tok.line_ = line_;
  curr_tok.cat_  = TokenCategory::NonCategorical;
  curr_tok.pos_  = index_;

  if(_peek_char(1) == u8'.' && _peek_char(2) == u8'.') { // '...'
    curr_tok.type_ = TokenType::DotThree;
    curr_tok.len_  = 3;
    _consume_char(3);
  } else if(_peek_char(1) == u8'.') { // '..'
    curr_tok.type_ = TokenType::DotTwo;
    curr_tok.len_  = 2; 
    _consume_char(2);
  } else { // '.'
    curr_tok.type_ = TokenType::Dot;
    curr_tok.cat_  = TokenCategory::ValidPostfix | TokenCategory::BinaryOp;
    curr_tok.len_  = 1;
    _consume_char(1);
  }
  
  return curr_tok;
}

inline auto Lexer::_token_equals() -> Token {
  Token curr_tok;
  curr_tok.line_ = line_;
  curr_tok.pos_ = index_;

  switch(_peek_char()) {
  case '=': // '=='
    curr_tok.type_ = TokenType::Eq;
    curr_tok.cat_  = TokenCategory::BinaryOp;
    curr_tok.cat_ |= TokenCategory::LogicalOp;
    curr_tok.cat_ |= TokenCategory::ComparisonOp;
    curr_tok.len_  = 2;
    _consume_char(2);
    break;
  case '>': // '=>'
    curr_tok.type_ = TokenType::FatArrow;
    curr_tok.cat_  = TokenCategory::NonCategorical;
    curr_tok.len_  = 2;
    _consume_char(2);
    break;
  default: // '='
    curr_tok.type_ = TokenType::ValueAssignment;
    curr_tok.cat_  = TokenCategory::BinaryOp;
    curr_tok.len_  = 1;
    _consume_char(1);
    break;
  }
  
  return curr_tok;
}

inline auto Lexer::_token_plus() -> Token {
  Token curr_tok;
  curr_tok.line_ = line_;
  curr_tok.pos_ = index_;

  switch(_peek_char()) {
  case '=': // '+='
    curr_tok.type_ = TokenType::PlusEq;
    curr_tok.cat_  = TokenCategory::BinaryOp;
    curr_tok.cat_ |= TokenCategory::ArithAssignOp;
    curr_tok.cat_ |= TokenCategory::ArithmeticOp;
    curr_tok.cat_ |= TokenCategory::PointerArithOp;
    curr_tok.len_  = 2;
    _consume_char(2);
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
    _consume_char(2);
    break;
  default: // '+'
    curr_tok.type_ = TokenType::Plus;
    curr_tok.cat_  = TokenCategory::BinaryOp;
    curr_tok.cat_ |= TokenCategory::UnaryOp;
    curr_tok.cat_ |= TokenCategory::ArithmeticOp;
    curr_tok.cat_ |= TokenCategory::PointerArithOp;
    curr_tok.cat_ |= TokenCategory::ValidPrefix;
    curr_tok.len_  = 1;
    _consume_char(1);
    break;
  }

  return curr_tok;
}

inline auto Lexer::_token_gthan() -> Token {
  Token curr_tok;
  curr_tok.cat_  = TokenCategory::BinaryOp;
  curr_tok.line_ = line_;
  curr_tok.pos_  = index_;
  
  if(_peek_char(1) == '=') { // '>='
    curr_tok.type_ = TokenType::Gte;
    curr_tok.cat_ |= TokenCategory::LogicalOp | TokenCategory::ComparisonOp;
    curr_tok.len_  = 2;
    _consume_char(2);
  } else if(_peek_char(1) == '>' && _peek_char(2) == '=') { // '>>='
    curr_tok.type_ = TokenType::RshiftEq;
    curr_tok.cat_ |= TokenCategory::BitwiseOp | TokenCategory::BitwiseAssignOp;
    curr_tok.len_  = 3;
    _consume_char(3);
  } else if(_peek_char(1) == '>') { // '>>'
    curr_tok.type_ = TokenType::Rshift;
    curr_tok.cat_ |= TokenCategory::BitwiseOp;
    curr_tok.len_  = 2;
    _consume_char(2);
  } else { // '>'
    curr_tok.type_ = TokenType::Gt;
    curr_tok.cat_ |= TokenCategory::LogicalOp | TokenCategory::ComparisonOp;
    curr_tok.len_  = 1;
    _consume_char(1);
  }

  return curr_tok;
}

inline auto Lexer::_token_squote() -> Token {
  Token curr_tok;
  curr_tok.line_ = line_;
  curr_tok.pos_  = index_;
  
  //
  // Check what's INSIDE the quotes first.
  // TODO: not sure if this code is fully correct...
  //

  _consume_char(1);
  if(_current_char() == '\\') {
    _consume_char(1);
    if(UTF8_LEADING(_current_char())) {
      curr_tok.len_  = index_ - curr_tok.pos_;
      curr_tok.type_ = TokenType::Illegal;
      return curr_tok;
    }
    _consume_char(1);
  } else if(_current_char() == '\'') {
    _consume_char(1);
    curr_tok.type_ = TokenType::ByteLiteral;
    curr_tok.cat_  = TokenCategory::Literal;
    curr_tok.len_  = index_ - curr_tok.pos_;
    return curr_tok;
  } else if(UTF8_LEADING(_current_char()) || _current_char() == '\n') {
    curr_tok.type_ = TokenType::Illegal;
    curr_tok.cat_  = TokenCategory::NonCategorical;
    curr_tok.len_  = index_ - curr_tok.pos_;
    return curr_tok;
  } else {
    _consume_char(1);
  }

  //
  // Ensure the following character is a closing quote, token == illegal otherwise.
  //

  if(_current_char() == '\0') {
    curr_tok = Token::eof(src_.size() - 1, line_);
  } else if(_current_char() == '\'') [[likely]] {
    _consume_char(1);
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

inline auto Lexer::_token_asterisk() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;

  if(_peek_char() == u8'=') { // '*='
    curr_tok.type_ = TokenType::MulEq;
    curr_tok.cat_  = TokenCategory::BinaryOp;
    curr_tok.cat_ |= TokenCategory::ArithAssignOp;
    curr_tok.cat_ |= TokenCategory::ArithmeticOp;
    curr_tok.len_ = 2;
    _consume_char(2);
  } else [[likely]] { // '*'
    curr_tok.type_ = TokenType::Mul;
    curr_tok.cat_  = TokenCategory::BinaryOp;
    curr_tok.cat_ |= TokenCategory::UnaryOp;
    curr_tok.cat_ |= TokenCategory::ArithmeticOp;
    curr_tok.cat_ |= TokenCategory::ValidPrefix;
    _consume_char(1);
  }

  return curr_tok;
}

inline auto Lexer::_token_colon() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.line_ = line_;
  
  if(_peek_char() == u8':') { // '::'
    curr_tok.type_  = TokenType::NamespaceOperator;
    curr_tok.cat_   = TokenCategory::ValidPrefix;
    curr_tok.len_   = 2;
    _consume_char(2);
  } else { // ':'
    curr_tok.type_  = TokenType::TypeAssignment;
    curr_tok.cat_   = TokenCategory::NonCategorical;
    curr_tok.len_   = 1;
    _consume_char(1);
  }

  return curr_tok;
}

auto Lexer::_token_bang() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.line_ = line_;

  if(_peek_char() == u8'=') { // '!='
    curr_tok.type_ = TokenType::Neq;
    curr_tok.cat_  = TokenCategory::BinaryOp;
    curr_tok.cat_ |= TokenCategory::LogicalOp;
    curr_tok.cat_ |= TokenCategory::ComparisonOp;
    curr_tok.len_  = 2;
    _consume_char(2);
  } else { // '!'
    curr_tok.type_ = TokenType::LogicalNot;
    curr_tok.cat_  = TokenCategory::UnaryOp;
    curr_tok.cat_ |= TokenCategory::ValidPrefix;
    curr_tok.cat_ |= TokenCategory::LogicalOp;
    curr_tok.len_  = 1;
    _consume_char(1);
  }
  
  return curr_tok;
}

inline auto Lexer::_token_percent() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.line_ = line_;
  curr_tok.cat_  = TokenCategory::BinaryOp | TokenCategory::ArithmeticOp;
  
  if(_peek_char() == u8'=') { // '%='
    curr_tok.type_ = TokenType::ModEq;
    curr_tok.cat_ |= TokenCategory::ArithAssignOp;
    curr_tok.len_  = 2;
    _consume_char(2);
  } else { // '%'
    curr_tok.type_ = TokenType::Mod;
    curr_tok.len_  = 1;
    _consume_char(1);
  }

  return curr_tok;
}

inline auto Lexer::_token_uparrow() -> Token {
  Token curr_tok;
  curr_tok.cat_  = TokenCategory::BinaryOp | TokenCategory::BitwiseOp;
  curr_tok.pos_  = index_;
  curr_tok.line_ = line_;

  if(_peek_char() == u8'=') { // '^='
    curr_tok.type_ = TokenType::XorEq;
    curr_tok.cat_ |= TokenCategory::BitwiseAssignOp;
    curr_tok.len_  = 2;
    _consume_char(2);
  } else { // '^'
    curr_tok.type_ = TokenType::Xor;
    curr_tok.len_  = 1;
    _consume_char(1);
  }

  return curr_tok;
}

inline auto Lexer::_token_pipe() -> Token {
  Token curr_tok {
    .pos_  = index_,
    .len_  = 2,
    .line_ = line_,
    .cat_  = TokenCategory::BinaryOp
  };

  switch(_peek_char()) {
  case u8'|': // '||'
    curr_tok.type_ = TokenType::LogicalOr;
    curr_tok.cat_ |= TokenCategory::LogicalOp;
    _consume_char(2);
    break;
  case u8'=': // '|='
    curr_tok.type_  = TokenType::BitwiseOrEq;
    curr_tok.cat_  |= TokenCategory::BitwiseOp | TokenCategory::BitwiseAssignOp;
    _consume_char(2);
    break;
  default: // '|'
    curr_tok.type_ = TokenType::BitwiseOr;
    curr_tok.cat_  = TokenCategory::BitwiseOp;
    curr_tok.len_  = 1;
    _consume_char(1);
    break;
  }

  return curr_tok;
}

inline auto Lexer::_token_lthan() -> Token {
  Token curr_tok {
    .pos_  = index_,
    .line_ = line_,
    .cat_  = TokenCategory::BinaryOp
  };

  if(_peek_char(1) == u8'=') { // '<='
    curr_tok.type_ = TokenType::Lte;
    curr_tok.cat_ |= TokenCategory::LogicalOp | TokenCategory::ComparisonOp;
    curr_tok.len_  = 2;
    _consume_char(2);
  } else if(_peek_char(1) == u8'<' && _peek_char(2) == u8'=') [[unlikely]] { // <<=
    curr_tok.type_ = TokenType::LshiftEq;
    curr_tok.cat_ |= TokenCategory::BitwiseOp | TokenCategory::BitwiseAssignOp;
    curr_tok.len_  = 3;
    _consume_char(3);
  } else if(_peek_char(1) == u8'<') { // '<<'
    curr_tok.type_ = TokenType::Lshift;
    curr_tok.cat_ |= TokenCategory::BitwiseOp;
    curr_tok.len_  = 2;
    _consume_char(2);
  } else { // '<'
    curr_tok.type_ = TokenType::Lt;
    curr_tok.cat_ |= TokenCategory::LogicalOp | TokenCategory::ComparisonOp;
    curr_tok.len_  = 1;
    _consume_char(1);
  }

  return curr_tok;
}

N19_FORCEINLINE auto Lexer::_token_null()  -> Token {
  Token curr_tok;
  curr_tok.type_ = TokenType::EndOfFile;
  curr_tok.cat_  = TokenCategory::NonCategorical;
  curr_tok.len_  = 0;
  curr_tok.pos_  = src_.size() - 1;
  return curr_tok;
}

N19_FORCEINLINE auto Lexer::_token_tilde() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::BitwiseNot;
  curr_tok.cat_  = TokenCategory::UnaryOp | TokenCategory::BitwiseOp | TokenCategory::ValidPrefix;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  _consume_char(1);
  return curr_tok;
}

N19_FORCEINLINE auto Lexer::_token_at() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::At;
  curr_tok.cat_  = TokenCategory::NonCategorical;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  _consume_char(1);
  return curr_tok;
}

N19_FORCEINLINE auto Lexer::_token_money() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::Money;
  curr_tok.cat_  = TokenCategory::NonCategorical;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  _consume_char(1);
  return curr_tok;
}

N19_FORCEINLINE auto Lexer::_token_lsqbrckt() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::LeftSqBracket;
  curr_tok.cat_  = TokenCategory::Punctuator | TokenCategory::ValidPostfix;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  _consume_char(1);
  return curr_tok;
}

N19_FORCEINLINE auto Lexer::_token_rsqbrckt() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::RightSqBracket;
  curr_tok.cat_  = TokenCategory::Punctuator;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  _consume_char(1);
  return curr_tok;
}

N19_FORCEINLINE auto Lexer::_token_semicolon() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::Semicolon;
  curr_tok.cat_  = TokenCategory::Punctuator;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  _consume_char(1);
  return curr_tok;
}

N19_FORCEINLINE auto Lexer::_token_lparen() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::LeftParen;
  curr_tok.cat_  = TokenCategory::Punctuator | TokenCategory::ValidPostfix;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  _consume_char(1);
  return curr_tok;
}

N19_FORCEINLINE auto Lexer::_token_rparen() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::RightParen;
  curr_tok.cat_  = TokenCategory::Punctuator;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  _consume_char(1);
  return curr_tok;
}

N19_FORCEINLINE auto Lexer::_token_lbrace() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::LeftBrace;
  curr_tok.cat_  = TokenCategory::Punctuator;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  _consume_char(1);
  return curr_tok;
}

N19_FORCEINLINE auto Lexer::_token_rbrace() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::RightBrace;
  curr_tok.cat_  = TokenCategory::Punctuator;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  _consume_char(1);
  return curr_tok;
}

N19_FORCEINLINE auto Lexer::_token_comma() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::Comma;
  curr_tok.cat_  = TokenCategory::Punctuator | TokenCategory::Terminator;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  _consume_char(1);
  return curr_tok;
}

auto Lexer::_produce_impl() -> Token {
  if(index_ >= src_.size()) {
    return Token::eof(src_.size() - 1, line_);
  }

  SWITCH_BEGIN:
  switch(_current_char()) {
    case u8'\\' : [[fallthrough]]; // illegal character! fallthrough.
    case u8'?'  : return Token::illegal(index_, 1, line_);
    case u8'#'  : _skip_comment();         goto SWITCH_BEGIN;
    case u8'\n' : _advance_consume_line(); goto SWITCH_BEGIN;
    case u8' '  : [[fallthrough]]; // skip character.
    case u8'\r' : [[fallthrough]]; // skip character.
    case u8'\b' : [[fallthrough]]; // skip character.
    case u8'\a' : [[fallthrough]]; // skip character.
    case u8'\v' : [[fallthrough]]; // skip character.
    case u8'\t' : _consume_char(1);        goto SWITCH_BEGIN;
    case u8'/'  : return _token_fwdslash();
    case u8'\0' : return _token_null();
    case u8'~'  : return _token_tilde();
    case u8'@'  : return _token_at();
    case u8'$'  : return _token_money();
    case u8'['  : return _token_lsqbrckt();
    case u8']'  : return _token_rsqbrckt();
    case u8';'  : return _token_semicolon();
    case u8'('  : return _token_lparen();
    case u8')'  : return _token_rparen();
    case u8'{'  : return _token_lbrace();
    case u8'}'  : return _token_rbrace();
    case u8','  : return _token_comma();
    case u8'-'  : return _token_hyphen();
    case u8'&'  : return _token_ampersand();
    case u8'.'  : return _token_dot();
    case u8'!'  : return _token_bang();
    case u8'<'  : return _token_lthan();
    case u8'>'  : return _token_gthan();
    case u8'+'  : return _token_plus();
    case u8'*'  : return _token_asterisk();
    case u8'%'  : return _token_percent();
    case u8'='  : return _token_equals();
    case u8'|'  : return _token_pipe();
    case u8'^'  : return _token_uparrow();
    case u8'\'' : return _token_squote();
    case u8'"'  : return _token_quote();
    case u8'`'  : return _token_quote();
    case u8':'  : return _token_colon();
    default     : return _token_ambiguous();
  }

  UNREACHABLE; // assertion
}

inline auto Lexer::_token_fwdslash() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.line_ = line_;

  if(_peek_char() == u8'=') { // '/='
    curr_tok.type_ = TokenType::DivEq;
    curr_tok.cat_  = TokenCategory::BinaryOp;
    curr_tok.cat_ |= TokenCategory::ArithAssignOp;
    curr_tok.cat_ |= TokenCategory::ArithmeticOp;
    curr_tok.len_  = 2;
    _consume_char(2);
  } else { // '/'
    curr_tok.type_ = TokenType::Div;
    curr_tok.cat_  = TokenCategory::BinaryOp;
    curr_tok.cat_ |= TokenCategory::UnaryOp;
    curr_tok.cat_ |= TokenCategory::ArithmeticOp;
    curr_tok.cat_ |= TokenCategory::ValidPrefix;
    curr_tok.len_  = 1;
    _consume_char(1);
  }

  return curr_tok;
}

inline auto Lexer::_token_quote() -> Token {
  Token curr_tok;
  const auto opening_quote = _current_char();
  const auto string_start  = index_;

  _consume_char(1);
  while(true) {
    if(_current_char() == '\0' || _current_char() == '\n') {
      curr_tok.type_ = TokenType::Illegal;
      curr_tok.cat_  = TokenCategory::NonCategorical;
      curr_tok.pos_  = string_start;
      break;
    } if(_current_char() == opening_quote) {
      _consume_char(1);
      curr_tok.type_ = TokenType::StringLiteral;
      curr_tok.cat_  = TokenCategory::Literal;
      curr_tok.pos_  = string_start;
      curr_tok.len_  = index_ - string_start;
      break;
    }

    /// skip escaped quote.
    if(_current_char() == '\\' && _peek_char() == opening_quote) {
      _consume_char(2);
    } else if(UTF8_LEADING(_current_char())) {
      _skip_utf8_sequence();
    } else {
      _consume_char(1);
    }
  }

  return curr_tok;
}

inline auto Lexer::_token_hex_lit() -> Token {
  ASSERT(_current_char() == '0');
  ASSERT(_peek_char() == 'x' || _peek_char() == 'X');

  const auto start = index_;
  _consume_char(2); // move past "0x"
  _skip_chars_until([](const char8_t ch) {
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

inline auto Lexer::_token_num_lit() -> Token {
  ASSERT( CH_IS_DIGIT(_current_char()) );
  const size_t start = index_;
  bool seen_exponent = false;
  bool seen_dot      = false;

  Token curr_tok;
  curr_tok.line_ = line_;
  curr_tok.pos_  = start;
  curr_tok.cat_  = TokenCategory::Literal;

  while(true) {
    const char8_t curr = _current_char();
    const char8_t next = _peek_char();

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
    /// Time to parse it out!!! :)
    else if(curr == u8'e' || curr == u8'E') {
      if(seen_exponent) {
        curr_tok.type_ = TokenType::Illegal;
        curr_tok.cat_  = TokenCategory::NonCategorical;
        return curr_tok;
      } if((next == u8'-' || next == u8'+') && !CH_IS_DIGIT(_peek_char(2))) {
        curr_tok.type_ = TokenType::Illegal;
        curr_tok.cat_  = TokenCategory::NonCategorical;
        return curr_tok;
      } if((next == u8'-' || next == u8'+') && CH_IS_DIGIT(_peek_char(2))) {
        _consume_char(1);
      }

      seen_exponent = true;
    }

    /// If we don't see '.' or 'e' and it isn't a digit,
    /// we've finished parsing this integer sequence.
    else if(!CH_IS_DIGIT(_current_char())) {
      break;
    }

    /// Otherwise just consume the current character.
    _consume_char(1);
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

inline auto Lexer::_token_oct_lit() -> Token {
  ASSERT(_current_char() == '0');
  ASSERT(CH_IS_DIGIT(_peek_char()));

  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.line_ = line_;
  curr_tok.type_ = TokenType::OctalLiteral;
  curr_tok.cat_  = TokenCategory::Literal;

  auto is_octal_digit = [](const char c) -> bool {
    return c >= '0' && c <= '7';
  };

  while(true) {
    if(!CH_IS_DIGIT(_current_char())) {
      break;
    } if(!is_octal_digit(_current_char())) {
      curr_tok.type_ = TokenType::Illegal;
      curr_tok.cat_  = TokenCategory::NonCategorical;
      return curr_tok;
    }
    _consume_char(1);
  }

  curr_tok.len_ = index_ - curr_tok.pos_;
  return curr_tok;
}

inline auto Lexer::_token_ambiguous() -> Token {
  ASSERT(index_ < src_.size());
  ASSERT(!is_reserved_byte(_current_char()));

  const char8_t next = _peek_char();
  const char8_t curr = _current_char();

  if(curr == u8'0' && (next == u8'x' || next == u8'X')) {
    return _token_hex_lit();
  } if(curr == u8'0' && CH_IS_DIGIT( next )) {
    return _token_oct_lit();
  } if(CH_IS_DIGIT( curr )) {
    return _token_num_lit();
  }

  const auto start = index_;
  _skip_chars_until([](const char8_t ch) {
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

auto Lexer::get_keyword(const std::u8string_view& str) -> Maybe<Keyword> {
  Keyword kw;                                /// The keyword.
  constexpr uint32_t seed = 0xbeef;          /// Hash seed.
  if(str.size() > 15) return Nothing;

  switch (murmur3_x86_32(str, seed)) {
  case u8"return"_mm32:                      /// "return"
    kw.cat |= TokenCategory::Keyword;        /// ControlFlow | Keyword
    kw.cat |= TokenCategory::ControlFlow;    ///
    kw.type = TokenType::Return;
    break;
  case u8"break"_mm32:                       /// "break"
    kw.cat |= TokenCategory::Keyword;        /// ControlFlow | Keyword
    kw.cat |= TokenCategory::ControlFlow;    ///
    kw.type = TokenType::Break;
    break;
  case u8"continue"_mm32:                    /// "continue"
    kw.cat |= TokenCategory::Keyword;        /// ControlFlow | Keyword
    kw.cat |= TokenCategory::ControlFlow;    ///
    kw.type = TokenType::Continue;
    break;
  case u8"for"_mm32:                         /// "for"
    kw.cat |= TokenCategory::Keyword;        /// ControlFlow | Keyword
    kw.cat |= TokenCategory::ControlFlow;    ///
    kw.type = TokenType::For;
    break;
  case u8"while"_mm32:                       /// "while"
    kw.cat |= TokenCategory::Keyword;        /// ControlFlow | Keyword
    kw.cat |= TokenCategory::ControlFlow;    ///
    kw.type = TokenType::While;
    break;
  case u8"do"_mm32:                          /// "do"
    kw.cat |= TokenCategory::Keyword;        /// ControlFlow | Keyword
    kw.cat |= TokenCategory::ControlFlow;    ///
    kw.type = TokenType::Do;
    break;
  case u8"if"_mm32:                          /// "if"
    kw.cat |= TokenCategory::Keyword;        /// ControlFlow | Keyword
    kw.cat |= TokenCategory::ControlFlow;    ///
    kw.type = TokenType::If;
    break;
  case u8"else"_mm32:                        /// "else"
    kw.cat |= TokenCategory::Keyword;        /// ControlFlow | Keyword
    kw.cat |= TokenCategory::ControlFlow;    ///
    kw.type = TokenType::Else;
    break;
  case u8"struct"_mm32:                      /// "struct"
    kw.cat |= TokenCategory::Keyword;        /// Keyword
    kw.type = TokenType::Struct;             ///
    break;
  case u8"switch"_mm32:                      /// "switch"
    kw.cat |= TokenCategory::Keyword;        /// ControlFlow | Keyword
    kw.cat |= TokenCategory::ControlFlow;    ///
    kw.type = TokenType::Switch;
    break;
  case u8"case"_mm32:                        /// "case"
    kw.cat |= TokenCategory::Keyword;        /// ControlFlow | Keyword
    kw.cat |= TokenCategory::ControlFlow;    ///
    kw.type = TokenType::Case;
    break;
  case u8"default"_mm32:                     /// "default"
    kw.cat |= TokenCategory::Keyword;        /// ControlFlow | Keyword
    kw.cat |= TokenCategory::ControlFlow;    ///
    kw.type = TokenType::Default;
    break;
  case u8"block"_mm32:                       /// "block"
    kw.cat |= TokenCategory::Keyword;        /// Keyword
    kw.type = TokenType::Block;
    break;
  case u8"defer"_mm32:                       /// "defer"
    kw.cat |= TokenCategory::Keyword;        /// Keyword
    kw.type = TokenType::Defer;
    break;
  case u8"defer_if"_mm32:                    /// "defer_if"
    kw.cat |= TokenCategory::Keyword;        /// Keyword
    kw.type = TokenType::DeferIf;
    break;
  case u8"sizeof"_mm32:                      /// "sizeof"
    kw.cat |= TokenCategory::Keyword;        /// UnaryOp | ValidPrefix | Keyword
    kw.cat |= TokenCategory::UnaryOp;        ///
    kw.cat |= TokenCategory::ValidPrefix;    ///
    kw.type = TokenType::Sizeof;
    break;
  case u8"typeof"_mm32:                      /// "typeof"
    kw.cat |= TokenCategory::Keyword;        /// UnaryOp | ValidPrefix | Keyword
    kw.cat |= TokenCategory::UnaryOp;        ///
    kw.cat |= TokenCategory::ValidPrefix;    ///
    kw.type = TokenType::Typeof;
    break;
  case u8"fallthrough"_mm32:                 /// "fallthrough"
    kw.cat |= TokenCategory::Keyword;        /// ControlFlow | Keyword
    kw.cat |= TokenCategory::ControlFlow;    ///
    kw.type = TokenType::Fallthrough;
    break;
  case u8"namespace"_mm32:                   /// "namespace"
    kw.cat |= TokenCategory::Keyword;        /// Keyword
    kw.type = TokenType::Namespace;          ///
    break;
  case u8"where"_mm32:                       /// "where"
    kw.cat |= TokenCategory::Keyword;        /// Keyword
    kw.type = TokenType::Where;              ///
    break;
  case u8"otherwise"_mm32:                   /// "otherwise"
    kw.cat |= TokenCategory::Keyword;        /// Keyword
    kw.type = TokenType::Otherwise;          ///
    break;
  case u8"proc"_mm32:                        /// "proc"
    kw.cat |= TokenCategory::Keyword;        /// Keyword
    kw.type = TokenType::Proc;               ///
    break;
  case u8"let"_mm32:                         /// "let"
    kw.cat |= TokenCategory::Keyword;        /// Keyword
    kw.type = TokenType::Let;                ///
    break;
  case u8"const"_mm32:                       /// "const"
    kw.cat |= TokenCategory::Keyword;        /// Keyword
    kw.type = TokenType::Const;              ///
    break;
  case u8"as"_mm32:                          /// "as"
    kw.cat |= TokenCategory::Keyword;        /// UnaryOp | ValidPostfix | Keyword
    kw.cat |= TokenCategory::ValidPostfix;   ///
    kw.cat |= TokenCategory::UnaryOp;        ///
    kw.type = TokenType::As;
    break;
  case u8"with"_mm32:                        /// "with"
    kw.cat |= TokenCategory::Keyword;        /// UnaryOp | ValidPostfix | Keyword
    kw.cat |= TokenCategory::ValidPostfix;   ///
    kw.cat |= TokenCategory::UnaryOp;        ///
    kw.type = TokenType::With;
    break;
  case u8"true"_mm32:                        /// "true"
    kw.cat  = TokenCategory::Literal;        /// Literal
    kw.type = TokenType::BooleanLiteral;     ///
    break;
  case u8"false"_mm32:                       /// "false"
    kw.cat  = TokenCategory::Literal;        /// Literal
    kw.type = TokenType::BooleanLiteral;     ///
    break;
  case u8"null"_mm32:                        /// "null"
    kw.cat  = TokenCategory::Literal;        /// Literal
    kw.type = TokenType::NullLiteral;        ///
    break;
  default: return Nothing;
  }

  return kw;
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

inline auto Lexer::_skip_chars_until(std::function<bool(char8_t)> cb) -> bool {
  while(!cb(_current_char()) && _current_char() != '\0') {
    const char8_t curr = _current_char();

    if(curr == '\n')            _advance_consume_line();  /// line feed
    else if(UTF8_LEADING(curr)) _skip_utf8_sequence();    /// UTF8 codepoint begin
    else _consume_char(1);                                /// ASCII character
  }

  return _current_char() != '\0';
}

inline auto Lexer::_skip_utf8_sequence() -> bool {
  const auto ch = static_cast<uint8_t>(_current_char());

  if ((ch & 0xE0) == 0xC0)      _consume_char(2); /// 2 byte codepoint.
  else if ((ch & 0xF0) == 0xE0) _consume_char(3); /// 3 byte codepoint.
  else if ((ch & 0xF8) == 0xF0) _consume_char(4); /// 4 byte codepoint.
  else return false;                              /// Something's wrong?

  return index_ - 1 < src_.size();
}

auto Lexer::create_shared(const FileRef& ref) -> Result<std::shared_ptr<Lexer>> {
  const auto fsize = TRY( ref.size() );

  /// Check against maximum allowed file size.
  /// No way this is ever true tbh.
  if(*fsize >= std::numeric_limits<uint32_t>::max()) {
    return make_error(ErrC::InvalidArg, "File is too large");
  }

  /// Check for an empty file.
  /// TODO: should we really error on this? I don't know if
  /// this even makes sense.
  if(*fsize == 0) {
    return make_error(ErrC::InvalidArg, "File is empty.");
  }

  auto lxr = std::make_shared<Lexer>();
  lxr->file_name_ = fs::absolute(*ref).string();
  lxr->src_.resize(*fsize);

  TRY(ref.read_into(as_writable_bytes(lxr->src_)));
  lxr->curr_ = lxr->_produce_impl();
  return make_result<std::shared_ptr<Lexer>>(lxr);
}

auto Lexer::expect(const TokenCategory cat, const bool cons) -> Result<void> {
  if(current().cat_ != cat) {
    const auto errc = ErrC::BadToken;
    const auto msg  = fmt("Expected token of kind \"{}\".", cat.to_string());
    return make_error(errc, std::cref(msg));
  }

  if(cons) consume(1);
  return make_result<void>();
}

auto Lexer::expect(const TokenType type, const bool cons) -> Result<void> {
  if(current().type_ != type) {
    const auto errc = ErrC::BadToken;
    const auto msg  = fmt("Expected token \"{}\".", type.to_string());
    return make_error(errc, std::cref(msg));
  }

  if(cons) consume(1);
  return make_result<void>();
}

auto Lexer::consume(const uint32_t amnt) -> const Token& {
  if(curr_ == TokenType::EndOfFile)
    return curr_;

  for(uint32_t i = 0; i < amnt; i++) {
    curr_ = _produce_impl();
    if(curr_ == TokenType::EndOfFile) break;
  }
  
  return curr_;
}

auto Lexer::dump() -> void {
  do {
    outs() << curr_.format(*this);
    consume(1);
  } while(curr_ != TokenType::EndOfFile && curr_ != TokenType::Illegal);

  if(curr_ == TokenType::Illegal)
    ErrorCollector::display_error("Illegal token!", *this, curr_, outs());
}

END_NAMESPACE(n19);