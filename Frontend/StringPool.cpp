/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Frontend/StringPool.hpp>
#include <IO/Fmt.hpp>
#include <Core/Panic.hpp>
BEGIN_NAMESPACE(n19);

auto StringPool::insert(std::string_view sv) -> Index {
  buff_.reserve(sv.size());
  size_t begin = buff_.size();

  for(const char ch : sv) {
    if(ch == '\0') break;
    buff_.emplace_back( ch );
  }

  buff_.emplace_back('\0');
  return static_cast<Index>(begin);
}

auto StringPool::find(RawIndexType index) -> std::string_view {
  bool is_valid = false;
  for(size_t i = index; i < buff_.size(); ++i) {
    if(buff_[i] == '\0') {
      is_valid = true;
      break;
    }
  }

  if(!is_valid) {
    PANIC(fmt("Out of bounds at index {}.", index));
  }

  return std::string_view(&buff_[index]);
}

auto StringPool::find(Index index) -> std::string_view {
  return find(static_cast<RawIndexType>(index));
}

auto StringPool::try_find(RawIndexType index) -> Maybe<std::string_view> {
  bool is_valid = false;
  for(size_t i = index; i < buff_.size(); ++i) {
    if(buff_[i] == '\0') {
      is_valid = true;
      break;
    }
  }

  if(!is_valid) {
    return Nothing;
  }

  return std::string_view(&buff_[index]);
}

auto StringPool::try_find(Index index) -> Maybe<std::string_view> {
  return try_find(static_cast<RawIndexType>(index));
}

END_NAMESPACE(n19);
