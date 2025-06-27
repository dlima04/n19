/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <Core/Common.hpp>
#include <Frontend/Entities/Entity.hpp>
#include <Core/ClassTraits.hpp>
#include <Core/Fmt.hpp>
#include <Core/Panic.hpp>
#include <Core/Maybe.hpp>
#include <Core/Result.hpp>
#include <unordered_map>
#include <print>
#include <utility>
BEGIN_NAMESPACE(rl);

using namespace n19;

class EntityTable {
  N19_MAKE_NONCOPYABLE(EntityTable);
  N19_MAKE_NONMOVABLE(EntityTable);
public:
  template<typename T, typename ...Args>
  auto insert(
    Entity::ID parent_id,
    size_t pos,
    uint32_t line,
    InputFile::ID file,
    const std::string& lname,
    Args&&... args
  ) -> Entity::Ptr<T>;

  template<typename T, typename ...Args>
  auto insert(
    const Entity::Ptr<>& parent,
    size_t pos,
    uint32_t line,
    InputFile::ID file,
    const std::string& lname,
    Args&&... args
  ) -> Entity::Ptr<T>;

  template<typename T, typename ...Args>
  auto swap_entity(
    Entity::ID id_of,
    Entity::ID parent_id,
    size_t new_pos,
    uint32_t new_line,
    InputFile::ID new_file,
    Args&&... args
  ) -> Entity::Ptr<T>;

  template<typename T, typename ...Args>
  auto swap_entity(
    Entity::ID id_of,
    const Entity::Ptr<>& parent_ptr,
    size_t new_pos,
    uint32_t new_line,
    InputFile::ID new_file,
    Args&&... args
  ) -> Entity::Ptr<T>;

  template<typename T, typename ...Args>
  auto swap_placeholder(
    Entity::ID id_of,
    Entity::ID parent_id,
    size_t new_pos,
    uint32_t new_line,
    InputFile::ID new_file,
    Args&&... args
  ) -> Result<Entity::Ptr<T>>;

  template<typename T, typename ...Args>
  auto swap_placeholder(
    Entity::ID id_of,
    const Entity::Ptr<>& parent_ptr,
    size_t new_pos,
    uint32_t new_line,
    InputFile::ID new_file,
    Args&&... args
  ) -> Result<Entity::Ptr<T>>;

  template<typename T>
  auto find_if(T&& pred) const -> Maybe<Entity::Ptr<>>;

  auto exists(Entity::ID id) const -> bool;
  auto find(Entity::ID id) const -> Entity::Ptr<>;
  auto resolve_link(Entity::Ptr<SymLink> ptr) const -> Entity::Ptr<>;
  auto dump(OStream& stream = outs()) -> void;
  auto dump_structures(OStream& stream = outs()) -> void;

  std::unordered_map<Entity::ID, Entity::Ptr<>> map_;
  std::shared_ptr<RootEntity> root_ = nullptr;

