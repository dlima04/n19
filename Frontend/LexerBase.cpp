/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#include <Frontend/LexerBase.hpp>
#include <Frontend/ErrorCollector.hpp>

#define KEYWORD_REG(STR, TYPE) {{STR}, {TYPE,  TokenCategory::Keyword}}
#define KEYWORD_LIT(STR, TYPE) {{STR}, {TYPE,  TokenCategory::Literal}}

auto n19::LexerBase::keywords() -> const KeywordMap& {
  static const KeywordMap kw_list {
    KEYWORD_REG("return", TokenType::Return),
    KEYWORD_REG("break", TokenType::Break),
    KEYWORD_REG("continue", TokenType::Continue),
    KEYWORD_REG("for", TokenType::For),
    KEYWORD_REG("while", TokenType::While),
    KEYWORD_REG("do", TokenType::Do),
    KEYWORD_REG("if", TokenType::If),
    KEYWORD_REG("else", TokenType::Else),
    KEYWORD_REG("struct", TokenType::Struct),
    KEYWORD_REG("switch", TokenType::Switch),
    KEYWORD_REG("case", TokenType::Case),
    KEYWORD_REG("default", TokenType::Default),
    KEYWORD_REG("block", TokenType::Block),
    KEYWORD_REG("defer", TokenType::Defer),
    KEYWORD_REG("defer_if", TokenType::DeferIf),
    KEYWORD_REG("sizeof", TokenType::Sizeof),
    KEYWORD_REG("typeof", TokenType::Typeof),
    KEYWORD_REG("fallthrough", TokenType::Fallthrough),
    KEYWORD_REG("namespace", TokenType::Namespace),
    KEYWORD_REG("where", TokenType::Where),
    KEYWORD_REG("otherwise", TokenType::Otherwise),
    KEYWORD_REG("proc", TokenType::Proc),
    KEYWORD_REG("let", TokenType::Let),
    KEYWORD_REG("const", TokenType::Const),
    KEYWORD_REG("as", TokenType::As),
    KEYWORD_REG("with",TokenType::With),
    KEYWORD_LIT("true",TokenType::BooleanLiteral),
    KEYWORD_LIT("false",TokenType::BooleanLiteral),
    KEYWORD_LIT("null",TokenType::NullLiteral),
  };

  #undef KEYWORD_REG
  #undef KEYWORD_LIT
  return kw_list;
}

auto n19::LexerBase::is_reserved_byte(const char c) -> bool {
  constexpr static auto bytes = []() -> auto constexpr {
    std::array<bool, 256> table = { false };
    table[' ']  = true;
    table['\r'] = true;
    table['\b'] = true;
    table['\t'] = true;
    table['\n'] = true;
    table[';']  = true;
    table['(']  = true;
    table[')']  = true;
    table['{']  = true;
    table['}']  = true;
    table[',']  = true;
    table['-']  = true;
    table['+']  = true;
    table['*']  = true;
    table['/']  = true;
    table['%']  = true;
    table['=']  = true;
    table['<']  = true;
    table['>']  = true;
    table['&']  = true;
    table['|']  = true;
    table['!']  = true;
    table['~']  = true;
    table['^']  = true;
    table['\''] = true;
    table['"']  = true;
    table['`']  = true;
    table['[']  = true;
    table[']']  = true;
    table['?']  = true;
    table[':']  = true;
    table['#']  = true;
    table['@']  = true;
    table['.']  = true;
    table['\\'] = true;
    table['$']  = true;
    table['\0'] = true;
    return table;
  }();

  return bytes[ c ] == true;
}
