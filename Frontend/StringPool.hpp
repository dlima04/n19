/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <Core/Maybe.hpp>
#include <Core/ClassTraits.hpp>
#include <string_view>
#include <vector>
BEGIN_NAMESPACE(n19);

/* n19::StringPool_:
 *
 * This class helps to prevent excessive string usage
 * for circumstances where it makes sense to do so.
 * The class stores a blob of strings in an internal flat
 * buffer of characters which can be accessed via index.
 * Nothing too fancy shmancy going on here.
 */
class StringPool {
N19_MAKE_NONCOPYABLE(StringPool);
public:
  using RawIndexType = size_t;
  enum class Index : RawIndexType {};

  auto insert(std::string_view) -> Index;

  // Attempts to locate a string
  auto try_find(RawIndexType)   -> Maybe<std::string_view>;
  auto try_find(Index index)    -> Maybe<std::string_view>;

  // Panics if the index is out of bounds
  auto find(RawIndexType index) -> std::string_view;
  auto find(Index index)        -> std::string_view;

  StringPool() = default;
 ~StringPool() = default;
private:
  std::vector<char> buff_;
};

END_NAMESPACE(n19);
