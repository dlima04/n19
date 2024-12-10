/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef ENTITYTABLE_HPP
#define ENTITYTABLE_HPP
#include <Frontend/Entity.hpp>
#include <Core/ClassTraits.hpp>
#include <Core/Fmt.hpp>
#include <Core/Panic.hpp>
#include <Core/Result.hpp>
#include <unordered_map>
#include <print>

namespace n19 {
  class EntityTable;
}

class n19::EntityTable {
N19_MAKE_NONCOPYABLE(EntityTable);
N19_MAKE_NONMOVABLE(EntityTable);
public:
  template<typename T, typename ...Args>
  auto insert(
    Entity::ID parent_id,
    size_t pos,
    uint32_t line,
    const std::string& file,
    const std::string& lname,
    Args... args
  ) -> Entity::Ptr<>;

  template<typename T, typename ...Args>
  auto insert(
    Entity::Ptr<> parent,
    size_t pos,
    uint32_t line,
    const std::string& file,
    const std::string& lname,
    Args... args
  ) -> Entity::Ptr<>;

  auto exists(Entity::ID id) const -> bool;
  auto find(Entity::ID id)   const -> Entity::Ptr<>;

  std::unordered_map<Entity::ID, Entity::Ptr<>> map_;
  std::shared_ptr<RootEntity> root_ = nullptr;

  ~EntityTable() = default;
  explicit EntityTable(const std::string& name);
private:
  Entity::ID curr_id_ = 1;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename ...Args>
auto n19::EntityTable::insert(
  const Entity::Ptr<> parent,
  const size_t pos,
  const uint32_t line,
  const std::string& file,
  const std::string& lname,
  Args... args ) -> Entity::Ptr<>
{
  ASSERT(parent != nullptr);
  ASSERT(line != 0);

  const auto id     = curr_id_;
  map_[id]          = std::make_shared<T>(args...);
  map_[id]->file_   = file;
  map_[id]->id_     = id;
  map_[id]->parent_ = parent->id_;
  map_[id]->lname_  = lname;
  map_[id]->name_   = parent->name_ + fmt("::{}", lname);
  map_[id]->pos_    = pos;
  map_[id]->line_   = line;

  #define X(NAME)                       \
  if constexpr(IsSame<T, NAME>) {       \
    map_[id]->type_ = EntityType::NAME; \
  }

  N19_ENTITY_TYPE_LIST
  #undef X

  ++curr_id_;
  return map_[id];
}

template<typename T, typename ...Args>
auto n19::EntityTable::insert(
  const Entity::ID parent_id,
  const size_t pos,
  const uint32_t line,
  const std::string& file,
  const std::string& lname,
  Args... args ) -> Entity::Ptr<>
{
  ASSERT(exists(parent_id));
  ASSERT(line != 0);

  const auto id = curr_id_;
  const auto parent = find(parent_id);

  map_[id]          = std::make_shared<T>(args...);
  map_[id]->file_   = file;
  map_[id]->id_     = id;
  map_[id]->parent_ = parent->id_;
  map_[id]->lname_  = lname;
  map_[id]->name_   = parent->name_ + fmt("::{}", lname);
  map_[id]->pos_    = pos;
  map_[id]->line_   = line;

  #define X(NAME)                       \
  if constexpr(IsSame<T, NAME>) {       \
    map_[id]->type_ = EntityType::NAME; \
  }

  N19_ENTITY_TYPE_LIST
  #undef X

  ++curr_id_;
  return map_[id];
}

#endif //ENTITYTABLE_HPP
