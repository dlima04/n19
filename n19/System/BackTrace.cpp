/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <n19/System/BackTrace.hpp>
#include <n19/Core/Try.hpp>
#include <n19/Core/MacroUtil.hpp>
#include <string_view>

#  ifdef __has_include
#  if __has_include(<cxxabi.h>)
#include <cxxabi.h>
#define N19_HAS_CXXABI_H_
#  endif
#  endif //__has_include

#  if defined(N19_WIN32)
#include <n19/System/Win32.hpp>
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
#  else /// POSIX

#include <execinfo.h>
#include <stdlib.h>
#include <dlfcn.h>
BEGIN_NAMESPACE(n19::sys);

#  ifdef N19_HAS_CXXABI_H_

///
/// Retrieve the maximum amount of stack frames.
/// Store them inside of the backtrace object.
auto BackTrace::get() -> Result<void> {
  void *trace[N19_BACKTRACE_MAX_FRAMES]{};
  ::Dl_info dlinfo{};

  int status = 0;
  const char* symname = nullptr;
  char* demangled = nullptr;

  int trace_size = ::backtrace(trace, N19_BACKTRACE_MAX_FRAMES);
  for(int i = 0; i < trace_size; i++) {
    if(!::dladdr(trace[i], &dlinfo))
      continue;

    symname = dlinfo.dli_sname;
    demangled = abi::__cxa_demangle(symname, nullptr, nullptr, &status);

    if(status == 0 && demangled) {
      symname = demangled;
    } else {
      return Error(ErrC::Internal, "abi::__cxa_demangle");
    }

    frames_[i] = BacktraceFrame{ .name_ = demangled, .addr_ = trace[i] };

    if (demangled)
      ::free(demangled);
  }

  return Result<void>::create();
}

auto BackTrace::dump_to(OStream& stream) -> Result<void> {
  void *trace[N19_BACKTRACE_MAX_FRAMES]{};
  ::Dl_info dlinfo{};

  int status = 0;
  const char* symname = nullptr;
  char* demangled = nullptr;

  int trace_size = ::backtrace(trace, N19_BACKTRACE_MAX_FRAMES);
  for(int i = 0; i < trace_size; i++) {
    if(!::dladdr(trace[i], &dlinfo))
      continue;

    symname = dlinfo.dli_sname;
    demangled = abi::__cxa_demangle(symname, nullptr, nullptr, &status);

    if(status == 0 && demangled) {
      symname = demangled;
    } else {
      return Error(ErrC::Internal, "abi::__cxa_demangle");
    }

    stream << "At " << trace[i] << " " << symname << "\n";

    if (demangled)
      ::free(demangled);
  }

  stream
    << "\nTraced "
    << trace_size
    << " frames out of "
    << N19_BACKTRACE_MAX_FRAMES
    << " max.\n";

  return Result<void>::create();
}

///
/// For dumping backtrace output to a file.
auto BackTrace::dump_to(File& file) -> Result<void> {
  using namespace std::string_view_literals;

  void *trace[N19_BACKTRACE_MAX_FRAMES]{};
  ::Dl_info dlinfo{};

  int status = 0;
  const char* symname = nullptr;
  char* demangled = nullptr;

  auto stream = OStream::from(file.dev());

  int trace_size = ::backtrace(trace, N19_BACKTRACE_MAX_FRAMES);
  for(int i = 0; i < trace_size; i++) {
    if(!::dladdr(trace[i], &dlinfo))
      continue;

    symname = dlinfo.dli_sname;
    demangled = abi::__cxa_demangle(symname, nullptr, nullptr, &status);

    if(status == 0 && demangled) {
      symname = demangled;
    } else {
      if(demangled)
        ::free(demangled);
      continue;
    }

    /// Need to do it this way otherwise a null terminator
    /// is written to the file
    stream
      << "At "
      << trace[i]
      << " "
      << symname
      << "\n";

    if (demangled)
      ::free(demangled);
  }

  /// Again, very stupid but necessary
  stream
    << "\nTraced "
    << trace_size
    << " frames out of "
    << N19_BACKTRACE_MAX_FRAMES
    << " max.\n";
  return Result<void>::create();
}

#  else //NO CXXABI HEADER:

auto BackTrace::get() -> Result<void> {
  constexpr int maxframes = N19_BACKTRACE_MAX_FRAMES;
  void* f_[maxframes]{};

  const int res = ::backtrace(f_, maxframes);
  char** syms = ::backtrace_symbols(f_, res);
  if(syms != nullptr) {
    for(int i = 0; i < res; i++) {
      frames_[i] = BacktraceFrame{ .name_ = syms[i], .addr_ = f_[i] };
    }
    ::free(syms);
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
    ::free(syms);
  }
  return Result<void>::create();
}

auto BackTrace::dump_to(File& file) -> Result<void> {
  constexpr int maxframes = N19_BACKTRACE_MAX_FRAMES;
  void* f_[maxframes]{};
  auto stream = OStream::from(file);

  const int res = ::backtrace(f_, maxframes);
  ::backtrace_symbols_fd(f_, res, file.value());
  return Result<void>::create();
}

#  endif //N19_HAS_CXXABI_H_

END_NAMESPACE(n19::sys);
#  endif

