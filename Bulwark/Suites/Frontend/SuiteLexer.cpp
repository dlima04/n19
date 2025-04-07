/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Bulwark/Bulwark.hpp>
#include <Frontend/Lexer.hpp>
#include <Frontend/Token.hpp>
#include <vector>
#include <string>
using namespace n19;

/// Test suite for the HIR lexer class.
/// TODO: add tests for UTF-8 and weird byte combinations!
///       also possibly weird string/character sequences like 'aadad'

static auto create_lexer(const std::string& source) -> std::shared_ptr<Lexer> {
  std::vector<char8_t> buffer;
  buffer.reserve(source.size());
  for(auto c : source) {
    buffer.push_back(static_cast<char8_t>(c));
  }

  return Lexer::create_shared(std::move(buffer)).value();
}

TEST_CASE(Lexer, BasicTokenRecognition) {
  SECTION(SimpleTokens, {
    auto lexer = create_lexer("(){}[];,");
    
    REQUIRE(lexer->current().type_ == TokenType::LeftParen);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::RightParen);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::LeftBrace);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::RightBrace);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::LeftSqBracket);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::RightSqBracket);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::Semicolon);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::Comma);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::EndOfFile);
  });

  SECTION(Operators, {
    auto lexer = create_lexer("+ - * / % == != < > <= >=&|^");
    
    REQUIRE(lexer->current().type_ == TokenType::Plus);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::Sub);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::Mul);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::Div);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::Mod);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::Eq);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::Neq);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::Lt);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::Gt);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::Lte);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::Gte);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::BitwiseAnd);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::BitwiseOr);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::Xor);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::EndOfFile);
  });

  SECTION(CompoundOperators, {
    auto lexer = create_lexer("+= -= *= /= %= &= |= ^= << >>");
    
    REQUIRE(lexer->current().type_ == TokenType::PlusEq);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::SubEq);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::MulEq);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::DivEq);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::ModEq);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::BitwiseAndEq);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::BitwiseOrEq);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::XorEq);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::Lshift);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::Rshift);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::EndOfFile);
  });
}

TEST_CASE(Lexer, Literals) {
  SECTION(IntegerLiterals, {
    auto lexer = create_lexer("42 0 123456789");
    
    REQUIRE(lexer->current().type_ == TokenType::IntLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "42");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::IntLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "0");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::IntLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "123456789");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::EndOfFile);
  });

  SECTION(FloatLiterals, {
    auto lexer = create_lexer("3.14 0.0 1e10 1.2e-3");
    
    REQUIRE(lexer->current().type_ == TokenType::FloatLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "3.14");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::FloatLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "0.0");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::FloatLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "1e10");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::FloatLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "1.2e-3");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::EndOfFile);
  });

  SECTION(HexLiterals, {
    auto lexer = create_lexer("0x42 0xFF 0xABCD");
    
    REQUIRE(lexer->current().type_ == TokenType::HexLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "0x42");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::HexLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "0xFF");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::HexLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "0xABCD");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::EndOfFile);
  });

  SECTION(OctalLiterals, {
    auto lexer = create_lexer("042 0777");
    
    REQUIRE(lexer->current().type_ == TokenType::OctalLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "042");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::OctalLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "0777");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::EndOfFile);
  });

  SECTION(StringLiterals, {
    auto lexer = create_lexer("\"hello\" \"world\" \"escaped\\\"quote\"");

    REQUIRE(lexer->current().type_ == TokenType::StringLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "\"hello\"");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::StringLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "\"world\"");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::StringLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "\"escaped\\\"quote\"");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::EndOfFile);
  });

  SECTION(BooleanLiterals, {
    auto lexer = create_lexer("true false");
    
    REQUIRE(lexer->current().type_ == TokenType::BooleanLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "true");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::BooleanLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "false");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::EndOfFile);
  });

  SECTION(NullLiteral, {
    auto lexer = create_lexer("null");
    
    REQUIRE(lexer->current().type_ == TokenType::NullLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "null");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::EndOfFile);
  });
}

