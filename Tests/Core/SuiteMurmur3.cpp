/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <catch2/catch_test_macros.hpp>
#include <Core/Murmur3.hpp>
#include <string>
using namespace n19;

constexpr uint32_t KNOWN_32_HASH   = 0x2352d5c7u;  /// "Hello, World!" with seed 0
constexpr uint64_t KNOWN_128_HASH1 = 0x93bc65736fb8a1c0ull;  /// "Hello, World!" with seed 0
constexpr uint64_t KNOWN_128_HASH2 = 0xaa43288c6771aebfull;

TEST_CASE("BasicFunctionality", "[Core.Murmur3]") {
  SECTION("EmptyInput") {
    REQUIRE(murmur3_x86_32(u8"", 0) == 0);
    auto empty_128 = murmur3_x64_128(u8"", 0);
    REQUIRE(empty_128.first_ == 0);
    REQUIRE(empty_128.second_ == 0);
  }

  SECTION("KnownTestVector") {
    REQUIRE(murmur3_x86_32(u8"Hello, World!", 0) == KNOWN_32_HASH);
    auto hash_128 = murmur3_x64_128(u8"Hello, World!", 0);
    REQUIRE(hash_128.first_ == KNOWN_128_HASH1);
    REQUIRE(hash_128.second_ == KNOWN_128_HASH2);
  }

  SECTION("DifferentSeeds") {
    REQUIRE(murmur3_x86_32(u8"Hello, World!", 0) != murmur3_x86_32(u8"Hello, World!", 1));
    auto hash_128_1 = murmur3_x64_128(u8"Hello, World!", 0);
    auto hash_128_2 = murmur3_x64_128(u8"Hello, World!", 1);
    REQUIRE(hash_128_1.first_ != hash_128_2.first_);
    REQUIRE(hash_128_1.second_ != hash_128_2.second_);
  }
}

TEST_CASE("InputLengths", "[Core.Murmur3]") {
  SECTION("ShortInput") {
    REQUIRE(murmur3_x86_32(u8"A", 0) != 0);
    auto hash_128 = murmur3_x64_128(u8"A", 0);
    REQUIRE(hash_128.first_ != 0);
    REQUIRE(hash_128.second_ != 0);
  }

  SECTION("MediumInput") {
    std::u8string medium(100, u8'A');
    REQUIRE(murmur3_x86_32(medium, 0) != 0);
    auto hash_128 = murmur3_x64_128(medium, 0);
    REQUIRE(hash_128.first_ != 0);
    REQUIRE(hash_128.second_ != 0);
  }

  SECTION("LongInput") {
    std::u8string long_str(1000, u8'A');
    REQUIRE(murmur3_x86_32(long_str, 0) != 0);
    auto hash_128 = murmur3_x64_128(long_str, 0);
    REQUIRE(hash_128.first_ != 0);
    REQUIRE(hash_128.second_ != 0);
  }
}

TEST_CASE("UserDefinedLiterals", "[Core.Murmur3]") {
  SECTION("Literal32") {
    REQUIRE(u8"Hello, World!"_mm32 != 0);
    REQUIRE(u8"Hello, World!"_mm32 == murmur3_x86_32(u8"Hello, World!", 0xbeef));
  }

  SECTION("Literal128") {
    auto hash_literal = u8"Hello, World!"_mm128;
    auto hash_direct = murmur3_x64_128(u8"Hello, World!", 0xbeef);
    REQUIRE(hash_literal.first_ == hash_direct.first_);
    REQUIRE(hash_literal.second_ == hash_direct.second_);
  }
}

TEST_CASE("ConstexprBehavior", "[Core.Murmur3]") {
  SECTION("Constexpr32") {
    constexpr auto hash = murmur3_x86_32(u8"Hello, World!", 0);
    REQUIRE(hash == KNOWN_32_HASH);
  }

  SECTION("Constexpr128") {
    constexpr auto hash = murmur3_x64_128(u8"Hello, World!", 0);
    REQUIRE(hash.first_ == KNOWN_128_HASH1);
    REQUIRE(hash.second_ == KNOWN_128_HASH2);
  }

  SECTION("ConstexprLiterals") {
    constexpr auto hash32 = u8"Hello, World!"_mm32;
    constexpr auto hash128 = u8"Hello, World!"_mm128;
    REQUIRE(hash32 != 0);
    REQUIRE(hash128.first_ != 0);
    REQUIRE(hash128.second_ != 0);
  }
}
