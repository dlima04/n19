/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef ENTITYTABLE_HPP
#define ENTITYTABLE_HPP
#include <Frontend/Entity.hpp>
#include <Core/ClassTraits.hpp>
#include <IO/Fmt.hpp>
#include <Core/Panic.hpp>
#include <Core/Result.hpp>
#include <unordered_map>
#include <print>
#include <utility>
BEGIN_NAMESPACE(n19);

class EntityTable {
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
    Args&&... args
  ) -> Entity::Ptr<T>;

  template<typename T, typename ...Args>
  auto insert(
    Entity::Ptr<> parent,
    size_t pos,
    uint32_t line,
    const std::string& file,
    const std::string& lname,
    Args&&... args
  ) -> Entity::Ptr<T>;

  auto resolve_link(Entity::Ptr<SymLink> ptr) const -> Entity::Ptr<>;
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
auto EntityTable::insert(
  const Entity::Ptr<> parent,
  const size_t pos,
  const uint32_t line,
  const std::string& file,
  const std::string& lname,
  Args&&... args ) -> Entity::Ptr<T>
{
  ASSERT(parent != nullptr);
  ASSERT(line != 0);

  const auto id     = curr_id_;
  map_[id]          = std::make_shared<T>(std::forward(args)...);
  map_[id]->file_   = file;
  map_[id]->id_     = id;
  map_[id]->parent_ = parent->id_;
  map_[id]->lname_  = lname;
  map_[id]->pos_    = pos;
  map_[id]->line_   = line;
  map_[id]->name_   = parent->id_ == N19_ROOT_ENTITY_ID
    ? fmt("::{}", lname) : parent->name_ + fmt("::{}", lname);

  #define X(NAME)                       \
  if constexpr(IsSame<T, NAME>) {       \
    map_[id]->type_ = EntityType::NAME; \
  }

  N19_ENTITY_TYPE_LIST
  #undef X

  ++curr_id_;
  return Entity::cast<T>(map_[id]);
}

template<typename T, typename ...Args>
auto EntityTable::insert(
  const Entity::ID parent_id,
  const size_t pos,
  const uint32_t line,
  const std::string& file,
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
  map_[id]->name_   = parent->id_ == N19_ROOT_ENTITY_ID
    ? fmt("::{}", lname) : parent->name_ + fmt("::{}", lname);

  #define X(NAME)                       \
  if constexpr(IsSame<T, NAME>) {       \
    map_[id]->type_ = EntityType::NAME; \
  }

  N19_ENTITY_TYPE_LIST
  #undef X

  ++curr_id_;
  return Entity::cast<T>(map_[id]);
}

END_NAMESPACE(n19);
#endif //ENTITYTABLE_HPP
