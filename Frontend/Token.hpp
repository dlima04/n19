/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef FRONTEND_TOKEN_HPP
#define FRONTEND_TOKEN_HPP
#include <string>
#include <string_view>
#include <cstdint>
#include <vector>

#define N19_TOKEN_TYPE_LIST      \
  X(None, "")                    \
  X(EndOfFile, "\\0")            \
  X(Illegal, "")                 \
  X(Identifier, "")              \
  X(ValueAssignment, "=")        \
  X(TypeAssignment, ":")         \
  X(NamespaceOperator, "::")     \
  X(Semicolon, ";")              \
  X(LeftParen, "(")              \
  X(RightParen, ")")             \
  X(LeftBrace, "{")              \
  X(RightBrace, "}")             \
  X(LeftSqBracket, "[")          \
  X(RightSqBracket, "]")         \
  X(Comma, ",")                  \
  X(Dot, ".")                    \
  X(DotTwo, "..")                \
  X(DotThree, "...")             \
  X(Huh, "?")                    \
  X(Backslash, "\\")             \
  X(Money, "$")                  \
  X(Pound, "#")                  \
  X(At, "@")                     \
  X(Eq, "==")                    \
  X(Neq, "!=")                   \
  X(Lt, "<")                     \
  X(Lte, "<=")                   \
  X(Gt, ">")                     \
  X(Gte, ">=")                   \
  X(LogicalAnd, "&&")            \
  X(LogicalOr, "||")             \
  X(LogicalNot, "!")             \
  X(IntLiteral, "")              \
  X(FloatLiteral, "")            \
  X(HexLiteral, "")              \
  X(OctalLiteral, "")            \
  X(ByteLiteral, "")             \
  X(BooleanLiteral, "")          \
  X(StringLiteral, "")           \
  X(Plus, "+")                   \
  X(PlusEq, "+=")                \
  X(Sub, "-")                    \
  X(SubEq, "-=")                 \
  X(Mul, "*")                    \
  X(MulEq, "*=")                 \
  X(Div, "/")                    \
  X(DivEq, "/=")                 \
  X(Mod, "%")                    \
  X(ModEq, "%=")                 \
  X(Inc, "++")                   \
  X(Dec, "--")                   \
  X(BitwiseNot, "~")             \
  X(BitwiseAnd, "&")             \
  X(BitwiseAndEq, "&=")          \
  X(BitwiseOr, "|")              \
  X(BitwiseOrEq, "|=")           \
  X(Xor, "^")                    \
  X(XorEq, "^=")                 \
  X(Lshift, "<<")                \
  X(LshiftEq, "<<=")             \
  X(Rshift, ">>")                \
  X(RshiftEq, ">>=")             \
  X(Proc, "proc")                \
  X(Let, "let")                  \
  X(Const, "const")              \
  X(NullLiteral, "null")         \
  X(Return, "return")            \
  X(Break, "break")              \
  X(Continue, "continue")        \
  X(For, "for")                  \
  X(While, "while")              \
  X(Do, "do")                    \
  X(If, "if")                    \
  X(Else, "else")                \
  X(Struct, "struct")            \
  X(Enum, "enum")                \
  X(Switch, "switch")            \
  X(Case, "case")                \
  X(Default, "default")          \
  X(Fallthrough, "fallthrough")  \
  X(Namespace, "namespace")      \
  X(Defer, "defer")              \
  X(DeferIf, "defer_if")         \
  X(Block, "block")              \
  X(Typeof, "typeof")            \
  X(Sizeof, "sizeof")            \
  X(As, "as")                    \
  X(Where, "where")              \
  X(With, "with")                \
  X(Otherwise, "otherwise")      \
  X(SkinnyArrow, "->")           \
  X(FatArrow, "=>")              \

