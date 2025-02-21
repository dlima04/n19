/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef ENTITYQUALIFIER_HPP
#define ENTITYQUALIFIER_HPP
#include <Frontend/EntityTable.hpp>
#include <Core/Platform.hpp>
#include <vector>
#include <memory>
#include <cstdint>

#define N19_EQ_FLAG_LIST   \
  X(None, 0ULL)            \
  X(Constant, 1ULL)        \
  X(Reference, 1ULL << 1)  \
  X(Rvalue, 1ULL << 2)     \

BEGIN_NAMESPACE(n19);

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

  std::vector<std::string> name_;

  ~EntityQualifierThunk() = default;
  EntityQualifierThunk()  = default;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline auto EntityQualifierBase::is_constant() const -> bool {
  return flags_ & Constant;
}

inline auto EntityQualifierBase::is_rvalue() const -> bool {
  return flags_ & Rvalue;
}

inline auto EntityQualifierBase::is_pointer() const -> bool {
  return ptr_depth_ > 0;
}

inline auto EntityQualifierBase::is_array() const -> bool {
  return !arr_lengths_.empty();
}

inline auto EntityQualifierBase::is_matrice() const -> bool {
  return arr_lengths_.size() > 1;
}

END_NAMESPACE(n19);
#endif //ENTITYQUALIFIER_HPP
