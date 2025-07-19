/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <catch2/catch_test_macros.hpp>
#include <n19/Core/StringPool.hpp>
#include <n19/System/PageAllocator.hpp>
#include <n19/Core/Defer.hpp>
#include <cstdlib>
#include <cstddef>
using namespace n19;

TEST_CASE("StringPool basic interning", "[Core.StringPool]") {
  StringPool pool(1024, 42);

  auto idx1 = pool.get_index("hello");
  auto idx2 = pool.get_index("hello");

  REQUIRE(idx1.offset == idx2.offset);
  REQUIRE(idx1.bucket == idx2.bucket);

  auto str1 = pool.get_string(idx1);
  REQUIRE(str1 == "hello");

  auto maybe_idx = pool.try_get_index("hello");
  REQUIRE(maybe_idx.has_value());
  REQUIRE(maybe_idx->offset == idx1.offset);
}

TEST_CASE("StringPool distinct strings", "[Core.StringPool]") {
  StringPool pool(1024, 1);

  auto idx1 = pool.get_index("one");
  auto idx2 = pool.get_index("two");

  const bool req = idx1.offset != idx2.offset || idx1.bucket != idx2.bucket;
  REQUIRE(req);

  REQUIRE(pool.get_string(idx1) == "one");
  REQUIRE(pool.get_string(idx2) == "two");
}

TEST_CASE("StringPool rejects too-large strings", "[Core.StringPool]") {
  StringPool pool(16, 123);
  std::string bigstr(100, 'X');

  auto maybe = pool.try_get_index(bigstr);
  REQUIRE_FALSE(maybe.has_value());
}

TEST_CASE("StringPool handles collisions correctly", "[Core.StringPool]") {
  /// We want to "fake" a collision here.
  constexpr uint32_t SEED = 2;
  StringPool pool(1024, SEED);

  std::string_view the_string = "foobarbaz";
  const Murmur3_32 the_hash = murmur3_x86_32(the_string, SEED);

  /// Allow a collision to occur. Now when "foobarbaz" is inserted, the pool
  /// will think that a hash collision has happened and will need to act accordingly.
  const StringPool::Index first_index = pool.insert_new_string_impl_("different");
  pool.indices_.emplace(the_hash, first_index);

  REQUIRE(pool.buffs_.size() == 1);
  const auto second_index = pool.try_get_index(the_string);

  REQUIRE(second_index.has_value());
  REQUIRE(second_index.value() != first_index); /// The indexes are unique.

  const auto retrieved_str = pool.try_get_string(second_index.value());
  REQUIRE(retrieved_str.has_value());
  REQUIRE(retrieved_str.value() == "foobarbaz"); /// Same as the string we inserted.

  size_t counter = 0;
  auto matches = pool.indices_.equal_range(the_hash);
  REQUIRE(matches.first != matches.second);

  for(auto it = matches.first; it != matches.second; ++it) {
    ++counter;
  }

  REQUIRE(counter == 2);
}

TEST_CASE("StringPool lots of collisions", "[Core.StringPool]") {
  constexpr uint32_t SEED = 2;
  StringPool pool(1024, SEED);

  std::string_view the_string = "foobarbaz";
  const Murmur3_32 the_hash = murmur3_x86_32(the_string, SEED);

  const StringPool::Index index1 = pool.insert_new_string_impl_("different1");
  const StringPool::Index index2 = pool.insert_new_string_impl_("different2");
  const StringPool::Index index3 = pool.insert_new_string_impl_("different3");
  const StringPool::Index index4 = pool.insert_new_string_impl_("different4");

  pool.indices_.emplace(the_hash, index1);
  pool.indices_.emplace(the_hash, index2);
  pool.indices_.emplace(the_hash, index3);
  pool.indices_.emplace(the_hash, index4);

  REQUIRE(pool.buffs_.size() == 1);
  const auto real_index = pool.try_get_index(the_string);

  REQUIRE(real_index.has_value());
  REQUIRE(real_index.value() != index1);
  REQUIRE(real_index.value() != index2);
  REQUIRE(real_index.value() != index3);
  REQUIRE(real_index.value() != index4);

  const auto real_retrieved = pool.try_get_string(*real_index);
  REQUIRE(real_retrieved.has_value());
  REQUIRE(real_retrieved.value() == "foobarbaz");

  const auto retrieved1 = pool.try_get_string(index1);
  const auto retrieved2 = pool.try_get_string(index2);
  const auto retrieved3 = pool.try_get_string(index3);
  const auto retrieved4 = pool.try_get_string(index4);

  REQUIRE(retrieved1.value() == "different1");
  REQUIRE(retrieved2.value() == "different2");
  REQUIRE(retrieved3.value() == "different3");
  REQUIRE(retrieved4.value() == "different4");

  size_t counter = 0;
  auto matches = pool.indices_.equal_range(the_hash);
  REQUIRE(matches.first != matches.second);

  for(auto it = matches.first; it != matches.second; ++it) {
    ++counter;
  }

  REQUIRE(counter == 5);
}

TEST_CASE("StringPool weird buffer stuff 1", "[Core.StringPool]") {
  const size_t page_size = sys::PageAllocator::page_size();
  StringPool pool(page_size, 5);

  REQUIRE(!pool.buffs_.empty());
  REQUIRE(page_size > 40);

  pool.buffs_.back().cur += (page_size - 11);

  std::string_view str1 = "AAAAAAA"; /// 7 characters
  REQUIRE(str1.size() == 7);

  const auto index1 = pool.try_get_index(str1);
  REQUIRE(index1.has_value());
  REQUIRE(pool.buffs_.size() == 1); /// still 1 at this point

  const std::ptrdiff_t diff = pool.buffs_.back().end - pool.buffs_.back().cur;
  REQUIRE(diff == 3); /// 3 character slots left.

  const auto index2 = pool.try_get_index("BBBBBBBBBB");
  REQUIRE(index2.has_value());

  /// number of buckets is now 2, because the insertion above
  /// could not fit inside of the first bucket.
  REQUIRE(pool.buffs_.size() == 2);

  /// retrieve the strings, verify them and shit.
  const auto retr1 = pool.try_get_string(index1.value());
  const auto retr2 = pool.try_get_string(index2.value());

  REQUIRE(retr1.has_value());
  REQUIRE(retr2.has_value());

  REQUIRE(retr1.value() == "AAAAAAA");
  REQUIRE(retr2.value() == "BBBBBBBBBB");
}

TEST_CASE("StringPool string is equal to the block size", "[Core.StringPool]") {
  StringPool pool(11, 0xbeef);
  std::string_view the_string = "AAAAAAAAAA";
  REQUIRE(the_string.size() == 10);

  /// the_string.size() + 1 == block_size (11 in this case)
  const auto index1 = pool.try_get_index(the_string);
  REQUIRE(index1.has_value());
  REQUIRE(pool.buffs_.size() == 1);

  /// Retrieve cause why not ig
  const auto retr = pool.try_get_string(index1.value());
  REQUIRE(retr.has_value());
  REQUIRE(retr.value() == "AAAAAAAAAA");
}

TEST_CASE("StringPool rejects empty string", "[Core.StringPool]") {
  StringPool pool(128, 11);

  auto maybe = pool.try_get_index("");
  REQUIRE_FALSE(maybe.has_value());
}

TEST_CASE("StringPool try_get_string with invalid index", "[Core.StringPool]") {
  StringPool pool(64, 555);

  auto result = pool.try_get_string(StringPool::Index{.offset = 999, .bucket = 999});
  REQUIRE_FALSE(result.has_value());
}

