/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" License.
* See the LICENSE file located at this project's root directory for
* more information.
*/

#include <EntityTable.h>
#include <algorithm>

n19::EntityTable::EntityTable(const std::string& name) {
  // Initialize the root entity.
  root_         = std::make_shared<RootEntity>();
  root_->file_  = name;
  root_->id_    = N19_ROOT_ENTITY_ID;
  root_->line_  = 0;
  root_->pos_   = 0;
  root_->lname_ = "::";
  root_->name_  = "::";

  // Create all builtin types and initialize them.
  // Ensure that their parent is the root entity.
  constexpr BuiltinType::Type builtins[] = {
  #define X(TYPE, _1, _2) BuiltinType::Type{BuiltinType::TYPE},
    N19_ENTITY_BUILTIN_LIST
  #undef X
  };

  for(const auto& type : builtins) {
    auto ptr = std::make_shared<BuiltinType>(type);
    auto id  = static_cast<Entity::ID>(type);
    ptr->id_ = id;
    ptr->parent_ = root_->id_;
    root_->children_.emplace_back(id);
    map_[id] = std::move(ptr);
  }

  // Add the root entity to the lookup table,
  // set the current_id_ to AFTER the last Builtin.
  map_[root_->id_] = root_;
  curr_id_ = BuiltinType::AfterLastID;
}

auto n19::EntityTable::get_root_entity() const
-> Entity::Ptr<RootEntity> {
  return root_;
}

auto n19::EntityTable::exists(const Entity::ID id) const
-> bool {
  ASSERT(id != N19_INVALID_ENTITY_ID);
  return map_.contains(id);
}

auto n19::EntityTable::find(const Entity::ID id) const
-> Entity::Ptr<> {
  ASSERT(exists(id));
  return map_.at(id);
}