/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#include <Frontend/Entity.h>

n19::BuiltinType::BuiltinType(const Type type) : n19::Type(EntityType::BuiltinType) {
  #define X(TYPE, STR, _1) if(type == TYPE){ lname_ = STR; name_ = "::" STR; }
  N19_ENTITY_BUILTIN_LIST
  #undef X
}
