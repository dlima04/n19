/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <n19/Core/StringUtil.hpp>
#include <n19/Core/Panic.hpp>
#include <n19/Core/Platform.hpp>
#include <cstdint>
#include <sstream>
#include <cctype>
BEGIN_NAMESPACE(n19);

static auto map_escape_(const char ch) -> Result<char> {
  switch(ch) {
    case 'n':  return '\n';
    case 't':  return '\t';
    case 'r':  return '\r';
    case 'b':  return '\b';
    case 'f':  return '\f';
    case 'a':  return '\a';
    case 'v':  return '\v';
    case '\\': return '\\';
    case '\'': return '\'';
    case '\"': return '\"';
    default:   return Error(
      ErrC::Conversion, "Invalid escape sequence.");
  }
}

auto unescape_string(std::string_view input) -> Result<std::string> {
  std::ostringstream result;
  size_t i = 0;
  const size_t len = input.length();

  while (i < len) {

    /// UTF-8 sequence
    /// skip this (carefully).
    if(uint8_t raw = static_cast<uint8_t>(input[i]); raw >= 0x80) {
      const size_t utf8_begin = i;
      bool invalid            = false;

      if      ((raw & 0xE0) == 0xC0) i += 2;
      else if ((raw & 0xF0) == 0xE0) i += 3;
      else if ((raw & 0xF8) == 0xF0) i += 4;
      else invalid = true;

      if(invalid || i - 1 >= input.size()) {
        return Error(ErrC::Conversion, "Invalid UTF-8 sequence.");
      }

      result << input.substr(utf8_begin, i - utf8_begin);
      continue;
    }

    /// Non-escaped character.
    if (!(input[i] == '\\' && i + 1 < len)) {
      result << input[i++];
      continue;
    }

    /// Regular escape sequence.
    /// Map the escaped character to its actual value, append it.
    const char next = input[i + 1];
    if (auto real_escape = map_escape_(next); real_escape) {
      result << real_escape.value();
      i += 2;
    }

    /// Hexadecimal escape sequence (\xHH)
    /// This is pretty bad. There must be a cleaner way of doing this...
    else if (next == 'x') {
      i += 2;
      int val = 0;
      int digits = 0;
      while (i < len && digits < 2) {
        if(!std::isxdigit(static_cast<uint8_t>(input[i]))) {
          return Error(ErrC::Conversion, "Invalid hex escape. Bad digit.");
        }

        val *= 16;
        if (input[i] >= '0' && input[i] <= '9')      val += input[i] - '0';
        else if (input[i] >= 'a' && input[i] <= 'f') val += input[i] - 'a' + 10;
        else if (input[i] >= 'A' && input[i] <= 'F') val += input[i] - 'A' + 10;
        ++i;
        ++digits;
      }
      result << static_cast<char>(val);
    }

    /// Octal escape sequence \nnn (up to 3 digits)
    else if (next >= '0' && next <= '7') {
      int val = 0;
      int digits = 0;
      ++i;
      while (i < len && digits < 3 && input[i] >= '0' && input[i] <= '7') {
        val = val * 8 + (input[i] - '0');
        ++i;
        ++digits;
      }
      result << static_cast<char>(val);
    }

    /// Unknown escape. We need to error on this.
    else
      return Error(ErrC::Conversion, "Invalid escape sequence.");
  }

  return result.str();
}

auto unescape_raw_string(std::string_view input) -> Result<std::string> {
  std::ostringstream result;
  size_t i = 0;
  const size_t len = input.length();

  while (i < len) {
    if(uint8_t raw = static_cast<uint8_t>(input[i]); raw >= 0x80) {
      const size_t utf8_begin = i;
      bool invalid            = false;

      if      ((raw & 0xE0) == 0xC0) i += 2;
      else if ((raw & 0xF0) == 0xE0) i += 3;
      else if ((raw & 0xF8) == 0xF0) i += 4;
      else invalid = true;

      if(invalid || i - 1 >= input.size()) {
        return Error(ErrC::Conversion, "Invalid UTF-8 sequence.");
      }

      result << input.substr(utf8_begin, i - utf8_begin);
      continue;
    }

    if( input[i] == '\\' && i + 1 < len && input[i + 1] == '`' ) {
      result << '`';
      i += 2;
      continue;
    }

    result << input[i++];
  }

  return result.str();
}

auto unescape_quoted_string(std::string_view input) -> Result<std::string> {
  auto actual = unescape_string(input);
  if (actual.has_value() && actual->size() >= 2) {
    actual->erase(0, 1);
    actual->pop_back();
  } else {
    return Error(ErrC::InvalidArg, "String is not correctly quoted.");
  }

  return actual;
}

auto unescape_raw_quoted_string(std::string_view input) -> Result<std::string> {
  auto actual = unescape_raw_string(input);
  if (actual.has_value() && actual->size() >= 2) {
    actual->erase(0, 1);
    actual->pop_back();
  } else {
    return Error(ErrC::InvalidArg, "Raw string is not correctly quoted.");
  }

  return actual;
}

END_NAMESPACE(n19);
