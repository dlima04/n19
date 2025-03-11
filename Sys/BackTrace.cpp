/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Sys/BackTrace.hpp>
#include <Core/Try.hpp>
#include <string_view>

#ifdef __has_include
#if __has_include(<cxxabi.h>)
#include <cxxabi.h>
#else
#define HAS_NO_CXXABI_H_
#endif
#else
#define HAS_NO_CXXABI_H_
#endif

#if defined(N19_WIN32)
#include <windows.h>
#include <DbgHelp.h>
BEGIN_NAMESPACE(n19::sys);

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
auto BackTrace::get() -> Result<void> {
  constexpr int maxframes = N19_BACKTRACE_MAX_FRAMES;
  void* f_[maxframes]{};

  const int res = backtrace(f_, maxframes);
  if(res == 0 || res > maxframes)
    return Error{ErrC::Internal, "Invalid number of returned frames"};

  char** syms = backtrace_symbols(f_, res); // TODO: demangle symbol names
  if(syms == nullptr)
    return Error{ErrC::Internal, "Failed to resolve symbol names"};

  for(int i = 0; i < res; i++)
    frames_[i] = BacktraceFrame{ .name_ = syms[i], .addr_ = f_[i] };

  free(syms);
  return Result<void>::create();
}

auto BackTrace::dump_to(OStream& stream) -> Result<void> {
  constexpr int maxframes = N19_BACKTRACE_MAX_FRAMES;
  void* f_[maxframes]{};

  const int res = backtrace(f_, maxframes);
  if(res == 0 || res > maxframes) {
    return Error{ErrC::Internal, "bad retrieved frame count."};
  }

  char** syms = backtrace_symbols(f_, res);
  if(syms == nullptr) return Error::from_native();

  for(int i = 0; i < res; i++)
    stream << "At " << syms[i] << "\n";

  stream << "\nTraced " << res << " frames,\n";
  stream << "Out of " << maxframes << " max." << Endl;
  free(syms);
  return Result<void>::create();
}

auto BackTrace::dump_to(File& file) -> Result<void> {
  constexpr int maxframes = N19_BACKTRACE_MAX_FRAMES;
  void* f_[maxframes]{};
  auto stream = OStream::from(file);

  const int res = backtrace(f_, maxframes);
  if(res == 0 || res > maxframes) return Error{ErrC::Internal};

  backtrace_symbols_fd(f_, res, file.value());
  return Result<void>::create();
}

END_NAMESPACE(n19::sys);
#endif