  ~EntityTable() = default;
  explicit EntityTable(const sys::String& name);
private:
  Entity::ID curr_id_ = 1;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename ...Args>
auto EntityTable::insert(
  const Entity::Ptr<>& parent,
  const size_t pos,
  const uint32_t line,
  const InputFile::ID file,
  const std::string& lname,
  Args&&... args ) -> Entity::Ptr<T>
{
  ASSERT(parent != nullptr);
  ASSERT(exists(parent->id_));
  ASSERT(line != 0);

  const auto id     = curr_id_;
  map_[id]          = std::make_shared<T>(std::forward(args)...);
  map_[id]->file_   = file;
  map_[id]->id_     = id;
  map_[id]->parent_ = parent->id_;
  map_[id]->lname_  = lname;
  map_[id]->pos_    = pos;
  map_[id]->line_   = line;
  map_[id]->name_   = parent->id_ == RL_ROOT_ENTITY_ID
    ? fmt("::{}", lname) : parent->name_ + fmt("::{}", lname);

  #define X(NAME)                       \
  if constexpr(IsSame<T, NAME>) {       \
    map_[id]->type_ = EntityType::NAME; \
  }

  RL_ENTITY_TYPE_LIST
  #undef X

  ++curr_id_;
  return Entity::cast<T>(map_[id]);
}

template<typename T, typename ...Args>
auto EntityTable::insert(
  const Entity::ID parent_id,
  const size_t pos,
  const uint32_t line,
  const InputFile::ID file,
  const std::string& lname,
  Args&&... args ) -> Entity::Ptr<T>
{
  ASSERT(exists(parent_id));
  ASSERT(line != 0);

  const auto id = curr_id_;
  const auto parent = find(parent_id);

  map_[id]          = std::make_shared<T>(std::forward(args)...);
  map_[id]->file_   = file;
  map_[id]->id_     = id;
  map_[id]->parent_ = parent->id_;
  map_[id]->lname_  = lname;
  map_[id]->pos_    = pos;
  map_[id]->line_   = line;
  map_[id]->name_   = parent->id_ == RL_ROOT_ENTITY_ID
    ? fmt("::{}", lname) : parent->name_ + fmt("::{}", lname);

  #define X(NAME)                       \
  if constexpr(IsSame<T, NAME>) {       \
    map_[id]->type_ = EntityType::NAME; \
  }

  RL_ENTITY_TYPE_LIST
  #undef X

  parent->chldrn_.emplace_back(id);
  ++curr_id_;
  return Entity::cast<T>(map_[id]);
}

template<typename T, typename... Args>
auto EntityTable::swap_entity(
  const Entity::ID id_of,
  const Entity::Ptr<>& parent_ptr,
  const size_t new_pos,
  const uint32_t new_line,
  const InputFile::ID new_file,
  Args&&... args ) -> Entity::Ptr<T>
{
  ASSERT(exists(parent_ptr->id_));
  ASSERT(new_line != 0);

  auto old = find(id_of);
  ASSERT(old->parent_ == parent_ptr->id_);

  map_[id_of] = std::make_shared<T>(std::forward(args)...);

  #define X(NAME)                          \
  if constexpr(IsSame<T, NAME>) {          \
    map_[id_of]->type_ = EntityType::NAME; \
  }

  RL_ENTITY_TYPE_LIST
  #undef X

  map_[id_of]->file_   = new_file;
  map_[id_of]->id_     = id_of;
  map_[id_of]->parent_ = old->parent_;
  map_[id_of]->pos_    = new_pos;
  map_[id_of]->line_   = new_line;
  map_[id_of]->chldrn_ = std::move(old->chldrn_);
  map_[id_of]->name_   = std::move(old->name_);
  map_[id_of]->lname_  = std::move(old->lname_);

  return Entity::cast<T>(map_[id_of]);
}

template<typename T, typename... Args>
auto EntityTable::swap_entity(
  const Entity::ID id_of,
  const Entity::ID parent_id,
  const size_t new_pos,
  const uint32_t new_line,
  const InputFile::ID new_file,
  Args &&... args ) -> Entity::Ptr<T>
{
  ASSERT(new_line != 0);
  auto old = find(id_of);
  const auto parent_ptr = find(parent_id);

  ASSERT(old->parent_ == parent_ptr->id_);

  map_[id_of] = std::make_shared<T>(std::forward(args)...);

  #define X(NAME)                          \
  if constexpr(IsSame<T, NAME>) {          \
    map_[id_of]->type_ = EntityType::NAME; \
  }

  RL_ENTITY_TYPE_LIST
  #undef X

  map_[id_of]->file_   = new_file;
  map_[id_of]->id_     = id_of;
  map_[id_of]->parent_ = old->parent_;
  map_[id_of]->pos_    = new_pos;
  map_[id_of]->line_   = new_line;
  map_[id_of]->chldrn_ = std::move(old->chldrn_);
  map_[id_of]->name_   = std::move(old->name_);
  map_[id_of]->lname_  = std::move(old->lname_);

  return Entity::cast<T>(map_[id_of]);
}

template<typename T, typename... Args>
auto EntityTable::swap_placeholder(
  const Entity::ID id_of,
  const Entity::Ptr<>& parent_ptr,
  const size_t new_pos,
  const uint32_t new_line,
  const InputFile::ID new_file,
  Args&&... args ) -> Result<Entity::Ptr<T>>
{
  EntityType type = EntityType::None;
  auto old = Entity::cast<PlaceHolder>(find(id_of));

  #define X(NAME)                 \
  if constexpr(IsSame<T, NAME>) { \
    type = EntityType::NAME;      \
  }

  RL_ENTITY_TYPE_LIST
  #undef X

  if(old->to_be_ == EntityType::None
    || ( old->to_be_.is_udt() && type.is_udt() ))
  {
    old->to_be_ = type;
  }

  if(old->to_be_ != type) {
    auto msg = fmt(
      "Expected entity \"{}\" to be of type "
      "\"{}\" (because of a previous declaration)"
      ", got \"{}\" instead.",
      old->name_,
      old->to_be_.to_string(),
      type.to_string()
    );

    return Error(ErrC::InvalidArg, msg);
  }

  return swap_entity<T>(
    id_of,
    parent_ptr,
    new_pos,
    new_line,
    new_file,
    std::forward(args)...);
}

template<typename T, typename... Args>
auto EntityTable::swap_placeholder(
  const Entity::ID id_of,
  const Entity::ID parent_id,
  const size_t new_pos,
  const uint32_t new_line,
  const InputFile::ID new_file,
  Args&&... args ) -> Result<Entity::Ptr<T>>
{
  EntityType type = EntityType::None;
  auto old = Entity::cast<PlaceHolder>(find(id_of));

  #define X(NAME)                 \
  if constexpr(IsSame<T, NAME>) { \
    type = EntityType::NAME;      \
  }

  RL_ENTITY_TYPE_LIST
  #undef X

  if(old->to_be_ == EntityType::None
    || ( old->to_be_.is_udt() && type.is_udt() ))
  {
    old->to_be_ = type;
  }

  if(old->to_be_ != type) {
    auto msg = fmt(
      "Expected entity \"{}\" to be of type "
      "\"{}\" (because of a previous declaration)"
      ", got \"{}\" instead.",
      old->name_,
      old->to_be_.to_string(),
      type.to_string()
    );

    return Error(ErrC::InvalidArg, msg);
  }

  return swap_entity<T>(
    id_of,
    parent_id,
    new_pos,
    new_line,
    new_file,
    std::forward(args)...);
}

template<typename T>
auto EntityTable::find_if(T&& pred) const -> Maybe<Entity::Ptr<>> {
  for(const auto &[id, ent] : this->map_) {
    if(pred(static_cast<const Entity::Ptr<>&>(ent))) {
      return static_cast<const Entity::Ptr<>&>(ent);
    }
  }
  return Nothing;
}

END_NAMESPACE(rl);