TEST_CASE(Lexer, IdentifiersAndKeywords) {
  SECTION(Identifiers, {
    auto lexer = create_lexer("foo bar123 _underscore");
    
    REQUIRE(lexer->current().type_ == TokenType::Identifier);
    REQUIRE(lexer->current().value(*lexer).value() == "foo");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::Identifier);
    REQUIRE(lexer->current().value(*lexer).value() == "bar123");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::Identifier);
    REQUIRE(lexer->current().value(*lexer).value() == "_underscore");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::EndOfFile);
  });

  SECTION(Keywords, {
    auto lexer = create_lexer("proc let const if else while for return");
    
    REQUIRE(lexer->current().type_ == TokenType::Proc);
    REQUIRE(lexer->current().value(*lexer).value() == "proc");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::Let);
    REQUIRE(lexer->current().value(*lexer).value() == "let");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::Const);
    REQUIRE(lexer->current().value(*lexer).value() == "const");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::If);
    REQUIRE(lexer->current().value(*lexer).value() == "if");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::Else);
    REQUIRE(lexer->current().value(*lexer).value() == "else");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::While);
    REQUIRE(lexer->current().value(*lexer).value() == "while");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::For);
    REQUIRE(lexer->current().value(*lexer).value() == "for");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::Return);
    REQUIRE(lexer->current().value(*lexer).value() == "return");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::EndOfFile);
  });
}

TEST_CASE(Lexer, Peeking) {
  SECTION(PeekSingleToken, {
    auto lexer = create_lexer("42 + 10");
    
    REQUIRE(lexer->current().type_ == TokenType::IntLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "42");
    
    // Peek at the next token without consuming
    auto peeked = lexer->peek(1);
    REQUIRE(peeked.type_ == TokenType::Plus);
    
    // Current token should still be the same
    REQUIRE(lexer->current().type_ == TokenType::IntLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "42");
    
    // Now consume and verify
    lexer->consume(1);
    REQUIRE(lexer->current().type_ == TokenType::Plus);
  });

  SECTION(PeekMultipleTokens, {
    auto lexer = create_lexer("42 + 10 * 5");
    
    REQUIRE(lexer->current().type_ == TokenType::IntLiteral);
    
    // Peek at multiple tokens
    auto peeked = lexer->peek(3);
    REQUIRE(peeked.type_ == TokenType::Mul);
    
    // Current token should still be the same
    REQUIRE(lexer->current().type_ == TokenType::IntLiteral);
    
    // Consume and verify
    lexer->consume(3);
    REQUIRE(lexer->current().type_ == TokenType::Mul);
  });

  SECTION(BatchedPeek, {
    auto lexer = create_lexer("42 + 10 * 5");

    auto tok = lexer->current();
    REQUIRE(tok.type_ == TokenType::IntLiteral);

    // Use batched_peek to get multiple tokens
    auto peeked = lexer->batched_peek<3>();
    REQUIRE(peeked[0].type_ == TokenType::Plus);
    REQUIRE(peeked[1].type_ == TokenType::IntLiteral);
    REQUIRE(peeked[2].type_ == TokenType::Mul);
    
    // Current token should still be the same
    REQUIRE(lexer->current().type_ == tok.type_);
    REQUIRE(lexer->current().cat_  == tok.cat_);
    REQUIRE(lexer->current().line_ == tok.line_);
    REQUIRE(lexer->current().pos_  == tok.pos_);
  });
}

TEST_CASE(Lexer, ErrorHandling) {
  SECTION(IllegalTokens, {
    auto lexer = create_lexer("42 ? 10");
    
    REQUIRE(lexer->current().type_ == TokenType::IntLiteral);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::Illegal);
    lexer->consume(1);
  });

  SECTION(UnterminatedString, {
    auto lexer = create_lexer("\"hello");
    
    REQUIRE(lexer->current().type_ == TokenType::Illegal);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::EndOfFile);
  });

  SECTION(InvalidNumberFormat, {
    auto lexer1 = create_lexer("1.2.3");
    auto lexer2 = create_lexer("0xGG");
    
    REQUIRE(lexer1->current().type_ == TokenType::Illegal);
    REQUIRE(lexer2->current().type_ == TokenType::Illegal);
  });
}

