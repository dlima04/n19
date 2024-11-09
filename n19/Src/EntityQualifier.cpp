#include <EntityQualifier.h>
#include <ConManip.h>

auto n19::EntityQualifier::format() const -> std::string {
  std::string buff;
  #define X(EQ_FLAG, UNUSED)                    \
  if(flags_ & n19::EntityQualifier::EQ_FLAG){   \
    buff += #EQ_FLAG " | ";                     \
  }
  N19_EQ_FLAG_LIST
  #undef X

  if(!buff.empty()) {
    buff.erase(buff.size() - 3);
    buff += ", ";
  }

  buff += fmt("ptr_depth = {}, ", ptr_depth_);
  buff += generics_.empty() ? "has_generics = True, " : "has_generics = False, ";
  buff += arr_lengths_.empty() ? "array_lengths = N/A " : "array_lengths = ";

  for(const auto& length : arr_lengths_) {
    buff += fmt("{} ", length);
  }

  return fmt(
    "{}ID: {} {}{}{}{}",         // Fmt string
    manip_string(Con::Blue),     // ID color = blue
    static_cast<uint32_t>(id_),  // The ID
    manip_string(Con::Reset),    // Reset console
    manip_string(Con::White),    // buff = white
    buff,                        // The buff
    manip_string(Con::Reset)     // Reset console
  );
}

auto n19::EntityQualifier::get_const_bool()
-> EntityQualifier
{
  EntityQualifier const_bool;
  const_bool.id_    = BuiltinType::Bool;
  const_bool.flags_ = Constant;
  return const_bool;
}

auto n19::EntityQualifier::get_const_f64()
-> EntityQualifier
{
  EntityQualifier const_f64;
  const_f64.id_     = BuiltinType::F64;
  const_f64.flags_  = Constant;
  return const_f64;
}

auto n19::EntityQualifier::get_const_ptr()
-> EntityQualifier
{
  EntityQualifier const_ptr;
  const_ptr.id_        = BuiltinType::Ptr;
  const_ptr.ptr_depth_ = 1;
  const_ptr.flags_     = Constant;
  return const_ptr;
}

auto n19::EntityQualifier::to_string(
  const EntityTable &tbl,
  const bool include_qualifiers,
  const bool include_postfixes ) const -> std::string
{
  std::string buff = "";
  const auto ent = tbl.find(id_);

  if(include_postfixes) {
    #define X(VAL, UNUSED) \
      if(flags_ & VAL) buff.append(#VAL " ");
    N19_EQ_FLAG_LIST // Convert postfixes to string
    #undef X
  }

  buff += ent->name_;
  if(!include_qualifiers) {
    return buff;
  }

  if(!generics_.empty()) {
    buff.append("$[");
    for(const auto& gen : generics_) {
      buff += fmt("{}, ", gen.to_string(tbl));
    }
    buff.erase(buff.size()-2);
    buff.append("]");
  } for(const auto& len : arr_lengths_) {
    buff.append(fmt("[{}]", len));
  } for(uint32_t i = 0; i < ptr_depth_; i++) {
    buff.append("*");
  }

  return buff;
}
