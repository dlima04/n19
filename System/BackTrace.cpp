/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <System/BackTrace.hpp>
#include <Core/Try.hpp>
#include <string_view>

///
/// TODO: demangle symbol names,
/// or just use <backtrace> if it's available
/// for the target compiler.
#ifdef __has_include
#if __has_include(<cxxabi.h>)
#include <cxxabi.h>
#define N19_HAS_CXXABI_H_
#endif
#endif

#ifdef __has_include
#if __has_include(<backtrace>)
#include <backtrace>
#define N19_HAS_STD_BACKTRACE_H_
#endif
#endif

#if defined(N19_WIN32)
#include <System/Win32.hpp>
#include <DbgHelp.h>
BEGIN_NAMESPACE(n19::sys);

///
/// Not on Windows yet, would be too much work
/// for something this minor
auto BackTrace::get() -> Result<void> {
  return Error{ErrC::NotImplimented, "No backtraces on Windows yet."};
}

auto BackTrace::dump_to(OStream& stream) -> Result<void> {
  return Error{ErrC::NotImplimented, "No backtraces on Windows yet."};
}

auto BackTrace::dump_to(File& file) -> Result<void> {
  return Error{ErrC::NotImplimented, "No backtraces on Windows yet."};
}

END_NAMESPACE(n19::sys);
#else /// POSIX
#include <execinfo.h>
#include <stdlib.h>
BEGIN_NAMESPACE(n19::sys);

///
/// Retrieve the maximum amount of stack frames.
/// Store them inside of the backtrace object.
auto BackTrace::get() -> Result<void> {
  constexpr int maxframes = N19_BACKTRACE_MAX_FRAMES;
  void* f_[maxframes]{};

  const int res = ::backtrace(f_, maxframes);
  char** syms = ::backtrace_symbols(f_, res); // TODO: demangle symbol names
  if(syms != nullptr) {
    for(int i = 0; i < res; i++) {
      frames_[i] = BacktraceFrame{ .name_ = syms[i], .addr_ = f_[i] };
    }
    free(syms);
  }
  return Result<void>::create();
}

auto BackTrace::dump_to(OStream& stream) -> Result<void> {
  constexpr int maxframes = N19_BACKTRACE_MAX_FRAMES;
  void* f_[maxframes]{};

  const int res = ::backtrace(f_, maxframes);
  char** syms = ::backtrace_symbols(f_, res);
  if(syms != nullptr) {
    for(int i = 0; i < res; i++)
      stream << "At " << syms[i] << "\n";

    stream << "\nTraced " << res << " frames,\n";
    stream << "Out of " << maxframes << " max." << Endl;
    free(syms);
  }
  return Result<void>::create();
}

///
/// For dumping backtrace output to a file.
auto BackTrace::dump_to(File& file) -> Result<void> {
  constexpr int maxframes = N19_BACKTRACE_MAX_FRAMES;
  void* f_[maxframes]{};
  auto stream = OStream::from(file);

  const int res = ::backtrace(f_, maxframes);
  ::backtrace_symbols_fd(f_, res, file.value());
  return Result<void>::create();
}

END_NAMESPACE(n19::sys);
#endif

