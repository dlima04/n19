/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <catch2/catch_test_macros.hpp>
#include <Core/StringUtil.hpp>
using namespace n19;

TEST_CASE("RegularStringUnescape", "[Core.StringUtil]") {
  SECTION("BasicEscapes") {
    auto result = unescape_string("\\n\\t\\r\\b\\f\\a\\v");
    REQUIRE(result.has_value());
    REQUIRE(result->size() == 7);
    REQUIRE((*result)[0] == '\n');
    REQUIRE((*result)[1] == '\t');
    REQUIRE((*result)[2] == '\r');
    REQUIRE((*result)[3] == '\b');
    REQUIRE((*result)[4] == '\f');
    REQUIRE((*result)[5] == '\a');
    REQUIRE((*result)[6] == '\v');
  }

  SECTION("QuoteEscapes") {
    auto result = unescape_string("\\\"\\'\\\\");
    REQUIRE(result.has_value());
    REQUIRE(result->size() == 3);
    REQUIRE((*result)[0] == '"');
    REQUIRE((*result)[1] == '\'');
    REQUIRE((*result)[2] == '\\');
  }

  SECTION("HexEscapes") {
    auto result = unescape_string("\\x41\\x42\\x43");
    REQUIRE(result.has_value());
    REQUIRE(result->size() == 3);
    REQUIRE((*result)[0] == 'A');
    REQUIRE((*result)[1] == 'B');
    REQUIRE((*result)[2] == 'C');
  }

  SECTION("OctalEscapes") {
    auto result = unescape_string("\\101\\102\\103");
    REQUIRE(result.has_value());
    REQUIRE(result->size() == 3);
    REQUIRE((*result)[0] == 'A');
    REQUIRE((*result)[1] == 'B');
    REQUIRE((*result)[2] == 'C');
  }

  SECTION("InvalidEscapes") {
    auto result = unescape_string("\\z");
    REQUIRE(!result.has_value());
  }

  SECTION("InvalidHex") {
    auto result = unescape_string("\\xG");
    REQUIRE(!result.has_value());
  }
}

TEST_CASE("QuotedStringUnescape", "[Core.StringUtil]") {
  SECTION("BasicQuoted") {
    auto result = unescape_quoted_string("\"Hello\\nWorld\"");
    REQUIRE(result.has_value());
    REQUIRE(*result == "Hello\nWorld");
  }

  SECTION("EmptyQuoted") {
    auto result = unescape_quoted_string("\"\"");
    REQUIRE(result.has_value());
    REQUIRE(*result == "");
  }

  SECTION("MixedContent") {
    auto result = unescape_quoted_string("\"Hello\\n\\x41\\101\"");
    REQUIRE(result.has_value());
    REQUIRE(*result == "Hello\nAA");
  }
}

TEST_CASE("RawStringUnescape", "[Core.StringUtil]") {
  SECTION("BasicRaw") {
    auto result = unescape_raw_string("Hello\\`World");
    REQUIRE(result.has_value());
    REQUIRE(*result == "Hello`World");
  }

  SECTION("NoEscapes") {
    auto result = unescape_raw_string("Hello World");
    REQUIRE(result.has_value());
    REQUIRE(*result == "Hello World");
  }
}

TEST_CASE("RawQuotedStringUnescape", "[Core.StringUtil]") {
  SECTION("BasicRawQuoted") {
    auto result = unescape_raw_quoted_string("`Hello\\`World`");
    REQUIRE(result.has_value());
    REQUIRE(*result == "Hello`World");
  }

  SECTION("EmptyRawQuoted") {
    auto result = unescape_raw_quoted_string("``");
    REQUIRE(result.has_value());
    REQUIRE(*result == "");
  }

  SECTION("ComplexRawQuoted") {
    auto result = unescape_raw_quoted_string("`Hello\\`World\\`Test`");
    REQUIRE(result.has_value());
    REQUIRE(*result == "Hello`World`Test");
  }
}
