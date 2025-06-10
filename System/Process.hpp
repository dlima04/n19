/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef N19_SYS_PROCESS_HPP
#define N19_SYS_PROCESS_HPP
#include <Core/ClassTraits.hpp>
#include <System/Win32.hpp>
#include <Core/Platform.hpp>
#include <Core/Maybe.hpp>
#include <Core/Result.hpp>
#include <System/String.hpp>
#include <IO/Console.hpp>
#include <cstdint>
#include <vector>
#include <utility>
BEGIN_NAMESPACE(n19::sys);

/* n19::sys::ExitCode
 *
 * An exit code for a given process. Returned
 * when a process is finished.
 */
struct ExitCode {
#ifdef N19_WIN32
  using Value = ::DWORD;
#else
  using Value = int;
#endif

  enum class Type : uint8_t {
    NotExited,
    Normal,
    FromSignal,
    Unknown
  };

  Value value;
  Type type;

  ExitCode(const Value v, const Type t) : value(v), type(t) {}
  ~ExitCode() = default;
};

#ifdef N19_WIN32
using ProcessHandleType_ = Handle<::HANDLE>;
#else
using ProcessHandleType_ = Handle<::pid_t>;
#endif

/* n19::sys::NaiveProcess_
 *
 * The following class is "naive" in the sense that it does not
 * consider many granular process-related features that
 * are available on most operating systems.
 *
 * Specifically:
 * - It does not concern itself with permissions or security.
 * - It does not allow for the grouping of processes into workgroups.
 * - It does not allow for manipulation of the process' address space.
 *
 * This class should be used when you "just need to run something" and
 * aren't particularly concerned with the details.
 */
class NaiveProcess_ : protected ProcessHandleType_ {
  N19_MAKE_NONCOPYABLE(NaiveProcess_);
public:
  NaiveProcess_(NaiveProcess_&&) noexcept;
  NaiveProcess_& operator=(NaiveProcess_&&) noexcept;

  // inherited from Handle
  auto close()      -> void override;
  auto invalidate() -> void override;
  auto is_invalid() -> bool override;

  NODISCARD_ auto wait()       -> ExitCode;
  NODISCARD_ auto exited()     -> bool;
  NODISCARD_ auto get_exit_code() -> ExitCode;
  NODISCARD_ auto get_id()     -> size_t;

  class Builder;
  friend class Builder;

private:
#ifdef N19_POSIX
  auto posix_blocking_wait_impl_() -> ExitCode;
  auto posix_nohang_wait_impl_() -> ExitCode;
#endif

  NaiveProcess_(ValueType vt);
  static auto launch(
    std::vector<String>& args,
    String& pathname,
    const String& working_dir,
    Maybe<IODevice> output_dev,
    Maybe<IODevice> error_dev,
    Maybe<IODevice> input_dev ) -> Result<NaiveProcess_>;

protected:
#ifdef N19_POSIX
  ExitCode posix_cached_exit_code_;
#endif
};

/* n19::sys::NaiveProcess_::Builder
 *
 * This helper class facilitates the launching of NaiveProcesses,
 * working directories, command line arguments, and more can
 * be specified using NaiveProcess_::Builder.
 */
class NaiveProcess_::Builder {
public:
  template<typename ...Args>
  FORCEINLINE_ auto args(Args&&... args) -> Builder& {
    args_ = decltype(args_){ args... };
    return *this;
  }

  template<typename T>
  FORCEINLINE_ auto working_directory(T&& str) -> Builder& {
    working_directory_ = str;
    return *this;
  }

  FORCEINLINE_ auto output_to(const IODevice& dev) -> Builder& {
    output_to_ = dev;
    return *this;
  }

  FORCEINLINE_ auto input_from(const IODevice& dev) -> Builder& {
    input_from_ = dev;
    return *this;
  }

  FORCEINLINE_ auto errs_to(const IODevice& dev) -> Builder& {
    errs_to_ = dev;
    return *this;
  }

  auto launch() -> Result<NaiveProcess_>;
  Builder(const String& name) : name_(name) {}

 ~Builder() = default;
private:
  std::vector<String> args_;
  String name_;
  String working_directory_;
  IODevice output_to_;
  IODevice errs_to_;
  IODevice input_from_;
};

FORCEINLINE_ NaiveProcess_::Builder NaiveProcess(const String& str) {
  return NaiveProcess_::Builder(str);
}

END_NAMESPACE(n19::sys);
#endif //N19_SYS_PROCESS_HPP
