/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#include <Core/Fmt.hpp>
#include <Core/ArgParse.hpp>
#include <string>
#include <charconv>
BEGIN_NAMESPACE(n19::argp);

auto Value<int64_t>::convert(const sys::String& str) -> Result<void> {
  try {
    const auto val = std::stoll(str);     // Try conversion of "str".
    value_ = static_cast<int64_t>(val);   // store it.
  } catch(const std::invalid_argument&) { //
    return make_error(ErrC::Conversion);  // handle conversion error.
  } catch(const std::exception& e) {      // If it's some other exception, panic.
    PANIC(e.what());                      //
  }

  return make_result<void>();
}

auto Value<bool>::convert(const sys::String& str) -> Result<void> {
  if(str == _nstr("true") || str == _nstr("false")) {
    value_ = (str == _nstr("true"));
  } else {                                // String value must be true/false.
    return make_error(ErrC::Conversion);  // Conversion error.
  }                                       //

  return make_result<void>();
}

auto Value<double>::convert(const sys::String& str) -> Result<void> {
  try {
    const auto val = std::stod(str);      // Try conversion of "str".
    value_ = static_cast<double>(val);    // store it.
  } catch(const std::invalid_argument&) { //
    return make_error(ErrC::Conversion);  // handle conversion error.
  } catch(const std::exception& e) {      // If it's some other exception, panic.
    PANIC(e.what());                      //
  }

  return make_result<void>();
}

auto Value<std::string>::convert(const std::string& str) -> Result<void> {
  value_ = str;                           // Nothing to do for this one.
  return make_result<void>();             // just store the string and leave...
}

END_NAMESPACE(n19::argp);