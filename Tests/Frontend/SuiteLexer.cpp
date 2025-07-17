/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <catch2/catch_test_macros.hpp>
#include <n19/Frontend/Lexer/Lexer.hpp>
#include <n19/Frontend/Lexer/Token.hpp>
#include <vector>
#include <string>
using namespace rl;

static auto create_lexer(const std::string& source) -> std::shared_ptr<Lexer> {
  std::vector<char8_t> buffer;
  buffer.reserve(source.size());
  for(auto c : source) {
    buffer.push_back(static_cast<char8_t>(c));
  }

  return Lexer::create_shared(std::move(buffer)).value();
}

TEST_CASE("BasicTokenRecognition", "[Frontend.Lexer]") {
  SECTION("SimpleTokens") {
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
  }

  SECTION("Operators") {
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
  }

  SECTION("CompoundOperators") {
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
  }
}

TEST_CASE("Literals", "[Frontend.Lexer]") {
  SECTION("IntegerLiterals") {
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
  }

  SECTION("FloatLiterals") {
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
  }

  SECTION("HexLiterals") {
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
  }

  SECTION("OctalLiterals") {
    auto lexer = create_lexer("042 0777");
    
    REQUIRE(lexer->current().type_ == TokenType::OctalLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "042");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::OctalLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "0777");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::EndOfFile);
  }

  SECTION("StringLiterals") {
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
  }

  SECTION("BooleanLiterals") {
    auto lexer = create_lexer("true false");
    
    REQUIRE(lexer->current().type_ == TokenType::BooleanLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "true");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::BooleanLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "false");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::EndOfFile);
  }

  SECTION("NullLiteral") {
    auto lexer = create_lexer("null");
    
    REQUIRE(lexer->current().type_ == TokenType::NullLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "null");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::EndOfFile);
  }
}

TEST_CASE("IdentifiersAndKeywords", "[Frontend.Lexer]") {
  SECTION("Identifiers") {
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
  }

  SECTION("Keywords") {
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
  }
}

TEST_CASE("Peeking", "[Frontend.Lexer]") {
  SECTION("PeekSingleToken") {
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
  }

  SECTION("PeekMultipleTokens") {
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
  }

  SECTION("BatchedPeek") {
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
  }
}

TEST_CASE("ErrorHandling", "[Frontend.Lexer]") {
  SECTION("IllegalTokens") {
    auto lexer = create_lexer("42 ? 10");
    
    REQUIRE(lexer->current().type_ == TokenType::IntLiteral);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::Illegal);
    lexer->consume(1);
  }

  SECTION("UnterminatedString") {
    auto lexer = create_lexer("\"hello");
    
    REQUIRE(lexer->current().type_ == TokenType::Illegal);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::EndOfFile);
  }

  SECTION("InvalidNumberFormat") {
    auto lexer1 = create_lexer("1.2.3");
    auto lexer2 = create_lexer("0xGG");
    
    REQUIRE(lexer1->current().type_ == TokenType::Illegal);
    REQUIRE(lexer2->current().type_ == TokenType::Illegal);
  }
}

TEST_CASE("LineCounting", "[Frontend.Lexer]") {
  SECTION("BasicLineCounting") {
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
  }

  SECTION("Comments") {
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
  }
}

TEST_CASE("Expect", "[Frontend.Lexer]") {
  SECTION("ExpectTokenType") {
    auto lexer = create_lexer("42 + 10");
    
    REQUIRE(lexer->expect_type(rl::TokenType::IntLiteral).has_value());
    REQUIRE(lexer->expect_type(rl::TokenType::Plus).has_value());
    REQUIRE(lexer->expect_type(rl::TokenType::IntLiteral).has_value());
    REQUIRE(lexer->expect_type(rl::TokenType::EndOfFile).has_value());
  }

  SECTION("ExpectTokenCategory") {
    auto lexer = create_lexer("42 + 10");

    REQUIRE(lexer->expect(rl::TokenCategory::Literal).has_value());
    REQUIRE(lexer->expect(rl::TokenCategory::ArithmeticOp).has_value());
    REQUIRE(lexer->expect(rl::TokenCategory::Literal).has_value());
    REQUIRE(lexer->current() == TokenType::EndOfFile);
  }

  SECTION("ExpectFailure") {
    auto lexer = create_lexer("42 + 10");
    REQUIRE(!lexer->expect_type(TokenType::Plus).has_value());
    /// TODO: add more here
  }
}

TEST_CASE("Revert", "[Frontend.Lexer]") {
  auto lexer = create_lexer("42 + 10");    
  REQUIRE(lexer->current().type_ == TokenType::IntLiteral);
  auto token = lexer->current();

  lexer->consume(1);
  REQUIRE(lexer->current().type_ == TokenType::Plus);

  lexer->revert_before(token);
  REQUIRE(lexer->current().type_ == TokenType::IntLiteral);
}

