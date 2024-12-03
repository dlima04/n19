/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#include <algorithm>
#include <Frontend/Lexer.hpp>
#include <Core/Fmt.hpp>
#include <Core/Panic.hpp>

auto n19::Token::eof(
  const uint32_t pos,
  const uint32_t line ) -> Token
{
  Token token;
  token.pos_   = pos;
  token.line_  = line;
  token.type_  = TokenType::EndOfFile;
  token.cat_   = TokenCategory::NonCategorical;
  return token;
}

auto n19::Token::illegal(
  const uint32_t pos,
  const uint32_t length,
  const uint32_t line ) -> Token
{
  Token token;
  token.pos_   = pos;
  token.line_  = line;
  token.len_   = length;
  token.cat_   = TokenCategory::NonCategorical;
  token.type_  = TokenType::Illegal;
  return token;
}

// Converts a given TokenType's underlying
// Type enumeration to a string.
auto n19::TokenType::to_string() const -> std::string {
  #define X(TYPE, STR) case TokenType::TYPE: return #TYPE;
  switch(value) {
    N19_TOKEN_TYPE_LIST
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
auto n19::TokenType::string_repr() const -> std::string {
#define X(TYPE, STR) case TokenType::TYPE: return STR;
  switch(value) {
    N19_TOKEN_TYPE_LIST
    default: return "Unknown"; // Failsafe
  }
#undef X
}

// Converts a given TokenCategory's underlying
// Type enumeration to a string.
auto n19::TokenCategory::to_string() const -> std::string {
  #define X(CAT, UNUSED) if(value & CAT) buff += (std::string(#CAT) + " | ");
   std::string buff;
   N19_TOKEN_CATEGORY_LIST
  #undef X

  if(!buff.empty() && buff[buff.size() - 2] == '|') {
    buff.erase(buff.size() - 3);
  } else {
    buff += "NonCategorical";
  }

  return buff;
}

auto n19::TokenCategory::is_any_of(const std::vector<Value>& vals) const -> bool {
  const auto itr = std::ranges::find_if(vals, [&](const Value& val) {
    return isa(val);
  });

  return itr != vals.end();
}

// Gets a given token's "value". This is the exact
// way in which it appears in a source file. For example,
// an identifier of "foo" would be returned as such,
// a string of "foo".
auto n19::Token::value(const Lexer& lxr) const -> Maybe<std::string> {
  if(len_ == 0) return std::nullopt;
  const auto bytes = lxr.get_bytes();
  ASSERT(pos_ < bytes.size());
  ASSERT(((pos_ + len_) - 1) < bytes.size());
  return std::string((char*)&bytes[pos_], len_);
}

// Formats a token into a more readable representation.
// For debugging/testing purposes only.
auto n19::Token::format(const Lexer& lxr) const -> std::string {
  std::string buffer;
  buffer += fmt("{:<10}: \"{}\"\n", type_.to_string(), value(lxr).value_or("N/A"));
  buffer += fmt("{:<10}: {}\n", "Line", line_);
  buffer += fmt("{:<10}: {}\n", "Position", pos_);
  buffer += fmt("{:<10}: {}\n", "Category", cat_.to_string());
  buffer += '\n';
  return buffer;
}

auto n19::TokenType::maybe_entity_begin() const -> bool {
  return value == NamespaceOperator || value == Identifier;
}

auto n19::TokenCategory::isa(const Value val) const -> bool {
  return this->value & val;
}