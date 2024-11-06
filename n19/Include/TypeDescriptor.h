#ifndef TYPEDESCRIPTOR_H
#define TYPEDESCRIPTOR_H
#include <EntityTable.h>
#include <vector>
#include <memory>
#include <cstdint>

#define JOY_TD_FLAG_LIST    \
  X(None, 0ULL)             \
  X(IsConstant, 1ULL)       \
  X(IsRvalue, 1ULL << 1)    \

namespace n19 {
  class TypeDescriptorBase;
  class TypeDescriptor;
  class TypeDescriptorThunk;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Base class. Represents a reference to a n19::Type.
// n19::TypeDescriptorBase applies qualifications to this type,
// namely pointer depth, constness and array lengths.
// this class does not represent the type itself.
class n19::TypeDescriptorBase {
public:
  auto is_constant() const -> bool;
  auto is_rvalue()   const -> bool;
  auto is_pointer()  const -> bool;
  auto is_array()    const -> bool;
  auto is_matrice()  const -> bool;

  enum Flags : uint8_t {
  #define X(NAME, VALUE) NAME = VALUE,
    JOY_TD_FLAG_LIST
  #undef X
  };

  std::vector<uint32_t> arr_lengths_;
  uint32_t ptr_depth_ = 0;
  uint8_t flags_      = 0;

  ~TypeDescriptorBase() = default;
  TypeDescriptorBase() = default;
};

// Represents a fully resolved reference to a n19::Type.
// Holds an entity ID and provides a way to access this
// entity.
class n19::TypeDescriptor final : public TypeDescriptorBase {
public:
  auto get(EntityTable &tbl) const -> Entity::Ptr<Type>;

  std::vector<TypeDescriptor> generics_;
  Entity::ID id_ = 0;

  ~TypeDescriptor() = default;
  TypeDescriptor()  = default;
};

// Represents an unresolved reference to a n19::Type.
// Each type is represented as a relative namespace path to
// a type that may or may not exist. Can be resolved into
// a joy::TypeDescriptor.
class n19::TypeDescriptorThunk final : public TypeDescriptorBase {
public:
  std::vector<std::string> name_;
  std::vector<TypeDescriptorThunk> generics_;

  ~TypeDescriptorThunk() = default;
  TypeDescriptorThunk()  = default;
};

#endif //TYPEDESCRIPTOR_H
