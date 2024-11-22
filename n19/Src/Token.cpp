/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#include <algorithm>
#include <Token.h>
#include <Fmt.h>

auto n19::Token::to_string() const -> std::string {
  #define X(TYPE, STR) case TokenType::TYPE: return STR;
  switch(type_.value) {
    N19_TOKEN_TYPE_LIST
    default: return "Unknown"; // Failsafe
  }
  #undef X
}

auto n19::TokenType::to_string() const -> std::string {
  #define X(TYPE, STR) case TokenType::TYPE: return #TYPE;
  switch(value) {
    N19_TOKEN_TYPE_LIST
    default: return "Unknown"; // Failsafe
  }
  #undef X
}

auto n19::TokenCategory::to_string() const -> std::string {
  #define X(CAT, UNUSED) if(value & CAT) buff += (std::string(#CAT) + " | ");
  std::string buff;
  N19_TOKEN_CATEGORY_LIST
  if(!buff.empty() && buff[buff.size() - 2] == '|') {
    buff.erase(buff.size() - 3);
  }
  return buff;
  #undef X
}

auto n19::TokenType::maybe_entity_begin() const
-> bool {
  return value == NamespaceOperator || value == Identifier;
}

auto n19::TokenCategory::isa(const Value val) const
-> bool {
  return this->value & val;
}

auto n19::TokenCategory::is_any_of(const std::vector<Value>& vals) const
-> bool {
  return std::ranges::find_if(vals, [&](const Value& val) {
    return isa(val);
  }) != vals.end();
}

auto n19::Token::format() const -> std::string {
  std::string buffer;
  buffer += fmt("{:<10}: \"{}\"\n", type_.to_string(), value_);
  buffer += fmt("{:<10}: {}\n", "Line", line_);
  buffer += fmt("{:<10}: {}\n", "Position", pos_);
  buffer += fmt("{:<10}: {}\n", "Category", cat_.to_string());
  buffer += '\n';
  return buffer;
}

auto n19::Token::eof(
  const size_t pos,
  const uint32_t line )
-> Token {
  return {
    pos,
    line,
    TokenType::EndOfFile,
    TokenCategory::NonCategorical,
    "\\0"
  };
}

auto n19::Token::illegal(
  const size_t pos,
  const uint32_t line,
  const std::string_view &str )
-> Token {
  return {
    pos,
    line,
    TokenType::Illegal,
    TokenCategory::NonCategorical,
    str
  };
}