#define N19_TOKEN_CATEGORY_LIST  \
  X(NonCategorical, 0ULL)        \
  X(Punctuator, 1ULL)            \
  X(UnaryOp, 1ULL << 1)          \
  X(BinaryOp, 1ULL << 2)         \
  X(Literal, 1ULL << 4)          \
  X(Keyword, 1ULL << 5)          \
  X(Identifier, 1ULL << 6)       \
  X(LogicalOp, 1ULL << 7)        \
  X(ArithmeticOp, 1ULL << 8)     \
  X(BitwiseOp, 1ULL << 9)        \
  X(ComparisonOp, 1ULL << 10)    \
  X(ArithAssignOp, 1ULL << 11)   \
  X(PointerArithOp, 1ULL << 12)  \
  X(ValidPostfix, 1ULL << 13)    \
  X(ValidPrefix, 1ULL << 14)     \
  X(BitwiseAssignOp, 1ULL << 15) \
  X(Terminator, 1ULL << 16)      \

namespace n19 {
  class Token;
  struct TokenType;
  struct TokenCategory;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct n19::TokenType {
  #define X(TOKEN_TYPE, STR_UNUSED) TOKEN_TYPE,
  enum Value : uint16_t {
    N19_TOKEN_TYPE_LIST
  };
  #undef X

  [[nodiscard]] auto to_string() const -> std::string;
  [[nodiscard]] auto maybe_entity_begin() const -> bool;
  auto operator==(const TokenType& other) const -> bool;
  auto operator!=(const TokenType& other) const -> bool;

  Value value  = None;
  TokenType()  = default;
  ~TokenType() = default;
  TokenType(const Value value) : value(value) {}
};

struct n19::TokenCategory {
  #define X(CAT, MASK) CAT = MASK,
  enum Value : size_t {
    N19_TOKEN_CATEGORY_LIST
  };
  #undef X

  [[nodiscard]] auto to_string() const -> std::string;
  [[nodiscard]] auto isa(Value val) const -> bool;
  [[nodiscard]] auto is_any_of(const std::vector<Value> &vals) const -> bool;

  auto operator|=(const TokenCategory &other) -> void;
  auto operator|=(Value other) -> void;
  auto operator==(const TokenCategory& other) const -> bool;
  auto operator!=(const TokenCategory& other) const -> bool;

  size_t value     = NonCategorical;
  TokenCategory()  = default;
  ~TokenCategory() = default;
  TokenCategory(const size_t value)
    : value(static_cast<Value>(value)) {}
};

class n19::Token {
public:
  TokenType type_    = TokenType::None;
  TokenCategory cat_ = TokenCategory::NonCategorical;
  size_t pos_        = 0;
  uint32_t line_     = 0;
  std::string_view value_;

  [[nodiscard]] auto format() const -> std::string;
  [[nodiscard]] auto to_string() const -> std::string;

  auto operator==(const TokenType other) const -> bool;
  auto operator==(const Token &other)    const -> bool;
  auto operator!=(const TokenType other) const -> bool;
  auto operator!=(const Token &other)    const -> bool;

  static auto eof(size_t pos, uint32_t line) -> Token;
  static auto illegal(size_t pos, uint32_t line, const std::string_view &str) -> Token;

  Token()  = default;
  ~Token() = default;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Inline operator functions

inline auto n19::TokenCategory::operator|=(
  const TokenCategory &other ) -> void
{
  value |= other.value;
}

inline auto n19::TokenCategory::operator|=(
  const Value other ) -> void
{
  value |= other;
}

inline auto n19::TokenCategory::operator!=(
  const TokenCategory& other ) const -> bool
{
  return other.value != this->value;
}

inline auto n19::TokenCategory::operator==(
  const TokenCategory& other ) const -> bool
{
  return other.value == this->value;
}

inline auto n19::TokenType::operator!=(
  const TokenType& other ) const -> bool
{
  return other.value != this->value;
}

inline auto n19::TokenType::operator==(
  const TokenType& other ) const -> bool
{
  return other.value == this->value;
}

inline auto n19::Token::operator==(
  const TokenType other ) const -> bool
{
  return type_ == other;
}

inline auto n19::Token::operator==(
  const Token &other ) const -> bool
{
  return other.type_ == this->type_;
}

inline auto n19::Token::operator!=(
  const Token &other ) const -> bool
{
  return other.type_ != type_;
}

inline auto n19::Token::operator!=(
  const TokenType other ) const -> bool
{
  return other != type_;
}

#endif //FRONTEND_TOKEN_HPP
