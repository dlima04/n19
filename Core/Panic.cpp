/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Core/Panic.hpp>
#include <IO/Console.hpp>
#include <IO/Fmt.hpp>
#include <cstdlib>
#include <utility>
BEGIN_NAMESPACE(n19);

auto PanicHandler::add_callback(Callback&& callback) -> bool {
  if(index_ >= callbacks_.size()) return false;
  callbacks_[index_++] = std::move(callback);
  return true;
}

auto PanicHandler::get() -> PanicHandler& {
  static PanicHandler the_handler;
  return the_handler;
}

auto PanicHandler::fatal(Message &msg) -> void {
  auto stream = OStream::from_stdout();
  stream << Con::RedFG  << Con::Bold;
  stream << "FATAL :: " << msg << Endl;

  for(size_t i = 0; i < callbacks_.size() && i < index_; ++i) {
    callbacks_[i]( *this );
  }

  ::exit(1);
}

auto PanicHandler::panic(Message &file, int line, Message &msg) -> void {
  auto stream = OStream::from_stdout();
  stream
    << Con::RedFG  << Con::Bold
    << "PANIC :: " << msg
    << Con::Reset  << fmt(" In file \"{}\" at line {}.", file, line)
    << Endl;

  for(size_t i = 0; i < callbacks_.size() && i < index_; ++i) {
    callbacks_[i]( *this );
  }

  ::exit(1); /// For now, just die.
}

END_NAMESPACE(n19);