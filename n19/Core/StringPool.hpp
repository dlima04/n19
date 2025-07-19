/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <n19/Core/Common.hpp>
#include <n19/Core/ClassTraits.hpp>
#include <n19/Core/Murmur3.hpp>
#include <n19/Core/Fmt.hpp>
#include <n19/Core/Maybe.hpp>

#include <string_view>
#include <cstdint>
#include <unordered_map>
#include <vector>
#include <type_traits>
BEGIN_NAMESPACE(n19);

class StringPool {
  N19_MAKE_NONCOPYABLE(StringPool);
public:
  struct FixedBlock {
    char *beg;        /// Beginning of the bucket.
    char *cur;        /// current position.
    char *end;        /// 1 element past the end of the buffer.
  };

  struct Index {
    N19_MAKE_SPACESHIP(Index);
    uint32_t offset;  /// offset into the bucket's buffer
    uint32_t bucket;  /// which bucket is this?
  };

  using ViewType_ = std::string_view;
  using HashType_ = Murmur3_32;

  NODISCARD_ Maybe<Index> try_get_index(ViewType_ vt);
  NODISCARD_ Maybe<ViewType_> try_get_string(Index index);

  NODISCARD_ ViewType_ get_string(Index index);
  NODISCARD_ Index get_index(ViewType_ vt);
  NODISCARD_ Index insert_new_string_impl_(ViewType_ vt);

  StringPool(size_t block_size, uint32_t seed);
  ~StringPool();

  StringPool(StringPool&& other) noexcept;
  StringPool& operator=(StringPool&& other) noexcept;

  std::vector<FixedBlock> buffs_;
  std::unordered_multimap<HashType_, Index> indices_;
  uint32_t hashseed_ = 0;
  size_t block_size_ = 0;
};

static_assert(std::is_aggregate_v<StringPool::FixedBlock>);
static_assert(std::is_aggregate_v<StringPool::Index>);

END_NAMESPACE(n19);
