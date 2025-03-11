/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Core/Panic.hpp>
#include <IO/Console.hpp>
#include <IO/Fmt.hpp>
#include <Sys/BackTrace.hpp>
#include <cstdlib>
#include <utility>
#include <mutex>

static std::mutex s_mtx_;
BEGIN_NAMESPACE(n19);

auto PanicHandler::add_callback(Callback&& callback) -> bool {
  std::lock_guard<std::mutex> lock(s_mtx_);
  if(index_ >= callbacks_.size()) return false;
  callbacks_[index_++] = std::move(callback);
  return true;
}

auto PanicHandler::get() -> PanicHandler& {
  static PanicHandler the_handler;
  return the_handler;
}

auto PanicHandler::fatal(Message &msg) -> void {
  s_mtx_.lock();
  auto stream = OStream::from_stdout();
  stream << Con::RedFG  << Con::Bold;
  stream << "FATAL :: " << msg << '\n';

  for(size_t i = 0; i < callbacks_.size() && i < index_; ++i) {
    callbacks_[i]( *this );
  }

  sys::BackTrace::dump_to(stream);
  stream.flush();
  ::exit(1); /// For now, just die. TODO: make better
}

auto PanicHandler::panic(Message &file, int line, Message &msg) -> void {
  s_mtx_.lock();
  auto stream = OStream::from_stdout();
  stream
    << Con::RedFG  << Con::Bold
    << "PANIC :: " << msg
    << Con::Reset  << fmt(" In file \"{}\" at line {}.", file, line)
    << '\n';

  for(size_t i = 0; i < callbacks_.size() && i < index_; ++i) {
    callbacks_[i]( *this );
  }

  sys::BackTrace::dump_to(stream);
  stream.flush();
  ::exit(1); /// For now, just die. TODO: make better
}

END_NAMESPACE(n19);