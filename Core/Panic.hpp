/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <Core/Platform.hpp>
#include <Core/ClassTraits.hpp>
#include <string>
#include <functional>
#include <array>
BEGIN_NAMESPACE(n19);

///
/// Assertion/panic macros
#ifndef N19_ASSERTIONS_OFF_
#   define PANIC(MSG) ::n19::PanicHandler::get().panic(__FILE__, __LINE__, MSG)
#   define FATAL(MSG) ::n19::PanicHandler::get().fatal(MSG)
#   define UNREACHABLE_ASSERTION PANIC("Default assertion - unreachable branch.")
#   define ASSERT(COND, ...) if( !(COND)) PANIC("Assertion \"" #COND "\" failed! " __VA_ARGS__ )
#else
#   define PANIC(MSG)
#   define FATAL(MSG)
#   define UNREACHABLE_ASSERTION
#   define ASSERT(COND, ...)
#endif

///
/// Panic handler singleton class
class PanicHandler {
  N19_MAKE_NONMOVABLE(PanicHandler);
  N19_MAKE_NONCOPYABLE(PanicHandler);
public:
  using Callback = std::function<void(PanicHandler&)>;
  using Message  = const std::string;

  NORETURN_ auto fatal(Message &msg) -> void;
  NORETURN_ auto panic(Message &file, int line, Message &msg) -> void;

  NODISCARD_ static auto get()  -> PanicHandler&;
  auto add_callback(Callback&&) -> bool;
private:
  PanicHandler() = default;
  size_t index_{};
  std::array<Callback, 24> callbacks_{};
};

END_NAMESPACE(n19);