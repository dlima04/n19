#include <Entity.h>

n19::BuiltinType::BuiltinType(const Type type) : n19::Type(EntityType::BuiltinType) {
#define X(TYPE, STR, _1) if(type == TYPE){ lname_ = STR; name_ = "::" STR; }
  N19_ENTITY_BUILTIN_LIST
#undef X
}
