/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#include <Core/ArgParse.hpp>
#include <Core/Panic.hpp>
#include <Core/ConManip.hpp>
#include <Sys/Stream.hpp>
#include <ranges>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <iomanip>

auto n19::argp::Parameter::create(
  const sys::StringView& lf,
  const sys::StringView& sf,
  const std::string_view& desc,
  const bool required,
  Maybe<Value>&& deflt ) -> Parameter
{
  Parameter param;
  param.desc_      = desc;
  param.lf_        = lf;
  param.required_  = required;
  param.sf_        = sf;
  param.default_   = deflt;
  return param;
}

auto n19::argp::Value::to_bool() const -> Result<bool> {
  if(value_ == _nstr("true")) {
    return make_result<bool>(true);
  } if(value_ == _nstr("false")) {
    return make_result<bool>(false);
  }

  return make_error(ErrC::NotFound);
}

auto n19::argp::Value::to_cdvs() const -> Result<std::vector<Value>> {
  if(value_.empty()) {
    return make_error(ErrC::NotFound);
  }

  std::vector<Value> vals;
  for(const auto& str
    : std::ranges::views::split(value_, _nchr(',')))
  {
    vals.emplace_back( sys::String(str.begin(), str.end()) );
  }

  return vals;
}

auto n19::argp::Value::to_f64() const -> Result<double> {
  try {
    const double db = std::stod(value_);
    return db;
  } catch(...) {
    return make_error(ErrC::NotFound);
  }

  UNREACHABLE;
}

auto n19::argp::Value::to_i64() const -> Result<int64_t> {
  try {
    const int64_t the_i64 = std::stoll(value_);
    return the_i64;
  } catch(...) {
    return make_error(ErrC::NotFound);
  }

  UNREACHABLE;
}

auto n19::argp::Parser::add_param(Parameter&& param) -> Parser& {
  ASSERT(param.lf_.starts_with(_nstr("--")));
  ASSERT(param.sf_.starts_with(_nstr("-")));
  params_.emplace_back(param);
  return *this;
}

auto n19::argp::Parser::get_arg(
  const sys::StringView &str ) const -> Result<Value>
{
  const auto the_arg =
  std::ranges::find_if(params_, [&](const Parameter& p) {
    return p.lf_ == str || p.sf_ == str;
  });

  if(the_arg != params_.end() && the_arg->val_.has_value()) {
    return make_result<Value>(the_arg->val_.value());
  } if(the_arg != params_.end() && the_arg->default_.has_value()) {
    return make_result<Value>(the_arg->default_.value());
  }

  return make_error(ErrC::NotFound);
}

auto n19::argp::Parser::_is_valid_argument(
  const sys::String& str ) const -> bool
{
  for(const auto& param : params_) {
    if(param.lf_ == str || param.sf_ == str)
      return true;
  }

  return false;
}

auto n19::argp::Parser::_already_passed(
  const size_t index,
  const std::vector<sys::StringView>& strings ) -> bool
{
  ASSERT(index < strings.size());
  for(size_t i = 0; i < index; i++) {
    if(strings[i] == strings[index]) return true;
  }

  return false;
}

auto n19::argp::Parser::_print_chunk_error(
  const std::string& msg,
  const size_t at,
  const std::vector<sys::StringView>& strings ) -> void
{
  // Unironically this is the best way to do this,
  // 3 loops is better than excessive allocation here
  sys::String spaces;
  sys::String filler;

  for(size_t i = 0; i < strings.size(); i++) {
    if(i != at) {
      sys::outs() << strings[i] << _nchr(' ');
      continue;
    }

    set_console(Con::Bold, Con::Red);
    sys::outs() << strings[i] << _nchr(' ');
    set_console(Con::Reset);
  }

  sys::outs() << _nchr('\n');
  for(size_t i = 0; i < strings.size(); i++) {
    filler.resize( strings[i].size() );
    if(i != at) {
      std::ranges::fill(filler, _nchr('~'));
    } else {
      std::ranges::fill(filler, _nchr('^'));
    }

    sys::outs() << filler << _nchr(' ');
  }

  sys::outs() << _nchr('\n');
  size_t i = 0;

  while(i < strings.size() && i != at) {
    spaces.resize(strings[i].size());
    std::ranges::fill(spaces, _nchr(' '));
    sys::outs() << spaces << _nchr(' ');
    ++i;
  }

  // Example of what this should look like:
  // --foo value --my-flag otherval
  // ~~~~~ ~~~~~ ^^^^^^^^^ ~~~~~~~~
  //             This is not a valid flag!

  std::println("{}", msg);
}

auto n19::argp::Parser::_check_required_params() const -> bool {
  bool success { true };
  for(const auto& param : params_) {
    if(param.required_ && !param.val_ && !param.default_) {
      set_console(Con::Red, Con::Bold);
      std::print("ERROR :: ");
      set_console(Con::Reset);

      std::print("Expected flag: ");
      set_console(Con::Bold);
      sys::outs()
        << param.lf_
        << _nchr(' ')
        << param.sf_
        << _nchr('\n');
      set_console(Con::Reset);
      success = false;
    }
  }

  return success;
}

