/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef ENTITYTABLE_H
#define ENTITYTABLE_H
#include <Entity.h>
#include <Fmt.h>
#include <Panic.h>
#include <Result.h>
#include <unordered_map>
#include <print>

namespace n19 {
  class EntityTable;
}

class n19::EntityTable {
public:
  EntityTable(const EntityTable&)             = delete;
  EntityTable& operator=(const EntityTable&)  = delete;
  EntityTable(const EntityTable&&)            = delete;
  EntityTable& operator=(const EntityTable&&) = delete;

  template<typename T> requires IsEntity<T>
  auto insert(
    Entity::ID parent_id,
    size_t pos,
    uint32_t line,
    const std::string &file,
    const std::string &lname
  ) -> Entity::Ptr<>;

  template<typename T> requires IsEntity<T>
  auto insert(
    Entity::Ptr<> parent,
    size_t pos,
    uint32_t line,
    const std::string &file,
    const std::string &lname
  ) -> Entity::Ptr<>;

  auto exists(Entity::ID id) const -> bool;
  auto entries()             const -> const std::unordered_map<Entity::ID, Entity::Ptr<>>&;
  auto find(Entity::ID id)   const -> Entity::Ptr<>;
  auto get_root_entity()     const -> Entity::Ptr<RootEntity>;

  ~EntityTable() = default;
  explicit EntityTable(const std::string& name);
private:
  // The global lookup table. Allows lookup of entities
  // without traversal through the tree.
  std::unordered_map<Entity::ID, Entity::Ptr<>> map_;
  // The root node for the entity tree.
  std::shared_ptr<RootEntity> root_ = nullptr;
  // An ID that increments upwards with each
  // insertion of an entity into the tree.
  Entity::ID curr_id_ = 1;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T> requires n19::IsEntity<T>
auto n19::EntityTable::insert(
  const Entity::Ptr<> parent,
  const size_t pos,
  const uint32_t line,
  const std::string& file,
  const std::string& lname ) -> Entity::Ptr<>
{
  ASSERT(parent != nullptr);
  ASSERT(exists(parent->id_));
  ASSERT(line != 0);

  const auto id { curr_id_ };
  map_[id]          = std::make_shared<T>();
  map_[id]->file_   = file;
  map_[id]->id_     = id;
  map_[id]->parent_ = parent->id_;
  map_[id]->lname_  = lname;
  map_[id]->name_   = parent->name_ + fmt("::{}", lname);
  map_[id]->pos_    = pos;
  map_[id]->line_   = line;

  ++curr_id_;
  return map_[id];
}

template<typename T> requires n19::IsEntity<T>
auto n19::EntityTable::insert(
  const Entity::ID parent_id,
  const size_t pos,
  const uint32_t line,
  const std::string& file,
  const std::string& lname ) -> Entity::Ptr<>
{
  ASSERT(exists(parent_id));
  ASSERT(line != 0);

  const auto id = curr_id_;
  const auto parent = find(parent_id);

  map_[id]          = std::make_shared<T>();
  map_[id]->file_   = file;
  map_[id]->id_     = id;
  map_[id]->parent_ = parent->id_;
  map_[id]->lname_  = lname;
  map_[id]->name_   = parent->name_ + fmt("::{}", lname);
  map_[id]->pos_    = pos;
  map_[id]->line_   = line;

  ++curr_id_;
  return map_[id];
}

inline auto n19::EntityTable::entries()
const -> const std::unordered_map<Entity::ID, Entity::Ptr<>>& {
  return map_;
}

#endif //ENTITYTABLE_H
