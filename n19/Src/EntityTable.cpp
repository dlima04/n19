#include <EntityTable.h>

n19::EntityTable::EntityTable(const std::string& name) {
  // Initialize the root entity.
  curr_id_      = N19_ROOT_ENTITY_ID;
  root_         = std::make_shared<RootEntity>();
  root_->file_  = name;
  root_->id_    = curr_id_++;
  root_->line_  = 0;
  root_->pos_   = 0;
  root_->lname_ = "::";
  root_->name_  = "::";

  // Create all builtin types and initialize them.
  // Ensure that their parent is the root entity.
  constexpr BuiltinType::Type builtins[] = {
  #define X(TYPE, UNUSED) BuiltinType::Type{BuiltinType::TYPE},
    N19_ENTITY_BUILTIN_LIST
  #undef X
  };

  for(const auto& type : builtins) {
    auto ptr = std::make_shared<BuiltinType>(type);
    auto id  = curr_id_++;
    ptr->id_ = id;
    ptr->parent_ = root_->id_;
    root_->children_.emplace_back(id);
    map_[id] = std::move(ptr);
  }

  // Add the root entity to the lookup table.
  map_[root_->id_] = root_;
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

auto n19::EntityTable::find(const Entity::ID id)
-> Entity::Ptr<> {
  ASSERT(exists(id));
  return map_[id];
}