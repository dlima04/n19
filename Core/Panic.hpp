/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef PANIC_HPP
#define PANIC_HPP
#include <string>
BEGIN_NAMESPACE(n19);

#define PANIC(MSG) ::n19::panic_impl_(__FILE__, __LINE__, MSG)
#define FATAL(MSG) ::n19::fatal_impl_(MSG)

#define UNREACHABLE PANIC("Default assertion - unreachable branch.")
#define ASSERT(COND, ...) if( !(COND)) PANIC("Assertion \"" #COND "\" failed! " __VA_ARGS__ )

[[noreturn]] auto fatal_impl_(const std::string &msg) -> void;
[[noreturn]] auto panic_impl_(const std::string &file, int line, const std::string &msg) -> void;

END_NAMESPACE(n19);
#endif //PANIC_HPP