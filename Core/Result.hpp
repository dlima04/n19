/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef RESULT_HPP
#define RESULT_HPP
#include <expected>
#include <optional>
#include <cstdint>
#include <format>
#include <string>

namespace n19 {
  enum class ErrC : uint8_t {
    None,       // Default value, no error occured.
    InvalidArg, // An argument passed is incorrect.
    FileIO,     // A file I/O operation has failed.
    Internal,   // Bad internal logic has caused a failure.
    NotFound,   // The thing you're looking for is not here.
    BadToken,   // An incorrect token was emmitted by the lexer.
    Native,     // A native, underlying OS error has occurred.
  };

  struct ErrorDescriptor {
    std::string msg;
    ErrC code = ErrC::None;
  };

  struct None {
    unsigned char _ = 0;
    None() = default;
  };

  template<typename T>
  using Result = std::expected<T, ErrorDescriptor>;
  using Error = std::unexpected<ErrorDescriptor>;

  // For consistency.
  template<typename T>
  using Maybe = std::optional<T>;

  template<typename T, typename ...Args>
  auto make_result(Args&&... args) -> Result<T>;

  template<typename ...Args>
  auto make_error(ErrC code, std::format_string<Args...>, Args... args) -> Error;
  auto make_error(ErrC code) -> Error;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T, typename ...Args>
auto n19::make_result(Args&&... args) -> Result<T> {
  return Result<T>{ T(std::forward<Args>(args)...) };
}

template<typename... Args>
auto n19::make_error(const ErrC code, std::format_string<Args...> fmt, Args... args) -> Error {
  std::string formatted;
  ErrorDescriptor desc;
  try {
    formatted = std::vformat(fmt.get(), std::make_format_args(args...));
  } catch(const std::format_error& e) {
    formatted = std::string("!! std::vformat: ") + e.what();
  } catch(...) {
    formatted = "!! format error";
  }

  desc.code = code;
  desc.msg  = formatted;
  return Error{ desc };
}

inline auto n19::make_error(const ErrC code) -> Error {
  const ErrorDescriptor desc{ .msg = "", .code = code };
  return Error{ desc };
}

#endif //RESULT_HPP
