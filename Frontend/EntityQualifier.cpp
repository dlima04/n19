/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Frontend/EntityQualifier.hpp>
#include <IO/Console.hpp>

auto n19::EntityQualifier::format() const -> std::string {
  std::string buff;

  #define X(EQ_FLAG, UNUSED)                      \
  if(flags_ & n19::EntityQualifier::EQ_FLAG){     \
    buff += #EQ_FLAG " | ";                       \
  }
    N19_EQ_FLAG_LIST
  #undef X

  if(!buff.empty()) {
    buff.erase(buff.size() - 3);
    buff += ", ";
  }

  buff += fmt("ptr_depth = {}, ", ptr_depth_);
  buff += arr_lengths_.empty() ? "array_lengths = N/A " : "array_lengths = ";

  for(const auto& length : arr_lengths_) {
    buff += fmt("{} ", length);
  }

  return fmt(
    "{}ID: {} {}{}{}{}",         /// Fmt string
    manip_string(Con::BlueFG),   /// ID color = blue
    static_cast<uint32_t>(id_),  /// The ID
    manip_string(Con::Reset),    /// Reset console
    manip_string(Con::WhiteFG),  /// buff = white
    buff,                        /// The buff
    manip_string(Con::Reset));   /// Reset console
}

auto n19::EntityQualifierThunk::format() const -> std::string {
  std::string buff;      /// the flags and qualifiers
  std::string full_name; /// full entity name

  #define X(EQ_FLAG, UNUSED)                      \
  if(flags_ & n19::EntityQualifier::EQ_FLAG){     \
    buff += #EQ_FLAG " | ";                       \
  }
    N19_EQ_FLAG_LIST
  #undef X

  if(!buff.empty()) {
    buff.erase(buff.size() - 3);
    buff += ", ";
  }

  buff += fmt("ptr_depth = {}, ", ptr_depth_);
  buff += arr_lengths_.empty() ? "array_lengths = N/A " : "array_lengths = ";

  for(const auto& length : arr_lengths_) {
    buff.append(fmt("{} ", length));
  } for(const auto& chunk : name_) {
    full_name.append(chunk);
  }

  return fmt(
    "{}{} {}{}{}{}",             /// Fmt string
    manip_string(Con::BlueFG),   /// ID color = blue
    full_name,                   /// The ID
    manip_string(Con::Reset),    /// Reset console
    manip_string(Con::WhiteFG),  /// buff = white
    buff,                        /// The buff
    manip_string(Con::Reset));   /// Reset console
}

auto n19::EntityQualifier::get_const_bool() -> EntityQualifier {
  EntityQualifier const_bool;
  const_bool.id_    = BuiltinType::Bool;
  const_bool.flags_ = Constant;
  return const_bool;
}

auto n19::EntityQualifier::get_const_f64() -> EntityQualifier {
  EntityQualifier const_f64;
  const_f64.id_     = BuiltinType::F64;
  const_f64.flags_  = Constant;
  return const_f64;
}

auto n19::EntityQualifier::get_const_ptr() -> EntityQualifier {
  EntityQualifier const_ptr;
  const_ptr.id_        = BuiltinType::Ptr;
  const_ptr.ptr_depth_ = 1;
  const_ptr.flags_     = Constant;
  return const_ptr;
}

auto n19::EntityQualifier::to_string(
  const EntityTable &tbl,
  const bool include_qualifiers,
  const bool include_postfixes ) const -> std::string
{
  std::string buff;
  const auto ent = tbl.find(id_);

  if(include_qualifiers) {
    #define X(VAL, UNUSED) if(flags_ & VAL) buff.append(#VAL " ");
    N19_EQ_FLAG_LIST /* convert to string repr */
    #undef X
  }

  buff += ent->name_;
  if(include_postfixes) {
    for(const auto& len : arr_lengths_) buff.append(fmt("[{}]", len));
    for(int i = 0; i < ptr_depth_; i++) buff.append("*");
  }

  return buff;
}

auto n19::EntityQualifierThunk::to_string(
  const bool include_qualifiers,
  const bool include_postfixes ) const -> std::string
{
  std::string buff;
  if(include_qualifiers) {
    #define X(VAL, UNUSED) if(flags_ & VAL) buff.append(#VAL " ");
    N19_EQ_FLAG_LIST /* convert to string repr */
    #undef X
  }

  for(const auto& chunk : name_) {
    buff.append(chunk);
  }

  if(include_postfixes) {
    for(const auto& len : arr_lengths_) buff.append(fmt("[{}]", len));
    for(int i = 0; i < ptr_depth_; i++) buff.append("*");
  }

  return buff;
}

