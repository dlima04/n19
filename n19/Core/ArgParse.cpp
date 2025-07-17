/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <n19/Core/ArgParse.hpp>
#include <n19/Core/Panic.hpp>
#include <n19/Core/Fmt.hpp>
#include <string>
#include <algorithm>
#include <ranges>
BEGIN_NAMESPACE(n19::argp);

auto Value<int64_t>::convert(const sys::String& str) -> Result<void> {
  try {
    const auto val = std::stoll(str);     /// Try conversion of "str".
    value_ = static_cast<int64_t>(val);   /// store it.
  } catch(const std::invalid_argument&) { ///
    return Error{ErrC::Conversion};       /// handle conversion error.
  } catch(const std::exception& e) {      /// If it's some other exception, panic.
    PANIC(e.what());                      ///
  }

  return Result<void>::create();
}

auto Value<bool>::convert(const sys::String& str) -> Result<void> {
  if(str.empty() || str == _nstr("true")) {
    value_ = true;                        /// "true" and an empty string are truthy.
  } else if(str == _nstr("false")){       ///
    value_ = false;                       /// If not true/false/"", error.
  } else {                                ///
    return Error{ErrC::Conversion};
  }

  return Result<void>::create();
}

auto Value<double>::convert(const sys::String& str) -> Result<void> {
  try {
    const auto val = std::stod(str);      /// Try conversion of "str".
    value_ = static_cast<double>(val);    /// store it.
  } catch(const std::invalid_argument&) { ///
    return Error{ErrC::Conversion};       /// handle conversion error.
  } catch(const std::exception& e) {      /// If it's some other exception, panic.
    PANIC(e.what());                      ///
  }

  return Result<void>::create();
}

auto Value<sys::String>::convert(const sys::String& str) -> Result<void> {
  value_ = str;                           /// Nothing to do for this one.
  return Result<void>::create();          /// just store the string and leave...
}

auto Value<PackType>::convert(const sys::String& str) -> Result<void> {
  value_.clear();
  auto split_views = str | std::ranges::views::split(_nchr(','));

  for(auto&& view : split_views) {
    value_.emplace_back(sys::String{ view.begin(), view.end() });
  }

  return Result<void>::create();
}

auto Parser::already_passed_(const size_t index ) const -> bool {
  ASSERT(index < args_.size());           /// Assert Not OOB
  for(size_t i = 0; i < index; ++i) {     /// Check all elements before index
    if(args_[i] == args_[index]) return true;
  }
                                          ///
  return false;                           /// Otherwise we haven't passed it.
}

auto Parser::print_chunk_error_(
  const size_t at, OStream& stream, const std::string& msg ) const -> void
{
  sys::String filler;                     /// String for filler characters
  filler.reserve(50);                     ///
  for(size_t i = 0; i < args_.size(); ++i) {
    if(i != at) {                         /// Regular chunk.
      stream << args_[i] << _nchr(' ');   /// Don't use any colour.
      continue;                           ///
    }

    stream << args_[i] << _nchr(' ');     /// display the problem
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

  stream << msg;
  stream << Endl;
}

auto Parser::is_flag_begin_(const sys::StringView& str) const -> bool {
  switch(arg_style_) {
    case ArgStyle::UNIX : return str.starts_with(_nstr("-"));
    case ArgStyle::DOS  : return str.starts_with(_nstr("/"));
    case ArgStyle::Masq : return str.starts_with(_nstr(":"));
    default: break;
  }

  UNREACHABLE_ASSERTION;
}

auto Parser::help(OStream& stream) const -> void {
  stream << "\n";
  for(const auto& param : params_) {
    stream << fmt(_nstr("{:<18} {:<13} {}\n"), param.long_, param.short_, param.desc_);
  }

  outs() << Endl;
}

auto Parser::parse(OStream& stream) -> Result<void> {
  for(size_t i = 0; i < args_.size(); i++) {
    if(!is_flag_begin_(args_[i])) {
      print_chunk_error_(i, stream, "Invalid flag format.");
      return Error{ErrC::InvalidArg};
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
    } else if(i + 1 < args_.size() && !is_flag_begin_(args_[i + 1])) {
      the_flag  = args_[i];
      the_value = args_[i + 1];
      ++i;
    } else {
      the_flag  = args_[i];
      the_value = _nstr("");
    }

    auto param_ptr = std::ranges::find_if(params_, [&](const Parameter& p) {
       return p.long_ == the_flag || p.short_ == the_flag;
    });

    if(param_ptr == params_.end()) {
      print_chunk_error_(i, stream, "Flag does not exist.");
      return Error(ErrC::InvalidArg);
    }
    if(the_value == _nstr("=")) {
      print_chunk_error_(i, stream, "Expected a value after \"=\"");
      return Error(ErrC::InvalidArg);
    }
    if(already_passed_(flag_pos)) {
      print_chunk_error_(flag_pos, stream, "Flag was passed more than once.");
      return Error(ErrC::InvalidArg);
    }
    if(the_value.starts_with(_nstr("="))) {
      the_value.erase(0, 1);
    }
    if(!param_ptr->val_->convert(the_value).has_value()) {
      print_chunk_error_(i, stream, "Invalid type for this value.");
      return Error(ErrC::InvalidArg);
    }
  }

  return Result<void>::create();
}


END_NAMESPACE(n19::argp);
