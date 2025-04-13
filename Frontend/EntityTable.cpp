/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Frontend/EntityTable.hpp>
#include <algorithm>
BEGIN_NAMESPACE(n19);

EntityTable::EntityTable(const std::string& name) {
  /// Initialize the root entity.
  root_         = std::make_shared<RootEntity>();
  root_->file_  = name;
  root_->id_    = N19_ROOT_ENTITY_ID;
  root_->line_  = 0;
  root_->pos_   = 0;
  root_->type_  = EntityType::RootEntity;
  root_->lname_ = "::";
  root_->name_  = "::";

  /// Create all builtin types and initialize them.
  /// Ensure that their parent is the root entity.
  constexpr BuiltinType::Type builtins[] = {
  #define X(TYPE, _1, _2) BuiltinType::Type{BuiltinType::TYPE},
    N19_ENTITY_BUILTIN_LIST
  #undef X
  };

  for(const auto& type : builtins) {
    auto ptr = std::make_shared<BuiltinType>(type);
    auto id  = static_cast<Entity::ID>(type);

    ptr->parent_ = root_->id_;
    ptr->id_     = id;
    ptr->type_   = EntityType::BuiltinType;

    root_->chldrn_.emplace_back(id);
    map_[id] = std::move(ptr);
  }

  /// Add the root entity to the lookup table,
  /// set the current_id_ to AFTER the last Builtin.
  map_[root_->id_] = root_;
  curr_id_ = BuiltinType::AfterLastID;
}

auto EntityTable::resolve_link(Entity::Ptr<SymLink> ptr) const -> Entity::Ptr<> {
  ASSERT(ptr);
  Entity::Ptr<> curr;
  Entity::Ptr<SymLink> next = ptr;

  do {
    ASSERT(next->link_ != N19_INVALID_ENTITY_ID);
    ASSERT(exists(next->link_));
    curr = map_.at(next->link_);
    next = Entity::try_cast<SymLink>(curr);
  } while(next);

  return curr;
}

auto EntityTable::exists(const Entity::ID id) const -> bool {
  ASSERT(id != N19_INVALID_ENTITY_ID);
  return map_.contains(id);
}

auto EntityTable::find(const Entity::ID id) const -> Entity::Ptr<> {
  ASSERT(exists(id));
  auto ptr  = map_.at(id);
  auto link = Entity::try_cast<SymLink>(ptr);
  if(link) return resolve_link(link);
  return ptr;
}

auto EntityTable::dump(OStream& stream) -> void {
  root_->print(0, stream, *this);
}

auto EntityTable::dump_structures(OStream& stream) -> void {
  for(auto &[id, entity] : map_) {
    if(entity->type_ != EntityType::Struct) continue;
    auto ptr = Entity::cast<Struct>(entity);
    stream
      << "-- "
      << Con::Bold
      << Con::MagentaFG
      << ptr->name_
      << Con::Reset
      << "\n";

    for(size_t i = 0; i < ptr->members_.size(); i++) {
      stream
        << i + 1
        << ". "
        << Con::GreenFG
        << ptr->members_[i].name_
        << Con::Reset
        << ": ";

      const auto member_ent = find(ptr->members_[i].type_id_);
      stream << Con::YellowFG;

    #define X(VAL, UNUSED) \
        if(ptr->members_[i].quals_.flags_ & EntityQualifierBase::VAL) \
        { stream << #VAL " "; }

        N19_EQ_FLAG_LIST
    #undef X

      stream << Con::Reset;
      stream << member_ent->name_;

      for(int i = 0; i < ptr->members_[i].quals_.ptr_depth_; i++) {
        stream << "*";
      } for(const auto& len : ptr->members_[i].quals_.arr_lengths_) {
        stream << fmt("[{}]", len);
      }

      stream << "\n";
    }
  }

  stream << "\n";
}

END_NAMESPACE(n19);
