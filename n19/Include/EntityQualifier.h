#ifndef TYPEDESCRIPTOR_H
#define TYPEDESCRIPTOR_H
#include <EntityTable.h>
#include <vector>
#include <memory>
#include <cstdint>

#define N19_EQ_FLAG_LIST   \
  X(None, 0ULL)            \
  X(Constant, 1ULL)        \
  X(Rvalue, 1ULL << 1)     \

namespace n19 {
  class EntityQualifierBase;
  class EntityQualifier;
  class EntityQualifierThunk;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Base class. Represents a reference to a n19::Type.
// n19::TypeDescriptorBase applies qualifications to this type,
// namely pointer depth, constness and array lengths.
// this class does not represent the type itself.
class n19::EntityQualifierBase {
public:
  [[nodiscard]] auto is_constant() const -> bool;
  [[nodiscard]] auto is_rvalue()   const -> bool;
  [[nodiscard]] auto is_pointer()  const -> bool;
  [[nodiscard]] auto is_array()    const -> bool;
  [[nodiscard]] auto is_matrice()  const -> bool;

  enum Flags : uint8_t {
  #define X(NAME, VALUE) NAME = VALUE,
    N19_EQ_FLAG_LIST
  #undef X
  };

  std::vector<uint32_t> arr_lengths_;
  uint32_t ptr_depth_ = 0;
  uint8_t flags_      = 0;

  ~EntityQualifierBase() = default;
  EntityQualifierBase() = default;
};

// Represents a fully resolved reference to a n19::Type.
// Holds an entity ID and provides a way to access this
// entity.
class n19::EntityQualifier final
  : public EntityQualifierBase {
public:
  [[nodiscard]] auto to_string(
    const EntityTable& tbl,
    bool include_qualifiers = true,
    bool include_postfixes = true
  ) const -> std::string;

  [[nodiscard]] auto get_entity_ptr(
    const EntityTable& tbl
  ) const -> Entity::Ptr<Type>;

  [[nodiscard]] auto format() const -> std::string;
  static auto get_const_bool() -> EntityQualifier;
  static auto get_const_f64()  -> EntityQualifier;
  static auto get_const_ptr()  -> EntityQualifier;

  std::vector<EntityQualifier> generics_;
  Entity::ID id_ = 0;

  ~EntityQualifier() = default;
  EntityQualifier()  = default;
};

// Represents an unresolved reference to a n19::Type.
// Each type is represented as a relative namespace path to
// a type that may or may not exist. Can be resolved into
// a joy::TypeDescriptor.
class n19::EntityQualifierThunk final
  : public EntityQualifierBase {
public:
  [[nodiscard]] auto to_string(
    bool include_qualifiers = true,
    bool include_postfixes = true
  ) const -> std::string;
  [[nodiscard]] auto format() const -> std::string;

  std::vector<std::string> name_;
  std::vector<EntityQualifierThunk> generics_;

  ~EntityQualifierThunk() = default;
  EntityQualifierThunk()  = default;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline auto n19::EntityQualifier::get_entity_ptr(const EntityTable &tbl) const
-> Entity::Ptr<Type> {
  return std::dynamic_pointer_cast<Type>(tbl.find(id_));
}

inline auto n19::EntityQualifierBase::is_constant() const
-> bool {
  return flags_ & Constant;
}

inline auto n19::EntityQualifierBase::is_rvalue() const
-> bool {
  return flags_ & Rvalue;
}

inline auto n19::EntityQualifierBase::is_pointer() const
-> bool {
  return ptr_depth_ > 0;
}

inline auto n19::EntityQualifierBase::is_array() const
-> bool {
  return !arr_lengths_.empty();
}

inline auto n19::EntityQualifierBase::is_matrice() const
-> bool {
  return arr_lengths_.size() > 1;
}

#endif //TYPEDESCRIPTOR_H
