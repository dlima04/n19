/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Sys/BackTrace.hpp>
#include <Core/Try.hpp>
#include <string_view>

#if defined(N19_WIN32)
#include <windows.h>
#include <DbgHelp.h>
BEGIN_NAMESPACE(n19::sys);



END_NAMESPACE(n19::sys);

#else /// POSIX
#include <execinfo.h>
#include <stdlib.h>
BEGIN_NAMESPACE(n19::sys);

auto BackTrace::get() -> Result<void> {
  constexpr int maxframes = N19_BACKTRACE_MAX_FRAMES;
  void* f_[maxframes]{};

  const int res = backtrace(f_, maxframes);
  if(res == 0 || res > maxframes) {
    return Error{ErrC::Internal}; /// Shouldn't happen tbh.
  }

  char** syms = backtrace_symbols(f_, res);
  if(syms == nullptr) {
    return Error{ErrC::Internal}; /// again, just a sanity check
  }

  for(int i = 0; i < res; i++) {
    frames_[i].addr_ = f_[i];
    frames_[i].name_ = syms[i];
  }

  free(syms);
  return Result<void>::create();
}

auto BackTrace::dump_to(OStream& stream) -> Result<void> {
  constexpr int maxframes = N19_BACKTRACE_MAX_FRAMES;
  void* f_[maxframes]{};

  const int res = backtrace(f_, maxframes);
  if(res == 0 || res > maxframes) return Error{ErrC::Internal};

  char** syms = backtrace_symbols(f_, res);
  if(syms == nullptr) return Error{ErrC::Internal};

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

  char** syms = backtrace_symbols(f_, res);
  if(syms == nullptr) return Error{ErrC::Internal};

  for(int i = 0; i < res; i++) {
    const std::string_view sym = syms[i];
    stream << "At " << sym << "\n";
  }

  stream << "\nTraced " << res << " frames,\n";
  stream << "Out of " << maxframes << " max." << Endl;
  free(syms);

  return Result<void>::create();
}

END_NAMESPACE(n19::sys);
#endif

