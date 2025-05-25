/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef ENTITY_HPP
#define ENTITY_HPP
#include <Core/Maybe.hpp>
#include <Core/Platform.hpp>
#include <Core/Concepts.hpp>
#include <Core/Panic.hpp>
#include <IO/Console.hpp>
#include <System/String.hpp>
#include <cstdint>
#include <string>
#include <vector>
#include <memory>

#define N19_ROOT_ENTITY_ID 1
#define N19_INVALID_ENTITY_ID 0

#define N19_EQ_FLAG_LIST                   \
  X(None,      0ULL)                       \
  X(Constant,  1ULL)                       \
  X(Reference, 1ULL << 1)                  \
  X(Rvalue,    1ULL << 2)                  \
  X(Volatile,  1ULL << 3)                  \
  X(Poison,    1ULL << 4)                  \

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
  X(Struct)      /* C-style structures  */ \
  X(AliasType)   /* Indirection, Type   */ \
  X(BuiltinType) /* Builtin, e.g. "int" */ \

BEGIN_NAMESPACE(n19);
class EntityTable;

#define X(NAME) class NAME;
  N19_ENTITY_TYPE_LIST
#undef X

enum class EntityType : uint16_t {
#define X(NAME) NAME,
  N19_ENTITY_TYPE_LIST
  None,
#undef X
};

class Entity {
public:
  template<typename T = Entity>
  using Ptr = std::shared_ptr<T>;
  using ID  = uint32_t;
  using Children = std::vector<ID>;

  Entity::ID  id_     = N19_INVALID_ENTITY_ID;
  Entity::ID  parent_ = N19_INVALID_ENTITY_ID;
  uint32_t    line_   = 0;
  size_t      pos_    = 0;
  EntityType  type_   = EntityType::None;
  sys::String file_;
  std::string lname_;
  std::string name_;
  Children    chldrn_;

  template<typename To, typename From>
  static auto cast(Entity::Ptr<From> p) -> Entity::Ptr<To> {
    auto out = std::dynamic_pointer_cast<To>(p);
    ASSERT(out);
    return out;
  }

  template<typename To, typename From>
  static auto try_cast(Entity::Ptr<From> p) -> Entity::Ptr<To> {
    return std::dynamic_pointer_cast<To>(p);
  }

  auto print_(
    uint32_t depth,
    OStream& stream
  ) const -> void;

  auto print_children_(
    uint32_t depth,
    OStream& stream,
    EntityTable& table
  ) const -> void;

  virtual auto print(
    uint32_t depth,
    OStream& stream,
    EntityTable& table
  ) const -> void = 0;

  virtual ~Entity() = default;
  Entity() = default;
};

class RootEntity final : public Entity {
public:
  auto print(uint32_t depth,
    OStream &stream,
    EntityTable &table
  ) const -> void override;

  RootEntity() = default;
 ~RootEntity() override = default;
};

