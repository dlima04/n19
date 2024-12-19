/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#include <Core/ArgParse.hpp>
#include <Core/Panic.hpp>
#include <Core/Fmt.hpp>
#include <string>
#include <algorithm>
BEGIN_NAMESPACE(n19::argp);

auto Value<int64_t>::convert(const sys::String& str) -> Result<void> {
  try {
    const auto val = std::stoll(str);     /// Try conversion of "str".
    value_ = static_cast<int64_t>(val);   /// store it.
  } catch(const std::invalid_argument&) { ///
    return make_error(ErrC::Conversion);  /// handle conversion error.
  } catch(const std::exception& e) {      /// If it's some other exception, panic.
    PANIC(e.what());                      ///
  }

  return make_result<void>();
}

auto Value<bool>::convert(const sys::String& str) -> Result<void> {
  if(str.empty() || str == _nstr("true")) {
    value_ = true;                        /// "true" and an empty string are truthy.
  } else if(str != _nstr("false")){       ///
    return make_error(ErrC::Conversion);  /// If not true/false/"", error.
  }                                       ///

  return make_result<void>();
}

auto Value<double>::convert(const sys::String& str) -> Result<void> {
  try {
    const auto val = std::stod(str);      /// Try conversion of "str".
    value_ = static_cast<double>(val);    /// store it.
  } catch(const std::invalid_argument&) { ///
    return make_error(ErrC::Conversion);  /// handle conversion error.
  } catch(const std::exception& e) {      /// If it's some other exception, panic.
    PANIC(e.what());                      ///
  }

  return make_result<void>();
}

auto Value<sys::String>::convert(const sys::String& str) -> Result<void> {
  value_ = str;                           /// Nothing to do for this one.
  return make_result<void>();             /// just store the string and leave...
}

auto Parser::_already_passed(const size_t index ) const -> bool {
  ASSERT(index < args_.size());           /// Assert Not OOB
  for(size_t i = 0; i < index; ++i) {     /// Check all elements before index
    if(args_[i] == args_[index]) return true;
  }
                                          ///
  return false;                           /// Otherwise we haven't passed it.
}

auto Parser::_print_chunk_error(
  const size_t at, OStream& stream, const std::string& msg ) const -> void
{
  sys::String filler;                     /// String for filler characters
  filler.reserve(50);                     ///
  
  for(size_t i = 0; i < args_.size(); ++i) {
    if(i != at) {                         /// Regular chunk.
      stream << args_[i] << _nchr(' ');   /// Don't use any colour.
      continue;                           ///
    }

    stream << Con::Bold << Con::RedFG;    /// Error chunk, use red+bold
    stream << args_[i] << _nchr(' ');     /// display the problem
    stream << Con::Reset;                 ///
  }

  stream << '\n';
  for(size_t i = 0; i < args_.size(); ++i) {
    filler.resize(args_[i].size());
    if(i != at) {
      std::ranges::fill(filler, _nchr('~'));
    } else {
      std::ranges::fill(filler, _nchr('^'));
    }

    stream << filler << _nchr(' ');
  }

  stream << '\n';
  size_t i = 0;

  while(i < args_.size() && i != at) {
    filler.resize(args_[i].size());
    std::ranges::fill(filler, _nchr(' '));
    stream << filler << _nchr(' ');
    ++i;
  }

  // Example of what this should look like:
  // --foo value --my-flag otherval
  // ~~~~~ ~~~~~ ^^^^^^^^^ ~~~~~~~~
  //             This is not a valid flag!

  stream << msg;
}

auto Parser::_is_flag_begin(const sys::StringView& str) const -> bool {
  switch(arg_style_) {
    case ArgStyle::UNIX : return str.starts_with(_nstr("-"));
    case ArgStyle::DOS  : return str.starts_with(_nstr("/"));
    case ArgStyle::Masq : return str.starts_with(_nstr(":"));
    default: break;
  }

  UNREACHABLE;
}

auto Parser::help(OStream& stream) const -> void {
  stream << Con::WhiteFG  << Con::Bold;
  stream << "-- Flags:\n" << Con::Reset;

  for(const auto& param : params_) {
    stream << fmt("{}{:<18} {:<4}{} :: {}\n",
       manip_string(Con::MagentaFG),
       param.long_,
       param.short_,
       manip_string(Con::Reset),
       param.desc_);
  }

  outs() << Endl;
}

auto Parser::parse(OStream& stream) -> Result<void> {
  for(size_t i = 0; i < args_.size(); i++) {
    if(!_is_flag_begin(args_[i])) {
      _print_chunk_error(i, stream, "Invalid flag format.");
      return make_error(ErrC::InvalidArg);
    }

    sys::String the_flag;
    sys::String the_value;

    /// Determine where the value for this flag is,
    /// if it exists. Remember, we can receive flags in 3 ways:
    /// - No value:     "--flag"
    /// - Value-Equals: "--flag=foo"
    /// - Value-After:  "--flag foo"
    /// All of these are valid so we need to account for them all.

    const auto equ_char = args_[i].find_first_of(_nchr('='));
    const auto flag_pos = i;

    if(equ_char != sys::String::npos) {
      the_flag  = args_[i].substr(0, equ_char);
      the_value = args_[i].substr(equ_char);
    } else if(i + 1 < args_.size() && !_is_flag_begin(args_[i + 1])) {
      the_flag  = args_[i];
      the_value = args_[i + 1];
      ++i;
    } else {
      the_flag  = args_[i];
      the_value = "";
    }

    auto param_ptr = std::ranges::find_if(params_, [&](const Parameter& p) {
       return p.long_ == the_flag || p.short_ == the_flag;
    });

    if(param_ptr == params_.end()) {
      _print_chunk_error(i, stream, "Flag does not exist.");
      return make_error(ErrC::InvalidArg);
    }
    
    if(the_value == "=") {
      _print_chunk_error(i, stream, "Expected a value after \"=\"");
      return make_error(ErrC::InvalidArg);
    }

    if(_already_passed(flag_pos)) {
      _print_chunk_error(flag_pos, stream, "Flag was passed more than once.");
      return make_error(ErrC::InvalidArg);
    }
    
    if(the_value.starts_with("=")) {
      the_value.erase(0, 1);
    }

    if(!param_ptr->val_->convert(the_value).has_value()) {
      _print_chunk_error(i, stream, "Invalid type for this value.");
      return make_error(ErrC::InvalidArg);
    }
  }

  return make_result<void>();
}


END_NAMESPACE(n19::argp);
