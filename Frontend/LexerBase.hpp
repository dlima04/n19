/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef LEXERBASE_HPP
#define LEXERBASE_HPP
#include <Core/Bytes.hpp>
#include <Core/Platform.hpp>
#include <Frontend/Token.hpp>
#include <string_view>
#include <unordered_map>
#include <cctype>
#include <cstdint>

#define UTF8_LEADING(CH)     (((uint8_t)CH) >= 0x80)
#define CHAR_IS_HEXDIGIT(CH) (std::isxdigit((uint8_t)CH)
#define CHAR_IS_DIGIT(CH)    (std::isdigit((uint8_t)CH)

namespace n19 {
  class LexerBase;
  struct Keyword;
  using KeywordMap =
  std::unordered_map<std::string_view, Keyword>;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class n19::LexerBase {
public:
  virtual auto current() const          -> const Token& = 0;
  virtual auto consume(uint32_t amnt)   -> const Token& = 0;
  virtual auto peek(uint32_t amnt)      -> const Token& = 0;
  virtual auto consume_u(TokenCategory) -> const Token& = 0;
  virtual auto consume_u(TokenType)     -> const Token& = 0;
  virtual auto get_bytes() const        -> Bytes = 0;

  static auto is_reserved_byte(char c) -> bool;
  static auto keywords() -> const KeywordMap&;
  virtual ~LexerBase() = default;
protected:
  size_t index_  = 0;
  uint32_t line_ = 1;
};

struct n19::Keyword {
  TokenType type;
  TokenCategory cat;
};

#endif //LEXERBASE_HPP