/// Base class. Represents a reference to a n19::Type.
/// n19::TypeDescriptorBase applies qualifications to this type,
/// namely pointer depth, constness and array lengths.
/// this class does not represent the type itself.
class EntityQualifierBase {
public:
  NODISCARD_ auto is_constant() const -> bool;
  NODISCARD_ auto is_rvalue()   const -> bool;
  NODISCARD_ auto is_pointer()  const -> bool;
  NODISCARD_ auto is_array()    const -> bool;
  NODISCARD_ auto is_matrice()  const -> bool;

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

/// Represents a fully resolved reference to a n19::Type.
/// Holds an entity ID and provides a way to access this
/// entity.
class EntityQualifier final
  : public EntityQualifierBase {
public:
  NODISCARD_ auto to_string(
    const EntityTable& tbl,
    bool include_qualifiers = true,
    bool include_postfixes = true
  ) const -> std::string;

  NODISCARD_ auto format() const -> std::string;
  static auto get_const_bool() -> EntityQualifier;
  static auto get_const_f64()  -> EntityQualifier;
  static auto get_const_ptr()  -> EntityQualifier;

  Entity::ID id_ = 0;

  ~EntityQualifier() = default;
  EntityQualifier()  = default;
};

/// Represents an unresolved reference to a n19::Type.
/// Each type is represented as a relative namespace path to
/// a type that may or may not exist. Can be resolved into
/// a n19::TypeDescriptor.
class EntityQualifierThunk final
  : public EntityQualifierBase {
public:
  NODISCARD_ auto to_string(
    bool include_qualifiers = true,
    bool include_postfixes = true
  ) const -> std::string;
  NODISCARD_ auto format() const -> std::string;

  std::string name_;

  ~EntityQualifierThunk() = default;
  EntityQualifierThunk()  = default;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline auto EntityQualifierBase::is_constant() const -> bool { return flags_ & Constant; }
inline auto EntityQualifierBase::is_rvalue()   const -> bool { return flags_ & Rvalue; }
inline auto EntityQualifierBase::is_pointer()  const -> bool { return ptr_depth_ > 0; }
inline auto EntityQualifierBase::is_array()    const -> bool { return !arr_lengths_.empty(); }
inline auto EntityQualifierBase::is_matrice()  const -> bool { return arr_lengths_.size() > 1; }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///
/// These entities are generated by the parser
/// during situations where we do not yet know whether
/// a type exists yet (i.e. it's used somewhere
/// before the declaration).
class PlaceHolder final : public Entity {
public:
  auto print(uint32_t depth,
    OStream &stream,
    EntityTable &table
  ) const -> void override;

  PlaceHolder() = default;
 ~PlaceHolder() override = default;
};

class SymLink : public Entity {
public:
  auto print(uint32_t depth,
    OStream &stream,
    EntityTable &table
  ) const -> void override;

  Entity::ID link_ = N19_INVALID_ENTITY_ID;
  SymLink() = default;
 ~SymLink() override = default;
};

class Variable final : public Entity {
public:
  auto print(uint32_t depth,
    OStream &stream,
    EntityTable &table
  ) const -> void override;

  EntityQualifierBase quals_;
  Entity::ID type_ = N19_INVALID_ENTITY_ID;

  Variable() = default;
 ~Variable() override = default;
};

class Type : public Entity {
public:
  auto print(uint32_t depth,
    OStream &stream,
    EntityTable &table
  ) const -> void override;

  Type() = default;
 ~Type() override = default;
};

class AliasType final : public SymLink {
public:
  auto print(uint32_t depth,
    OStream &stream,
    EntityTable &table
  ) const -> void override;

  EntityQualifierBase quals_;
  AliasType() = default;
 ~AliasType() override = default;
};

class Static final : public Entity {
public:
  auto print(uint32_t depth,
    OStream &stream,
    EntityTable &table
  ) const -> void override;

  Static() = default;
 ~Static() override = default;
};

///
/// For callable procedures.
/// if the return_type_ is N19_INVALID_ENTITY_ID,
/// the return type is void.
class Proc : public Entity {
public:
  auto print(uint32_t depth,
    OStream &stream,
    EntityTable &table
  ) const -> void override;

  std::vector<Entity::ID> parameters_;
  Entity::ID return_type_ = N19_INVALID_ENTITY_ID;

  Proc() = default;
 ~Proc() override = default;
};

class Struct : public Type {
public:
  auto print(uint32_t depth,
    OStream &stream,
    EntityTable &table
  ) const -> void override;

  struct Member {
    std::string name_;
    EntityQualifierBase quals_;
    Entity::ID type_id_;
  };

  std::vector<Member> members_;

  Struct() = default;
 ~Struct() override = default;
};

///
/// These entities should always exist
/// in the table by default, and should never be inserted
/// after the table is created.
class BuiltinType final : public Type {
public:
#define X(TYPE, UNUSED, VALUE) TYPE = VALUE,
  enum Type : Entity::ID {
    N19_ENTITY_BUILTIN_LIST
    AfterLastID
  } builtin_type_ = AfterLastID;
#undef X

  auto print(uint32_t depth,
    OStream &stream,
    EntityTable &table
  ) const -> void override;

  ~BuiltinType() override = default;
  explicit BuiltinType(Type type);
};

END_NAMESPACE(n19);
#endif //ENTITY_HPP
