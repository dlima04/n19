#include <TypeDescriptor.h>

auto n19::TypeDescriptor::get(EntityTable &tbl) const -> Entity::Ptr<Type> {
  return std::dynamic_pointer_cast<Type>(tbl.find(id_));
}

auto n19::TypeDescriptorBase::is_constant() const -> bool {
  return flags_ & IsConstant;
}

auto n19::TypeDescriptorBase::is_rvalue() const -> bool {
  return flags_ & IsRvalue;
}

auto n19::TypeDescriptorBase::is_pointer() const -> bool {
  return ptr_depth_ > 0;
}

auto n19::TypeDescriptorBase::is_array() const -> bool {
  return !arr_lengths_.empty();
}

auto n19::TypeDescriptorBase::is_matrice() const -> bool {
  return arr_lengths_.size() > 1;
}