TEST_CASE("UTF8Parsing", "[Frontend.Lexer]") {
  SECTION("UTF8Identifiers") {
    // Create a lexer with UTF-8 encoded identifiers
    std::u8string chinese = u8"世界";  // "World" in Chinese
    std::u8string japanese = u8"こんにちは";  // "Hello" in Japanese
    
    std::vector<char8_t> buffer;
    buffer.reserve(chinese.size() + japanese.size() + 1);  // +1 for space
    
    // Copy Chinese identifier
    buffer.insert(buffer.end(), chinese.begin(), chinese.end());
    buffer.push_back(u8' ');  // Add space between identifiers
    // Copy Japanese identifier
    buffer.insert(buffer.end(), japanese.begin(), japanese.end());
    
    auto lexer = Lexer::create_shared(std::move(buffer)).value();
    
    // First identifier should be recognized
    REQUIRE(lexer->current().type_ == TokenType::Identifier);
    REQUIRE(lexer->current().value(*lexer).value().length() > 0);
    lexer->consume(1);
    
    // Second identifier should be recognized
    REQUIRE(lexer->current().type_ == TokenType::Identifier);
    REQUIRE(lexer->current().value(*lexer).value().length() > 0);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::EndOfFile);
  }

  SECTION("UTF8InStringLiterals") {
    // Create a lexer with UTF-8 encoded string literals
    std::u8string chinese = u8"世界";  // "World" in Chinese
    std::u8string japanese = u8"こんにちは";  // "Hello" in Japanese
    
    std::vector<char8_t> buffer;
    buffer.reserve(chinese.size() + japanese.size() + 6);  // +6 for quotes and space
    
    // First string literal
    buffer.push_back(u8'"');
    buffer.insert(buffer.end(), chinese.begin(), chinese.end());
    buffer.push_back(u8'"');
    buffer.push_back(u8' ');
    
    // Second string literal
    buffer.push_back(u8'"');
    buffer.insert(buffer.end(), japanese.begin(), japanese.end());
    buffer.push_back(u8'"');
    
    auto lexer = Lexer::create_shared(std::move(buffer)).value();
    
    // First string literal should be recognized
    REQUIRE(lexer->current().type_ == TokenType::StringLiteral);
    REQUIRE(lexer->current().value(*lexer).value().length() > 0);
    lexer->consume(1);
    
    // Second string literal should be recognized
    REQUIRE(lexer->current().type_ == TokenType::StringLiteral);
    REQUIRE(lexer->current().value(*lexer).value().length() > 0);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::EndOfFile);
  }

  SECTION("UTF8EscapedInStringLiterals") {
    // Create a lexer with UTF-8 encoded string literals containing escaped characters
    std::u8string chinese = u8"世界";  // "World" in Chinese
    
    std::vector<char8_t> buffer;
    buffer.reserve(chinese.size() + 5);  // +5 for quotes, escaped newline, and space
    
    buffer.push_back(u8'"');
    buffer.push_back(u8'\\');
    buffer.push_back(u8'n');
    buffer.push_back(u8' ');
    buffer.insert(buffer.end(), chinese.begin(), chinese.end());
    buffer.push_back(u8'"');
    
    auto lexer = Lexer::create_shared(std::move(buffer)).value();
    
    // String literal with escaped newline and UTF-8 should be recognized
    REQUIRE(lexer->current().type_ == TokenType::StringLiteral);
    REQUIRE(lexer->current().value(*lexer).value().length() > 0);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::EndOfFile);
  }
}

TEST_CASE("CharacterLiterals", "[Frontend.Lexer]") {
  SECTION("ValidCharacterLiterals") {
    auto lexer = create_lexer("'a' 'b' 'c' '\\n' '\\t' '\\r' '\\0'");
    
    REQUIRE(lexer->current().type_ == TokenType::ByteLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "'a'");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::ByteLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "'b'");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::ByteLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "'c'");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::ByteLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "'\\n'");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::ByteLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "'\\t'");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::ByteLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "'\\r'");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::ByteLiteral);
    REQUIRE(lexer->current().value(*lexer).value() == "'\\0'");
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::EndOfFile);
  }

  SECTION("InvalidCharacterLiterals") {
    // Multiple characters in single quotes should be illegal
    auto lexer1 = create_lexer("'aa'");
    REQUIRE(lexer1->current().type_ == TokenType::Illegal);
    
    // UTF-8 characters in single quotes should be illegal
    std::u8string chinese = u8"世";  // Single Chinese character
    
    std::vector<char8_t> buffer;
    buffer.reserve(chinese.size() + 2);  // +2 for quotes
    
    buffer.push_back(u8'\'');
    buffer.insert(buffer.end(), chinese.begin(), chinese.end());
    buffer.push_back(u8'\'');
    
    auto lexer2 = Lexer::create_shared(std::move(buffer)).value();
    REQUIRE(lexer2->current().type_ == TokenType::Illegal);
    
    // Unterminated character literal
    auto lexer3 = create_lexer("'a");

    REQUIRE(lexer3->current().type_ == TokenType::Illegal);
  }
}

TEST_CASE("InvalidTokens", "[Frontend.Lexer]") {
  SECTION("InvalidNumbers") {
    // Invalid number formats
    auto lexer1 = create_lexer("1.2.3");
    auto lexer3 = create_lexer("091");
    auto lexer2 = create_lexer("0xGG");

    REQUIRE(lexer1->current() == TokenType::Illegal);
    REQUIRE(lexer2->current() == TokenType::Illegal);
    REQUIRE(lexer3->current() == TokenType::Illegal);
  }

  SECTION("InvalidIdentifiers") {
    // Identifiers starting with numbers
    auto lexer = create_lexer("123abc");
    
    REQUIRE(lexer->current().type_ == TokenType::IntLiteral);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::Identifier);
    lexer->consume(1);
    
    REQUIRE(lexer->current().type_ == TokenType::EndOfFile);
  }
}
