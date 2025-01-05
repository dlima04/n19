/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef ENTITY_HPP
#define ENTITY_HPP
#include <Core/Concepts.hpp>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>

// Some constants:
// The root entity ID begins at 1.
// a value of 0 is an invalid ID.
#define N19_ROOT_ENTITY_ID 1
#define N19_INVALID_ENTITY_ID 0

#define N19_ENTITY_BUILTIN_LIST            \
  X(I8,   "i8",  N19_ROOT_ENTITY_ID + 1)   \
  X(U8,   "u8",  N19_ROOT_ENTITY_ID + 2)   \
  X(I16,  "i16", N19_ROOT_ENTITY_ID + 3)   \
  X(U16,  "u16", N19_ROOT_ENTITY_ID + 4)   \
  X(I32,  "i32", N19_ROOT_ENTITY_ID + 5)   \
  X(U32,  "u32", N19_ROOT_ENTITY_ID + 6)   \
  X(I64,  "i64", N19_ROOT_ENTITY_ID + 7)   \
  X(U64,  "u64", N19_ROOT_ENTITY_ID + 8)   \
  X(F32,  "f32", N19_ROOT_ENTITY_ID + 9)   \
  X(F64,  "f64", N19_ROOT_ENTITY_ID + 10)  \
  X(Ptr,  "ptr", N19_ROOT_ENTITY_ID + 11)  \
  X(Bool, "bool",N19_ROOT_ENTITY_ID + 12)  \

#define N19_ENTITY_TYPE_LIST               \
  X(Entity)      /* Base class for all  */ \
  X(RootEntity)  /* First in the tree   */ \
  X(Proc)        /* Callable procedures */ \
  X(Type)        /* Data types          */ \
  X(PlaceHolder) /* Temporary entities  */ \
  X(SymLink)     /* Indirection entity  */ \
  X(Variable)    /* For local variables */ \
  X(Static)      /* For namespaces, etc */ \
  X(Enum)        /* Enumeration         */ \
  X(Struct)      /* C-style structures  */ \
  X(AliasType)   /* Indirection, Type   */ \
  X(BuiltinType) /* Builtin, e.g. "int" */ \

BEGIN_NAMESPACE(n19);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Begin macro cancer aids fuckery.

#define X(NAME) class NAME;
  N19_ENTITY_TYPE_LIST
#undef X

enum class EntityType : uint16_t {
#define X(NAME) NAME,
  N19_ENTITY_TYPE_LIST
  None,
#undef X
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Begin class definitions

class Entity {
public:
  template<typename T = Entity>
  using Ptr = std::shared_ptr<T>;
  using ID  = uint32_t;
  using Children = std::vector<ID>;

  Entity::ID  id_     = 0;
  Entity::ID  parent_ = 0;
  uint32_t    line_   = 0;
  size_t      pos_    = 0;
  EntityType  type_   = EntityType::None;
  std::string file_;
  std::string lname_;
  std::string name_;
  Children    chldrn_;

  virtual ~Entity() = default;
  Entity() = default;
};

class RootEntity final : public Entity {
public:
  RootEntity() = default;
 ~RootEntity() override = default;
};

class Type : public Entity {
public:
  ~Type() override = default;
  Type() = default;
};

class BuiltinType final : public Type {
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

END_NAMESPACE(n19);
#endif //ENTITY_HPP
