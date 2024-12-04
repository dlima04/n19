/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#include <Frontend/Lexer.hpp>
#include <Frontend/ErrorCollector.hpp>
#include <Core/ResultMacros.hpp>
#include <filesystem>
#include <algorithm>
#include <limits>
#include <functional>
#include <array>

inline auto n19::Lexer::_token_hyphen() -> Token {
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

inline auto n19::Lexer::_token_ampersand() -> Token {
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

inline auto n19::Lexer::_token_dot() -> Token {
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

inline auto n19::Lexer::_token_equals() -> Token {
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

inline auto n19::Lexer::_token_plus() -> Token {
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

inline auto n19::Lexer::_token_gthan() -> Token {
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

inline auto n19::Lexer::_token_squote() -> Token {
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

inline auto n19::Lexer::_token_asterisk() -> Token {
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

inline auto n19::Lexer::_token_colon() -> Token {
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

auto n19::Lexer::_token_bang() -> Token {
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

inline auto n19::Lexer::_token_percent() -> Token {
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

inline auto n19::Lexer::_token_uparrow() -> Token {
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

inline auto n19::Lexer::_token_pipe() -> Token {
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

inline auto n19::Lexer::_token_lthan() -> Token {
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

N19_FORCEINLINE auto n19::Lexer::_token_null()  -> Token {
  Token curr_tok;
  curr_tok.type_ = TokenType::EndOfFile;
  curr_tok.cat_  = TokenCategory::NonCategorical;
  curr_tok.len_  = 0;
  curr_tok.pos_  = src_.size() - 1;
  return curr_tok;
}

N19_FORCEINLINE auto n19::Lexer::_token_tilde() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::BitwiseNot;
  curr_tok.cat_  = TokenCategory::UnaryOp | TokenCategory::BitwiseOp | TokenCategory::ValidPrefix;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  _consume_char(1);
  return curr_tok;
}

N19_FORCEINLINE auto n19::Lexer::_token_at() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::At;
  curr_tok.cat_  = TokenCategory::NonCategorical;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  _consume_char(1);
  return curr_tok;
}

N19_FORCEINLINE auto n19::Lexer::_token_money() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::Money;
  curr_tok.cat_  = TokenCategory::NonCategorical;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  _consume_char(1);
  return curr_tok;
}

N19_FORCEINLINE auto n19::Lexer::_token_lsqbrckt() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::LeftSqBracket;
  curr_tok.cat_  = TokenCategory::Punctuator | TokenCategory::ValidPostfix;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  _consume_char(1);
  return curr_tok;
}

N19_FORCEINLINE auto n19::Lexer::_token_rsqbrckt() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::RightSqBracket;
  curr_tok.cat_  = TokenCategory::Punctuator;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  _consume_char(1);
  return curr_tok;
}

N19_FORCEINLINE auto n19::Lexer::_token_semicolon() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::Semicolon;
  curr_tok.cat_  = TokenCategory::Punctuator;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  _consume_char(1);
  return curr_tok;
}

N19_FORCEINLINE auto n19::Lexer::_token_lparen() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::LeftParen;
  curr_tok.cat_  = TokenCategory::Punctuator | TokenCategory::ValidPostfix;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  _consume_char(1);
  return curr_tok;
}

N19_FORCEINLINE auto n19::Lexer::_token_rparen() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::RightParen;
  curr_tok.cat_  = TokenCategory::Punctuator;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  _consume_char(1);
  return curr_tok;
}

N19_FORCEINLINE auto n19::Lexer::_token_lbrace() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::LeftBrace;
  curr_tok.cat_  = TokenCategory::Punctuator;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  _consume_char(1);
  return curr_tok;
}

N19_FORCEINLINE auto n19::Lexer::_token_rbrace() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::RightBrace;
  curr_tok.cat_  = TokenCategory::Punctuator;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  _consume_char(1);
  return curr_tok;
}

N19_FORCEINLINE auto n19::Lexer::_token_comma() -> Token {
  Token curr_tok;
  curr_tok.pos_  = index_;
  curr_tok.type_ = TokenType::Comma;
  curr_tok.cat_  = TokenCategory::Punctuator | TokenCategory::Terminator;
  curr_tok.line_ = line_;
  curr_tok.len_  = 1;
  _consume_char(1);
  return curr_tok;
}

auto n19::Lexer::_produce_impl() -> Token {
  if(index_ >= src_.size()) {
    return Token::eof(src_.size() - 1, line_);
  }

  while(true)
  switch(_current_char()) {
    case u8'\\': [[fallthrough]]; // illegal character! fallthrough.
    case u8'?':  return Token::illegal(index_, 1, line_);
    case u8'#':  _skip_comment();         continue;
    case u8'\n': _advance_consume_line(); continue;
    case u8' ':  [[fallthrough]]; // skip character.
    case u8'\r': [[fallthrough]]; // skip character.
    case u8'\b': [[fallthrough]]; // skip character.
    case u8'\a': [[fallthrough]]; // skip character.
    case u8'\v': [[fallthrough]]; // skip character.
    case u8'\t': _consume_char(1);        continue;
    case u8'/':  return _token_fwdslash();
    case u8'\0': return _token_null();
    case u8'~':  return _token_tilde();
    case u8'@':  return _token_at();
    case u8'$':  return _token_money();
    case u8'[':  return _token_lsqbrckt();
    case u8']':  return _token_rsqbrckt();
    case u8';':  return _token_semicolon();
    case u8'(':  return _token_lparen();
    case u8')':  return _token_rparen();
    case u8'{':  return _token_lbrace();
    case u8'}':  return _token_rbrace();
    case u8',':  return _token_comma();
    case u8'-':  return _token_hyphen();
    case u8'&':  return _token_ampersand();
    case u8'.':  return _token_dot();
    case u8'!':  return _token_bang();
    case u8'<':  return _token_lthan();
    case u8'>':  return _token_gthan();
    case u8'+':  return _token_plus();
    case u8'*':  return _token_asterisk();
    case u8'%':  return _token_percent();
    case u8'=':  return _token_equals();
    case u8'|':  return _token_pipe();
    case u8'^':  return _token_uparrow();
    case u8'\'': return _token_squote();
    case u8'"':  return _token_quote();
    case u8'`':  return _token_quote();
    case u8':':  return _token_colon();
    default:     return _token_ambiguous();
  }

  UNREACHABLE; // assertion
}

inline auto n19::Lexer::_token_fwdslash() -> Token {
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

inline auto n19::Lexer::_token_quote() -> Token {
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

    // skip escaped quote.
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

inline auto n19::Lexer::_token_hex_lit() -> Token {
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

inline auto n19::Lexer::_token_num_lit() -> Token {
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

    if(curr == '.') {
      if(!CH_IS_DIGIT(next)) {
        break;
      } if(seen_dot || seen_exponent) {
        curr_tok.type_ = TokenType::Illegal;
        curr_tok.cat_  = TokenCategory::NonCategorical;
        return curr_tok;
      }
      seen_dot = true;
    }
    else if(curr == 'e' || curr == 'E') {
      if(seen_exponent) {
        curr_tok.type_ = TokenType::Illegal;
        curr_tok.cat_  = TokenCategory::NonCategorical;
        return curr_tok;
      }
      seen_exponent = true;
      if(next == '-' || next == '+') {
        _consume_char(1);
        if(!CH_IS_DIGIT(_peek_char())) {
          curr_tok.type_ = TokenType::Illegal;
          curr_tok.cat_  = TokenCategory::NonCategorical;
          return curr_tok;
        }
      }
    }
    else if(!CH_IS_DIGIT(_current_char())) {
      break;
    }

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

inline auto n19::Lexer::_token_oct_lit() -> Token {
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

inline auto n19::Lexer::_token_ambiguous() -> Token {
  ASSERT(index_ < src_.size());
  ASSERT(!is_reserved_byte(_current_char()));

  if(CH_IS_DIGIT( _current_char()) ) {
    const char8_t next = _peek_char();
    const char8_t curr = _current_char();
    if(curr == u8'0' && (next == u8'x' || next == u8'X')) {
      return _token_hex_lit();
    } if(curr == u8'0' && CH_IS_DIGIT( next )) {
      return _token_oct_lit();
    } else [[likely]] {
      return _token_num_lit();
    }
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
  const auto keyword_ptr = Lexer::keywords().find(val);

  if(keyword_ptr != Lexer::keywords().end()) {
    curr_tok.type_ = keyword_ptr->second.type;
    curr_tok.cat_  = keyword_ptr->second.cat;
  } else {
    curr_tok.type_ = TokenType::Identifier;
    curr_tok.cat_  = TokenCategory::Identifier;
  }

  return curr_tok;
}

auto n19::Lexer::keywords() -> const std::unordered_map<std::u8string_view, Keyword>& {
  // Compile-time evaluated.
  // We construct the keywords during compilation and
  // store them in our hash table at runtime for lookup later.

  constexpr static auto keywords_ = []() consteval -> auto {
    std::array<std::pair<std::u8string_view, Keyword>, 29> arr{};
    arr[0].first       = u8"return";
    arr[0].second.cat |= TokenCategory::Keyword;
    arr[0].second.cat |= TokenCategory::ControlFlow;
    arr[0].second.type = TokenType::Return;

    arr[1].first       = u8"break";
    arr[1].second.cat |= TokenCategory::Keyword;
    arr[1].second.cat |= TokenCategory::ControlFlow;
    arr[1].second.type = TokenType::Break;

    arr[2].first       = u8"continue";
    arr[2].second.cat |= TokenCategory::Keyword;
    arr[2].second.cat |= TokenCategory::ControlFlow;
    arr[2].second.type = TokenType::Continue;

    arr[3].first       = u8"for";
    arr[3].second.cat |= TokenCategory::Keyword;
    arr[3].second.cat |= TokenCategory::ControlFlow;
    arr[3].second.type = TokenType::For;

    arr[4].first       = u8"while";
    arr[4].second.cat |= TokenCategory::Keyword;
    arr[4].second.cat |= TokenCategory::ControlFlow;
    arr[4].second.type = TokenType::While;

    arr[5].first       = u8"do";
    arr[5].second.cat |= TokenCategory::Keyword;
    arr[5].second.cat |= TokenCategory::ControlFlow;
    arr[5].second.type = TokenType::Do;

    arr[6].first       = u8"if";
    arr[6].second.cat |= TokenCategory::Keyword;
    arr[6].second.cat |= TokenCategory::ControlFlow;
    arr[6].second.type = TokenType::If;

    arr[7].first       = u8"else";
    arr[7].second.cat |= TokenCategory::Keyword;
    arr[7].second.cat |= TokenCategory::ControlFlow;
    arr[7].second.type = TokenType::Else;

    arr[8].first       = u8"struct";
    arr[8].second.cat |= TokenCategory::Keyword;
    arr[8].second.type = TokenType::Struct;

    arr[9].first       = u8"switch";
    arr[9].second.cat |= TokenCategory::Keyword;
    arr[9].second.cat |= TokenCategory::ControlFlow;
    arr[9].second.type = TokenType::Switch;

    arr[10].first       = u8"case";
    arr[10].second.cat |= TokenCategory::Keyword;
    arr[10].second.cat |= TokenCategory::ControlFlow;
    arr[10].second.type = TokenType::Case;

    arr[11].first       = u8"default";
    arr[11].second.cat |= TokenCategory::Keyword;
    arr[11].second.cat |= TokenCategory::ControlFlow;
    arr[11].second.type = TokenType::Default;

    arr[12].first       = u8"block";
    arr[12].second.cat |= TokenCategory::Keyword;
    arr[12].second.type = TokenType::Block;

    arr[13].first       = u8"defer";
    arr[13].second.cat |= TokenCategory::Keyword;
    arr[13].second.type = TokenType::Defer;

    arr[14].first       = u8"defer_if";
    arr[14].second.cat |= TokenCategory::Keyword;
    arr[14].second.type = TokenType::DeferIf;

    arr[15].first       = u8"sizeof";
    arr[15].second.cat |= TokenCategory::Keyword;
    arr[15].second.cat |= TokenCategory::UnaryOp;
    arr[15].second.cat |= TokenCategory::ValidPrefix;
    arr[15].second.type = TokenType::Sizeof;

    arr[16].first       = u8"typeof";
    arr[16].second.cat |= TokenCategory::Keyword;
    arr[16].second.cat |= TokenCategory::UnaryOp;
    arr[16].second.cat |= TokenCategory::ValidPrefix;
    arr[16].second.type = TokenType::Typeof;

    arr[17].first       = u8"fallthrough";
    arr[17].second.cat |= TokenCategory::Keyword;
    arr[17].second.cat |= TokenCategory::ControlFlow;
    arr[17].second.type = TokenType::Fallthrough;

    arr[18].first       = u8"namespace";
    arr[18].second.cat |= TokenCategory::Keyword;
    arr[18].second.type = TokenType::Namespace;

    arr[19].first       = u8"where";
    arr[19].second.cat |= TokenCategory::Keyword;
    arr[19].second.type = TokenType::Where;

    arr[20].first       = u8"otherwise";
    arr[20].second.cat |= TokenCategory::Keyword;
    arr[20].second.type = TokenType::Otherwise;

    arr[21].first       = u8"proc";
    arr[21].second.cat |= TokenCategory::Keyword;
    arr[21].second.type = TokenType::Proc;

    arr[22].first       = u8"let";
    arr[22].second.cat |= TokenCategory::Keyword;
    arr[22].second.type = TokenType::Let;

    arr[23].first       = u8"const";
    arr[23].second.cat |= TokenCategory::Keyword;
    arr[23].second.type = TokenType::Const;

    arr[24].first       = u8"as";
    arr[24].second.cat |= TokenCategory::Keyword;
    arr[24].second.cat |= TokenCategory::ValidPostfix;
    arr[24].second.cat |= TokenCategory::UnaryOp;
    arr[24].second.type = TokenType::As;

    arr[25].first       = u8"with";
    arr[25].second.cat |= TokenCategory::Keyword;
    arr[25].second.cat |= TokenCategory::ValidPostfix;
    arr[25].second.cat |= TokenCategory::UnaryOp;
    arr[25].second.type = TokenType::With;

    arr[26].first       = u8"true";
    arr[26].second.cat  = TokenCategory::Literal;
    arr[26].second.type = TokenType::BooleanLiteral;

    arr[27].first       = u8"false";
    arr[27].second.cat  = TokenCategory::Literal;
    arr[27].second.type = TokenType::BooleanLiteral;

    arr[28].first       = u8"null";
    arr[28].second.cat  = TokenCategory::Literal;
    arr[28].second.type = TokenType::NullLiteral;
    return arr;
  }();

  static const std::unordered_map kw_list{keywords_.begin(), keywords_.end()};
  return kw_list;
}

auto n19::Lexer::is_reserved_byte(const char8_t c) -> bool {
  constexpr static auto bytes = []() consteval -> auto {
    constexpr char8_t chars[] = {
      u8';', u8'(', u8')', u8'{', u8'}', u8',', u8'-', u8'+', u8'*', u8'/',
      u8'%', u8'=', u8'<', u8'>', u8'&', u8'|', u8'!', u8'~', u8'^', u8'\'',
      u8'"', u8'`', u8'[', u8']', u8'?', u8':', u8'#', u8'@', u8'.', u8'\\',
      u8'$', u8'\0'
    };

    std::array<bool, 256> table = { false };
    for(const char8_t ch : chars) {
      table[ch] = true;
    }

    return table;
  }();

  return bytes[ c ] == true;
}

inline auto n19::Lexer::_skip_chars_until(std::function<bool(char8_t)> cb) -> bool {
  while(!cb(_current_char()) && _current_char() != '\0') {
    if(_current_char() == '\n') _advance_consume_line();
    else if(UTF8_LEADING(_current_char())) _skip_utf8_sequence();
    else _consume_char(1);
  }
  return _current_char() != '\0';
}

inline auto n19::Lexer::_skip_utf8_sequence() -> bool {
  const auto ch = static_cast<uint8_t>(_current_char());
  if      ((ch & 0xE0) == 0xC0) _consume_char(2);
  else if ((ch & 0xF0) == 0xE0) _consume_char(3);
  else if ((ch & 0xF8) == 0xF0) _consume_char(4);
  else return false;

  return index_ - 1 < src_.size();
}

auto n19::Lexer::create(const FileRef& ref) -> Result<std::shared_ptr<Lexer>> {
  const auto fsize = ref.size().OR_RETURN();
  if(*fsize >= std::numeric_limits<uint32_t>::max()) [[unlikely]] {
    return make_error(ErrC::InvalidArg, "Provided file is too large.");
  } if(*fsize == 0) {
    return make_error(ErrC::InvalidArg, "Provided file is empty.");
  }

  auto lxr = std::make_shared<Lexer>();
  lxr->file_name_ = fs::absolute(*ref).string();
  lxr->src_.resize(*fsize);

  ref.read_into(n19::as_writable_bytes(lxr->src_)).OR_RETURN();
  if(lxr->src_.size() > 3
    && static_cast<uint8_t>( lxr->src_[0] ) == 0xEF
    && static_cast<uint8_t>( lxr->src_[1] ) == 0xBB
    && static_cast<uint8_t>( lxr->src_[2] ) == 0xBF )
  {
    lxr->index_ = 3; // Skip UTF8 Byte Order Mark if it exists.
  }

  return make_result<std::shared_ptr<Lexer>>(lxr);
}

auto n19::Lexer::dump() -> void {
  Token curr_tok;
  do {
    curr_tok = _produce_impl();
  } while(curr_tok != TokenType::EndOfFile && curr_tok != TokenType::Illegal);
}

auto n19::Lexer::expect(const TokenCategory cat, const bool cons) -> Result<None> {
  if(const auto curr = current(); curr.cat_ != cat) {
    return make_error(
      ErrC::BadToken, "Unexpected token \"{}\" (expected \"{}\").",
      current().cat_.to_string(), cat.to_string());
  }
  if(cons) consume(1);
  return make_result<None>();
}

auto n19::Lexer::expect(const TokenType type, const bool cons) -> Result<None> {
  if(const auto curr = current(); curr.type_ != type) {
    return make_error(
      ErrC::BadToken, "Unexpected token \"{}\" (expected \"{}\").",
      curr.cat_.to_string(), type.to_string());
  }
  if(cons) consume(1);
  return make_result<None>();
}

auto n19::Lexer::consume(const uint32_t amnt) -> Token {
  if(const auto curr = current(); curr == TokenType::EndOfFile) {
    return curr;
  }

  Token curr_tok;
  for(uint32_t i = 0; i < amnt; i++) {
    curr_tok = toks_.dequeue();
    if(curr_tok == TokenType::EndOfFile)
      break;
  }

  return curr_tok;
}