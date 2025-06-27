/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Frontend/Lexer/Lexer.hpp>
#include <Core/Fmt.hpp>
#include <Core/Panic.hpp>
#include <Core/Murmur3.hpp>
#include <algorithm>
#include <new>
BEGIN_NAMESPACE(rl);

auto Token::eof(
  const uint32_t pos, const uint32_t line ) -> Token
{
  Token token;
  token.pos_   = pos;
  token.line_  = line;
  token.cat_   = TokenCategory::NonCategorical;
  token.type_  = TokenType::EndOfFile;
  return token;
}

auto Token::illegal(
  const uint32_t pos,
  const uint32_t length, const uint32_t line ) -> Token
{
  Token token;
  token.len_   = length;
  token.pos_   = pos;
  token.line_  = line;
  token.cat_   = TokenCategory::NonCategorical;
  token.type_  = TokenType::Illegal;
  return token;
}

// Converts a given TokenType's underlying
// Type enumeration to a string.
auto TokenType::to_string() const -> std::string {
  #define X(TYPE, STR) case TokenType::TYPE: return #TYPE;
  switch(value) {
    RL_TOKEN_TYPE_LIST
    default: return "Unknown"; // Failsafe
  }
  #undef X
}

// Converts a TokenType to it's given string
// representation. This is different from converting
// the underlying type, for example:
// - TokenType::LogicalAnd becomes "&&".
// - TokenType::PlusEq becomes "+="
// etc.
auto TokenType::string_repr() const -> std::string {
#define X(TYPE, STR) case TokenType::TYPE: return STR;
  switch(value) {
    RL_TOKEN_TYPE_LIST
    default: return "Unknown"; // Failsafe
  }
#undef X
}

// Converts a given TokenCategory's underlying
// Type enumeration to a string.
auto TokenCategory::to_string() const -> std::string {
  #define X(CAT, UNUSED) if(value & CAT) buff += (std::string(#CAT) + " | ");
   std::string buff;
   RL_TOKEN_CATEGORY_LIST
  #undef X

  if(!buff.empty() && buff[buff.size() - 2] == '|') {
    buff.erase(buff.size() - 3);
  } else {
    buff += "NonCategorical";
  }

  return buff;
}

// Retrieves a TokenCategory value, using a given string
// which represents a keyword. The keyword may or may not exist.
// returns the category if it exists, or Nothing otherwise.
auto TokenCategory::from_keyword(const std::u8string_view& str)
-> Maybe<TokenCategory>
{
  constexpr uint32_t seed = 0xbeef;
  if(str.size() > 15) return Nothing;

  switch(murmur3_x86_32(str, seed)) {
#define KEYWORD_X(NAME, UNUSED, CAT) case u8##NAME##_mm32: return CAT;
    LR_KEYWORDS; default: break;
#undef KEYWORD_X
  }

  return Nothing;
}

// Gets a given token's "value". This is the exact
// way in which it appears in a source file. For example,
// an identifier of "foo" would be returned as such,
// a string of "foo". For the plus op it would return "+".
auto Token::value(const Lexer& lxr) const -> Maybe<std::string> {
  if(len_ == 0) return Nothing;
  const auto bytes = lxr.get_bytes();
  ASSERT(pos_ < bytes.size());
  ASSERT(pos_ + len_ - 1 < bytes.size());

  std::string ret;
  ret.reserve(len_);

  for(size_t i = pos_, j = 0; j < len_; i++, j++) {
    auto ch = bytes[i];
    ret += (static_cast<char>(ch));
  }
  return ret;
}

// Formats a token into a more readable representation.
// For debugging/testing purposes only.
auto Token::format(const Lexer &lxr) const -> std::string {
  std::string buffer;
  buffer += fmt("{:<12}: ", type_.to_string());
  buffer += fmt("\"{}\" -- ", value(lxr).value_or("N/A"));
  buffer += fmt("LINE={},POS={} -- ", line_, pos_);
  buffer += fmt("{}\n", cat_.to_string());
  return buffer;
}

// Retrieves a TokenType value, using a given string
// which represents a keyword. The keyword may or may not exist.
// returns the type if it exists, or Nothing otherwise.
auto TokenType::from_keyword(const std::u8string_view& keyword)
-> Maybe<TokenType>
{
  constexpr uint32_t seed = 0xbeef;
  if(keyword.size() > 15) return Nothing;

  switch(murmur3_x86_32(keyword, seed)) {
#define KEYWORD_X(NAME, TYPE, UNUSED) case u8##NAME##_mm32: return TYPE;
  LR_KEYWORDS; default: break;
#undef KEYWORD_X
  }

  return Nothing;
}

// is this token a terminating token? (';', ',')
auto Token::is_terminator() const -> bool {
  return type_ == TokenType::Semicolon || type_ == TokenType::Comma;
}

// TODO
// operator precedence of a given token.
auto TokenType::prec() const -> Precedence::Value {
  switch(this->value) {
  case ValueAssignment:   return Precedence::max;
  case PlusEq:            FALLTHROUGH_;
  case SubEq:             FALLTHROUGH_;
  case MulEq:             FALLTHROUGH_;
  case DivEq:             FALLTHROUGH_;
  case ModEq:             FALLTHROUGH_;
  case LshiftEq:          FALLTHROUGH_;
  case RshiftEq:          FALLTHROUGH_;
  case BitwiseAndEq:      FALLTHROUGH_;
  case BitwiseOrEq:       FALLTHROUGH_;
  case XorEq:             return Precedence::max - 1;
  case LogicalAnd:        return Precedence::max - 2;
  case LogicalOr:         return Precedence::max - 3;
  case Mul:               FALLTHROUGH_;
  case Div:               FALLTHROUGH_;
  case Mod:               return Precedence::max - 4;
  case Plus:              FALLTHROUGH_;
  case Sub:               return Precedence::max - 5;
  case Lshift:            FALLTHROUGH_;
  case Rshift:            return Precedence::max - 6;
  case Lt:                FALLTHROUGH_;
  case Lte:               FALLTHROUGH_;
  case Gt:                FALLTHROUGH_;
  case Gte:               return Precedence::max - 7;
  case Eq:                FALLTHROUGH_;
  case Neq:               return Precedence::max - 8;
  case BitwiseAnd:        return Precedence::max - 9;
  case Xor:               return Precedence::max - 10;
  case BitwiseOr:         return Precedence::max - 11;
  case Dot:               FALLTHROUGH_;
  case SkinnyArrow:       return Precedence::max - 12;
  case NamespaceOperator: return Precedence::max - 13;
  default: break;
  }

  PANIC("TokenType::prec(): default assertion.");
}

END_NAMESPACE(rl);