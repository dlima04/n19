/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <n19/Core/StringPool.hpp>
#include <n19/System/PageAllocator.hpp>
#include <n19/Core/Panic.hpp>
#include <n19/Core/Platform.hpp>
#include <cstring>
#include <utility>
#include <limits>
#include <stddef.h>
BEGIN_NAMESPACE(n19);

StringPool::StringPool(size_t block_size, uint32_t seed)
  : hashseed_(seed), block_size_(block_size)
{
  ASSERT(block_size > 0, "Invalid block size");
  this->buffs_.reserve(2);
  char* page = static_cast<char*>(sys::PageAllocator::alloc(block_size));

  this->buffs_.emplace_back(FixedBlock{
    .beg = page,
    .cur = page,
    .end = page + block_size});
}

StringPool& StringPool::operator=(StringPool &&other) noexcept
{
  if(this != &other) {
    this->hashseed_ = other.hashseed_;
    this->buffs_ = std::move(other.buffs_);
    this->block_size_ = other.block_size_;
    this->indices_ = std::move(other.indices_);
  }
  return *this;
}

StringPool::StringPool(StringPool &&other) noexcept
{
  this->block_size_ = other.block_size_;
  this->buffs_ = std::move(other.buffs_);
  this->hashseed_ = other.hashseed_;
  this->indices_ = std::move(other.indices_);
}

StringPool::~StringPool()
{
  for(auto& block : this->buffs_) {
    if(block.beg != nullptr)
      sys::PageAllocator::free(block.beg, block_size_);

    block.beg = nullptr;
    block.end = nullptr;
    block.cur = nullptr;
  }
}

auto StringPool::insert_new_string_impl_(const ViewType_ vt) -> Index
{
  ASSERT(!vt.empty(), "Empty string!");
  ASSERT(vt.size() + 1 <= this->block_size_, "String is too large.");
  ASSERT(!this->buffs_.empty(), "No blocks?");

  auto* curbuf = &this->buffs_.back();
  const auto remaining = static_cast<size_t>(curbuf->end - curbuf->cur);
  if(remaining < vt.size() + 1) {
    /// Allocate a new block.
    char* ptr = static_cast<char*>(sys::PageAllocator::alloc(block_size_));

    curbuf = &this->buffs_.emplace_back(FixedBlock{
      .beg = ptr,
      .cur = ptr,
      .end = ptr + block_size_});
  }

  /// Beginning index of where the string will be stored.
  std::ptrdiff_t start = curbuf->cur - curbuf->beg;

  /// bump the pointer.
  std::memcpy(curbuf->cur, vt.data(), vt.size());
  curbuf->cur[ vt.size() ] = '\0';
  curbuf->cur += vt.size() + 1;

  ASSERT(start >= 0);
  ASSERT(start <= std::numeric_limits<uint32_t>::max(), "Offset overflow");
  ASSERT((curbuf->end - curbuf->cur) >= 0, "wtf?");

  Index index;
  index.bucket = this->buffs_.size() - 1;
  index.offset = static_cast<uint32_t>(start);

  return index;
}

auto StringPool::try_get_index(const ViewType_ vt) -> Maybe<Index>
{
  if(vt.empty() || vt.size() + 1 > block_size_)
    return Nothing;

  const HashType_ hash = murmur3_x86_32(vt, hashseed_);
  auto matches = indices_.equal_range(hash);

  /// The hash might already exist. Try and find the string.
  for(auto it = matches.first; it != matches.second; ++it) {
    auto str = get_string(it->second);
    if(str == vt) return it->second;
  }

  /// A hash collision has occurred, or no matches were found.
  auto new_index = insert_new_string_impl_(vt);
  indices_.emplace(hash, new_index);
  return new_index;
}

auto StringPool::get_index(ViewType_ vt) -> Index
{
  ASSERT(!vt.empty(), "Empty strings are disallowed.");
  ASSERT(vt.size() + 1 <= this->block_size_, "String is too large.");

  const HashType_ hash = murmur3_x86_32(vt, hashseed_);
  auto matches = indices_.equal_range(hash);

  /// The hash might already exist. Try and find the string.
  for(auto it = matches.first; it != matches.second; ++it) {
    auto str = get_string(it->second);
    if(str == vt) return it->second;
  }

  /// A hash collision has occurred, or no matches were found.
  auto new_index = insert_new_string_impl_(vt);
  indices_.emplace(hash, new_index);
  return new_index;
}

auto StringPool::try_get_string(const Index index) -> Maybe<ViewType_>
{
  if(index.bucket >= buffs_.size())
    return Nothing;

  FixedBlock& bucket = buffs_[index.bucket];
  const auto bucket_size = static_cast<size_t>(bucket.end - bucket.beg);
  for(size_t i = index.offset; i < bucket_size; i++) {
    if(bucket.beg[i] == '\0')
      return ViewType_(&bucket.beg[index.offset]);
  }

  return Nothing;
}

auto StringPool::get_string(const Index index) -> ViewType_
{
  ASSERT(index.bucket < buffs_.size(), "bucket out of bounds");
  FixedBlock& bucket = buffs_[index.bucket];

  const auto bucket_size = static_cast<size_t>(bucket.end - bucket.beg);
  for(size_t i = index.offset; i < bucket_size; i++) {
    if(bucket.beg[i] == '\0')
      return ViewType_(&bucket.beg[index.offset]);
  }

  PANIC(fmt("Invalid string in bucket {} with index {}.",
    index.bucket, index.offset));
}

//https://youtu.be/J6f6y7P_AQ4
END_NAMESPACE(n19);