auto n19::argp::Parser::debug_print() const -> void {
  auto print_value = []<typename T>(
    T&& value, const sys::String& title ) -> void
  {
    sys::outs()
      << std::setw(12)
      << std::left
      << title
      << _nstr(" :: ")
      << value
      << _nchr('\n');
  };

  sys::outs() << std::boolalpha;
  for(const auto& param : params_) {
    set_console(Con::Green);
    std::println("\"{}\"", param.desc_);
    set_console(Con::Reset);

    sys::StringView the_default;
    sys::StringView the_value;

    if(param.default_ && !param.default_->value_.empty()) {
      the_default = param.default_->value_;
    } else {
      the_default = _nstr("N/A");
    }

    if(param.val_ && !param.val_->value_.empty()) {
      the_value = param.val_->value_;
    } else {
      the_value = _nstr("N/A");
    }

    print_value(param.sf_, _nstr("Short Form"));
    print_value(param.lf_, _nstr("Long Form"));
    print_value(param.required_, _nstr("Required"));
    print_value(the_value, _nstr("Value"));
    print_value(the_default, _nstr("Default"));
    sys::outs() << _nchr('\n');
  }

  sys::outs() << std::noboolalpha;
  sys::outs() << _nchr('\n');
}

auto n19::argp::Parser::print() const -> void {
  set_console(Con::White, Con::Bold);
  std::println("-- Flags:");
  set_console(Con::Reset);

  for(const auto& param : params_) {
    set_console(Con::Magenta);
    sys::outs()
      << std::setw(19)
      << std::left
      << param.lf_
      << std::setw(4)
      << std::left
      << param.sf_;
    set_console(Con::Reset);
    std::println(":: {}", param.desc_);
  }

  sys::outs() << _nchr('\n');
}

// Flow:
// 1. Ensure the current string begins with -- or -.
// 2. Check to see if there's a parameter to match it.
// 3. Check to see if there's a value associated with it.
// 4. If there is, check if we passed it already.
// 5. Store the string inside of the parameter.
// 6. If there's a value associated with the string, store it too.
// 7. Repeat steps 1 through 5 on each string.
// 8. Check if all required arguments have been passed.

auto n19::argp::Parser::parse(
  const std::vector<sys::StringView>& chunks ) -> Result<None>
{
  for(size_t i = 0; i < chunks.size(); i++) {
    const bool is_invalid =
      !chunks[i].starts_with(_nstr("-")) &&
      !chunks[i].starts_with(_nstr("--"));
    if(is_invalid) {
      _print_chunk_error("Invalid flag format.", i, chunks);
      return make_error(ErrC::InvalidArg);
    }

    sys::String the_value;
    sys::String the_flag;

    // Determine where the value for this flag is,
    // if it exists. Remember, we can receive flags in 3 ways:
    // - No value:     "--flag"
    // - Value-Equals: "--flag=foo"
    // - Value-After:  "--flag foo"
    // All of these are valid so we need to account for them all.

    const auto equals = chunks[i].find_first_of(_nchr('='));
    const auto flag_index = i;

    if(equals != sys::String::npos) {
      the_flag  = chunks[i].substr(0, equals);
      the_value = chunks[i].substr(equals);
    } else if(i + 1 < chunks.size()
       && !chunks[i + 1].starts_with("-")
       && !chunks[i + 1].starts_with("--"))
    {
      the_flag  = chunks[i];
      the_value = chunks[i + 1];
      ++i;
    } else {
      the_flag  = chunks[i];
      the_value = "";
    }

    //
    // Check if we've received a valid value,
    // and if this is a duplicate argument.
    //

    if(the_value == "=") {
      _print_chunk_error("Expected a value after \"=\"", i, chunks);
      return make_error(ErrC::InvalidArg);
    }

    if(!_is_valid_argument(the_flag)) {
      _print_chunk_error("Flag does not exist.", i, chunks);
      return make_error(ErrC::InvalidArg);
    }

    if(_already_passed(flag_index, chunks)) {
      _print_chunk_error("Flag was passed more than once.", flag_index, chunks);
      return make_error(ErrC::InvalidArg);
    }

    if(the_value.starts_with("=")) {
      the_value.erase(0, 1);
    }

    auto param_ptr =
    std::ranges::find_if(params_, [&](const Parameter& p) {
      return p.lf_ == the_flag || p.sf_ == the_flag;
    });

    ASSERT(param_ptr != params_.end());
    param_ptr->val_ = Value(the_value);
  }

  if(!_check_required_params()) {
    return make_error(ErrC::InvalidArg);
  }

  return make_result<None>();
}
