/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Frontend/Entity.hpp>

n19::BuiltinType::BuiltinType(const Type type) {
  #define X(TYPE, STR, _1) if(type == TYPE){ lname_ = STR; name_ = "::" STR; }
  N19_ENTITY_BUILTIN_LIST
  #undef X
}
