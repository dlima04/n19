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
  map_[curr_id_++] = std::make_shared<BuiltinType>(BuiltinType::I8);
  map_[curr_id_++] = std::make_shared<BuiltinType>(BuiltinType::U8);
  map_[curr_id_++] = std::make_shared<BuiltinType>(BuiltinType::I16);
  map_[curr_id_++] = std::make_shared<BuiltinType>(BuiltinType::U16);
  map_[curr_id_++] = std::make_shared<BuiltinType>(BuiltinType::I32);
  map_[curr_id_++] = std::make_shared<BuiltinType>(BuiltinType::U32);
  map_[curr_id_++] = std::make_shared<BuiltinType>(BuiltinType::I64);
  map_[curr_id_++] = std::make_shared<BuiltinType>(BuiltinType::U64);
  map_[curr_id_++] = std::make_shared<BuiltinType>(BuiltinType::F32);
  map_[curr_id_++] = std::make_shared<BuiltinType>(BuiltinType::F64);
  map_[curr_id_++] = std::make_shared<BuiltinType>(BuiltinType::Bool);
  map_[curr_id_++] = std::make_shared<BuiltinType>(BuiltinType::Ptr);

  // Make each builtin type's parent the root entity,
  // add each builtin type as a child of the root.
  for(auto &[ID, ptr] : map_) {
    ptr->parent_ = root_->id_;
    ptr->id_ = ID;
    root_->children_.emplace_back(ID);
  }

  // Finally, add the root entity to the lookup table.
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