TEST_CASE(Lexer, LineCounting) {
  SECTION(BasicLineCounting, {
    auto lexer = create_lexer("42\n+ 10\n* 5");
    
    REQUIRE(lexer->current().line_ == 1);
    REQUIRE(lexer->current().type_ == TokenType::IntLiteral);
    lexer->consume(1);
    
    REQUIRE(lexer->current().line_ == 2);
    REQUIRE(lexer->current().type_ == TokenType::Plus);
    lexer->consume(1);
    
    REQUIRE(lexer->current().line_ == 2);
    REQUIRE(lexer->current().type_ == TokenType::IntLiteral);
    lexer->consume(1);
    
    REQUIRE(lexer->current().line_ == 3);
    REQUIRE(lexer->current().type_ == TokenType::Mul);
    lexer->consume(1);
    
    REQUIRE(lexer->current().line_ == 3);
    REQUIRE(lexer->current().type_ == TokenType::IntLiteral);
    lexer->consume(1);
    
    REQUIRE(lexer->current().line_ == 3);
    REQUIRE(lexer->current().type_ == TokenType::EndOfFile);
  });

  SECTION(Comments, {
    auto lexer = create_lexer("42 # This is a comment\n+ 10");
    
    REQUIRE(lexer->current().line_ == 1);
    REQUIRE(lexer->current().type_ == TokenType::IntLiteral);
    lexer->consume(1);
    
    REQUIRE(lexer->current().line_ == 2);
    REQUIRE(lexer->current().type_ == TokenType::Plus);
    lexer->consume(1);
    
    REQUIRE(lexer->current().line_ == 2);
    REQUIRE(lexer->current().type_ == TokenType::IntLiteral);
    lexer->consume(1);
    
    REQUIRE(lexer->current().line_ == 2);
    REQUIRE(lexer->current().type_ == TokenType::EndOfFile);
  });
}

TEST_CASE(Lexer, Expect) {
  SECTION(ExpectTokenType, {
    auto lexer = create_lexer("42 + 10");
    
    REQUIRE(lexer->expect(TokenType(TokenType::IntLiteral)).has_value());
    REQUIRE(lexer->expect(TokenType(TokenType::Plus)).has_value());
    REQUIRE(lexer->expect(TokenType(TokenType::IntLiteral)).has_value());
    REQUIRE(lexer->expect(TokenType(TokenType::EndOfFile)).has_value());
  });

  SECTION(ExpectTokenCategory, {
    auto lexer = create_lexer("42 + 10");

    REQUIRE(lexer->expect(TokenCategory::Literal).has_value());
    REQUIRE(lexer->expect(TokenCategory::ArithmeticOp).has_value());
    REQUIRE(lexer->expect(TokenCategory::Literal).has_value());
    REQUIRE(lexer->current() == TokenType::EndOfFile);
  });

  SECTION(ExpectFailure, {
    auto lexer = create_lexer("42 + 10");
    REQUIRE(!lexer->expect(TokenType(TokenType::Plus)).has_value());
    /// TODO: add more here
  });
}

TEST_CASE(Lexer, Revert) {
  SECTION(RevertToPreviousToken, {
    auto lexer = create_lexer("42 + 10");
    
    REQUIRE(lexer->current().type_ == TokenType::IntLiteral);
    auto token = lexer->current();
    
    lexer->consume(1);
    REQUIRE(lexer->current().type_ == TokenType::Plus);
    
    lexer->revert(token);
    REQUIRE(lexer->current().type_ == TokenType::IntLiteral);
  });
} 