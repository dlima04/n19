/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <n19/Core/Common.hpp>
#include <n19/Core/Platform.hpp>
#include <n19/Core/Bytes.hpp>
#include <n19/Core/Concepts.hpp>
#include <n19/Core/Panic.hpp>
#include <array>
#include <type_traits>
BEGIN_NAMESPACE(n19);

// =========================================
// Flat profile bases
// =========================================

template<typename Ty_>
struct FlatProfile { /// Must be specialized
  static_assert(FalseType<Ty_>::value, "No flat profile.");
  FlatProfile() = delete;
};

// =========================================
// Trivial specializations for FlatProfile
// =========================================

template<typename Ty_> requires(std::is_fundamental_v<Ty_>)
struct FlatProfile<Ty_>
{
  using BufferType = ByteCopy<FlatProfile>;
  using ValueType  = Ty_;

  NODISCARD_ static FlatProfile create(Ty_ t) { return FlatProfile{.value_ = t}; }

  NODISCARD_ FORCEINLINE_
  Ty_ deserialize() const { return value_; }

  NODISCARD_ FORCEINLINE_
  BufferType serialize() const { return BufferType(value_) }

  ValueType value_;
};

template<typename Ty_> requires (
  AnyOf<Ty_, std::string_view, std::u8string_view, std::wstring_view> )
struct FlatProfile<Ty_>
{
  using BufferType = ByteCopy<FlatProfile>;
  using ValueType  = Ty_;

  NODISCARD_ static FlatProfile create(Ty_ t) {
    return FlatProfile { .beg_ = t.data(), .len_ = t.size() };
  }

  NODISCARD_ FORCEINLINE_
  Ty_ deserialize() const { return ValueType(beg_, len_) }

  NODISCARD_ FORCEINLINE_
  BufferType serialize() const { return BufferType(beg_, len_); }

  typename ValueType::const_pointer beg_;
  size_t len_;
};

template<typename Ty_, size_t sz_>
struct FlatProfile<std::array<Ty_, sz_>> {
  using BufferType = ByteCopy<FlatProfile>;
  using ValueType  = std::array<Ty_, sz_>;

  NODISCARD_ static FlatProfile create(ValueType arr) {
    return FlatProfile{ .arr_ = arr };
  }

  NODISCARD_ FORCEINLINE_
  ValueType deserialize() const { return ValueType(arr_) }

  NODISCARD_ FORCEINLINE_
  BufferType serialize() const { return BufferType(arr_); }

  ValueType arr_;
};

// =========================================
// Concepts
// =========================================

template<typename Ty_>
concept FlatProfileTrivial = std::is_aggregate_v<FlatProfile<Ty_>>;

template<typename Ty_>
concept HasFlatProfileSerialize = requires(Ty_ a){
  { a.flat_profile() } -> DecayIs<FlatProfile>;
  { declval<FlatProfile<Ty_>>().serialize() } -> ByteCopy<FlatProfile<Ty_>>;
};

template<typename Ty_>
concept HasFlatProfileDeserialize = requires(Ty_ a){
  { declval<FlatProfile<Ty_>>().deserialize() } -> DecayIs<Ty_>;
};

template<typename Ty_>
concept HasFlatProfile =
  FlatProfileTrivial<Ty_> && HasFlatProfileSerialize<Ty_> && HasFlatProfileDeserialize<Ty_>;

END_NAMESPACE(n19);

