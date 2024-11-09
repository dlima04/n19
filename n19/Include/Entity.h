#ifndef ENTITY_H
#define ENTITY_H
#include <Concepts.h>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#ifndef ENTITYTABLE_H
  static_assert("Must include EntityTable.h before Entity.h.");
#endif

// Some constants:
// The root entity ID ALWAYS begins at 1.
// a value of 0 is an invalid ID.
#define N19_ROOT_ENTITY_ID 1
#define N19_INVALID_ENTITY_ID 0

// All entity types that can be
// found in an EntityTable.
// Can be later represented as an
// enumeration, class name, etc.
#define N19_ENTITY_TYPE_LIST                 \
  X(Entity)                                  \
  X(RootEntity)                              \
  X(Procedure)                               \
  X(Type)                                    \
  X(PlaceHolder)                             \
  X(SymLink)                                 \
  X(Variable)                                \
  X(Static)                                  \
  X(Enum)                                    \
  X(Struct)                                  \
  X(AliasType)                               \
  X(BuiltinType)                             \
  X(GenericType)                             \
  X(GenericProcedure)                        \

// List of builtin entity types.
// - the name
// - the string representation
// - The guaranteed ID it has in the table.
// - ID should always be ROOT + n
#define N19_ENTITY_BUILTIN_LIST              \
  X(I8,   "i8",  N19_ROOT_ENTITY_ID + 1)     \
  X(U8,   "u8",  N19_ROOT_ENTITY_ID + 2)     \
  X(I16,  "i16", N19_ROOT_ENTITY_ID + 3)     \
  X(U16,  "u16", N19_ROOT_ENTITY_ID + 4)     \
  X(I32,  "i32", N19_ROOT_ENTITY_ID + 5)     \
  X(U32,  "u32", N19_ROOT_ENTITY_ID + 6)     \
  X(I64,  "i64", N19_ROOT_ENTITY_ID + 7)     \
  X(U64,  "u64", N19_ROOT_ENTITY_ID + 8)     \
  X(F32,  "f32", N19_ROOT_ENTITY_ID + 9)     \
  X(F64,  "f64", N19_ROOT_ENTITY_ID + 10)    \
  X(Ptr,  "ptr", N19_ROOT_ENTITY_ID + 11)    \
  X(Bool, "bool",N19_ROOT_ENTITY_ID + 12)    \

namespace n19 {
  enum class EntityType : uint16_t {
  #define X(NAME) NAME,
    N19_ENTITY_TYPE_LIST
  #undef X
  };
  #define X(NAME) class NAME;
    N19_ENTITY_TYPE_LIST
  #undef X
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// The base class for all compilation entities.
class n19::Entity {
public:
  template<typename T = Entity> requires IsEntity<T>
  using Ptr = std::shared_ptr<T>;
  using ID  = uint32_t;
  using Children = std::vector<ID>;

  ID id_;
  ID parent_;
  uint32_t line_;
  size_t pos_;
  EntityType type_;
  std::string file_;
  std::string lname_;
  std::string name_;
  Children children_;

  virtual ~Entity() = default;
protected:
  explicit Entity(const EntityType type):
    id_(N19_INVALID_ENTITY_ID),
    parent_(N19_INVALID_ENTITY_ID),
    line_(0),
    pos_(0),
    type_(type) {}
};

// The root entity in the entity tree.
// All other entities are children of this one,
// including the builtin types.
class n19::RootEntity final : public Entity {
public:
  RootEntity() : Entity(EntityType::RootEntity) {}
  ~RootEntity() override = default;
};

// The base class for all Type entities.
class n19::Type : public Entity {
public:
  ~Type() override = default;
protected:
  explicit Type(const EntityType type) : Entity(type) {}
};

class n19::Procedure final : public Entity {
public:

};

// Represents a builtin scalar type,
// including integers, floating point types, and "ptr".
// Can also be referred to as a "primitive".
class n19::BuiltinType final : public Type {
public:
  #define X(TYPE, UNUSED, VALUE) TYPE = VALUE,
  enum Type : Entity::ID {
    N19_ENTITY_BUILTIN_LIST
    AfterLastID
  } type_ = AfterLastID;
  #undef X

  ~BuiltinType() override = default;
  explicit BuiltinType(Type type);
};

#endif //ENTITY_H
