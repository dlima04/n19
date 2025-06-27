/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Frontend/Entities/EntityTable.hpp>
#include <Core/Fmt.hpp>
#include <Core/Console.hpp>
BEGIN_NAMESPACE(rl);

BuiltinType::BuiltinType(const Type type) {
  #define X(TYPE, STR, _1) if(type == TYPE){ lname_ = STR; name_ = "::" STR; }
  RL_ENTITY_BUILTIN_LIST
  #undef X
}

auto EntityQualifier::format() const -> std::string {
  std::string buff;

  #define X(EQ_FLAG, UNUSED)                      \
  if(flags_ & rl::EntityQualifierBase::EQ_FLAG){  \
    buff += #EQ_FLAG " | ";                       \
  }
    RL_EQ_FLAG_LIST
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

auto EntityQualifierThunk::format() const -> std::string {
  std::string buff;

  #define X(EQ_FLAG, UNUSED)                      \
  if(flags_ & rl::EntityQualifierBase::EQ_FLAG){  \
    buff += #EQ_FLAG " | ";                       \
  }
    RL_EQ_FLAG_LIST
  #undef X

  if(!buff.empty()) {
    buff.erase(buff.size() - 3);
    buff += ", ";
  }

  buff += fmt("ptr_depth = {}, ", ptr_depth_);
  buff += arr_lengths_.empty() ? "array_lengths = N/A " : "array_lengths = ";

  for(const auto& length : arr_lengths_) {
    buff.append(fmt("{} ", length));
  }

  return fmt(
    "{}{} {}{}{}{}",             /// Fmt string
    manip_string(Con::BlueFG),   /// ID color = blue
    name_,                       /// The name
    manip_string(Con::Reset),    /// Reset console
    manip_string(Con::WhiteFG),  /// buff = white
    buff,                        /// The buff
    manip_string(Con::Reset));   /// Reset console
}

auto EntityQualifier::get_const_bool() -> EntityQualifier {
  EntityQualifier const_bool;
  const_bool.id_    = BuiltinType::Bool;
  const_bool.flags_ = Constant;
  return const_bool;
}

auto EntityQualifier::get_const_f64() -> EntityQualifier {
  EntityQualifier const_f64;
  const_f64.id_     = BuiltinType::F64;
  const_f64.flags_  = Constant;
  return const_f64;
}

auto EntityQualifier::get_const_ptr() -> EntityQualifier {
  EntityQualifier const_ptr;
  const_ptr.id_        = BuiltinType::Ptr;
  const_ptr.ptr_depth_ = 1;
  const_ptr.flags_     = Constant;
  return const_ptr;
}

auto EntityQualifier::to_string(
  const EntityTable &tbl,
  const bool include_qualifiers,
  const bool include_postfixes ) const -> std::string
{
  std::string buff;
  const auto ent = tbl.find(id_);

  if(include_qualifiers) {
    #define X(VAL, UNUSED) if(flags_ & VAL) buff.append(#VAL " ");
    RL_EQ_FLAG_LIST /* convert to string repr */
    #undef X
  }

  buff += ent->name_;
  if(include_postfixes) {
    for(const auto& len : arr_lengths_) buff.append(fmt("[{}]", len));
    for(int i = 0; i < ptr_depth_; i++) buff.append("*");
  }

  return buff;
}

auto EntityQualifierThunk::to_string(
  const bool include_qualifiers,
  const bool include_postfixes ) const -> std::string
{
  std::string buff;
  buff.reserve(name_.size() + 6);
  if(include_qualifiers) {
    #define X(VAL, UNUSED) if(flags_ & VAL) buff.append(#VAL " ");
    RL_EQ_FLAG_LIST /* convert to string repr */
    #undef X
  }

  buff += name_;
  if(include_postfixes) {
    for(int i = 0; i < ptr_depth_; i++) buff.append("*");
    for(const auto& len : arr_lengths_) buff.append(fmt("[{}]", len));
  }

  return buff;
}

auto Entity::print_children_(
  const uint32_t depth,
  OStream &stream, EntityTable &table ) const -> void
{
  stream << "\n";
  for(Entity::ID id : chldrn_) {
    auto ptr = table.find(id);
    ptr->print(depth + 1, stream, table);
  }
}

auto Entity::print_(
  const uint32_t depth,
  OStream &stream ) const -> void
{
  for(uint32_t i = 0; i < depth; i++)
    stream << "  |";
  if(depth)
    stream << "_ ";

  /// Preamble
  stream
    << Con::Bold
    << Con::MagentaFG
    << this->name_
    << Con::Reset;
  stream
    << " <"
    << Con::YellowFG
    << this->line_
    << Con::Reset
    << ','
    << Con::YellowFG
    << this->pos_
    << Con::Reset
    << "> -- ";

  /// Display entity type
#define X(TYPE)           \
  case EntityType::TYPE:  \
  stream                  \
    << Con::GreenFG       \
    << #TYPE              \
    << Con::Reset         \
    << " with ";          \
  break;

  switch(type_.value) {
    RL_ENTITY_TYPE_LIST
    default: UNREACHABLE_ASSERTION;
  }
#undef X

  /// Entity ID and other info
  stream
    << "EntityID="
    << Con::GreenFG
    << this->id_
    << Con::Reset
    << ", FileID="
    << Con::GreenFG
    << this->file_
    << Con::Reset
    << " ";
}

auto Type::print(
  const uint32_t depth,
  OStream &stream, EntityTable &table ) const -> void
{
  print_(depth, stream);
  print_children_(depth, stream, table);
}

auto Struct::print(
  const uint32_t depth,
  OStream &stream, EntityTable &table ) const -> void
{
  print_(depth, stream);
  print_children_(depth, stream, table);
}

auto BuiltinType::print(
  const uint32_t depth,
  OStream &stream, EntityTable &table) const -> void
{
  print_(depth, stream);
  print_children_(depth, stream, table);
}

auto AliasType::print(
  const uint32_t depth,
  OStream &stream, EntityTable &table ) const -> void
{
  print_(depth, stream);
  stream
    << ", Link="
    << Con::BlueFG
    << this->link_
    << Con::Reset
    << Con::Bold
    << ", Qualifiers: "
    << Con::Reset;

  std::string buff;

  #define X(EQ_FLAG, UNUSED)                             \
  if(quals_.flags_ & rl::EntityQualifierBase::EQ_FLAG){ \
    buff += #EQ_FLAG " | ";                              \
  }
    RL_EQ_FLAG_LIST
  #undef X

  if(!buff.empty()) {
    buff.erase(buff.size() - 3);
    buff += ", ";
  }

  buff += fmt("ptr_depth = {}, ", quals_.ptr_depth_);
  buff += quals_.arr_lengths_.empty() ? "array_lengths = N/A " : "array_lengths = ";

  for(const auto& length : quals_.arr_lengths_) {
    buff += fmt("{} ", length);
  }

  stream << "\n";
  print_children_(depth, stream, table);
}

auto Static::print(
  const uint32_t depth,
  OStream& stream, EntityTable &table ) const -> void
{
  print_(depth, stream);
  print_children_(depth, stream, table);
}

auto Proc::print(
  const uint32_t depth,
  OStream &stream, EntityTable &table) const -> void
{
  print_(depth, stream);
  stream << "Parameters: ( " << Con::BlueFG;
  for(Entity::ID id : parameters_) {
    stream << id << " ";
  }

  stream
    << Con::Reset
    << "), "
    << "ReturnType="
    << Con::BlueFG
    << return_type_
    << Con::Reset;
  print_children_(depth, stream, table);
}

auto PlaceHolder::print(
  const uint32_t depth,
  OStream &stream, EntityTable &table ) const -> void
{
  print_(depth, stream);
  stream
    << Con::RedFG
    << "(PLACEHOLDER)"
    << Con::Reset;
  print_children_(depth, stream, table);
}

auto SymLink::print(
  const uint32_t depth,
  OStream &stream, EntityTable &table ) const -> void
{
  print_(depth, stream);
  stream
    << ", Link="
    << Con::BlueFG
    << this->link_
    << Con::Reset;
  print_children_(depth, stream, table);
}

auto Variable::print(
  const uint32_t depth,
  OStream &stream, EntityTable &table ) const -> void
{
  print_(depth, stream);
  print_children_(depth, stream, table);
}

auto RootEntity::print(
  const uint32_t depth,
  OStream &stream, EntityTable &table ) const -> void
{
  print_(depth, stream);
  stream
    << Con::RedFG
    << "(ROOT)"
    << Con::Reset;
  print_children_(depth, stream, table);
}

/*
 * Convert the entity to a string representation.
 */
auto EntityType::to_string() const -> std::string {
  #define X(NAME) case EntityType::NAME: return #NAME;
  switch(this->value) {
    RL_ENTITY_TYPE_LIST
    case EntityType::None: return "None";
    default: break;
  }
  #undef X
  UNREACHABLE_ASSERTION;
}

/*
 * Is this entity a user-defined type?
 */
auto EntityType::is_udt() const -> bool {
  switch(this->value) {
    case Struct:      FALLTHROUGH_;
    case Type:        FALLTHROUGH_;
    case BuiltinType: FALLTHROUGH_;
    case AliasType:   return true;
    default:          return false;
  }
}

END_NAMESPACE(rl);