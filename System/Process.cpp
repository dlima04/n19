/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <System/Process.hpp>
#include <Core/Fmt.hpp>
#include <Core/Panic.hpp>
#include <Core/Defer.hpp>
#include <algorithm>

#ifdef N19_POSIX
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#endif

BEGIN_NAMESPACE(n19::sys);
#ifdef N19_WIN32

auto NaiveProcess_::launch(
  std::vector<String>& args,
  String& pathname,
  String& working_dir,
  Maybe<IODevice> output_dev,
  Maybe<IODevice> error_dev,
  Maybe<IODevice> input_dev ) -> Result<NaiveProcess_>
{
  if(pathname.empty()) {
    return Error(ErrC::InvalidArg, "Empty pathname");
  }

  /// Disallow empty arguments.
  for(size_t i = 0; i < args.size(); ++i) {
    if (args[i].empty()) {
      return Error(
        ErrC::InvalidArg, fmt("Empty argument at position {}.", i));
    }
  }

  auto to_backslashes = [](const Char ch) -> Char {
    return ch == L'/' ? L'\\' : ch;
  };

  if(!working_dir.empty())
    std::ranges::transform(working_dir, working_dir.begin(), to_backslashes);

  std::ranges::transform(pathname, pathname.begin(), to_backslashes);

  String full_buff = pathname;
  for(const auto& arg : args) {
    full_buff += _nstr(" ");
    full_buff += arg;
  }

  ::PROCESS_INFORMATION pi{};
  ::STARTUPINFOW si{};
  si.cb = sizeof(::STARTUPINFOW);

  ::SECURITY_ATTRIBUTES nul_sa{};
  nul_sa.nLength = sizeof(::SECURITY_ATTRIBUTES);
  nul_sa.lpSecurityDescriptor = nullptr;
  nul_sa.bInheritHandle = TRUE;

  // Open handle to NUL device.
  /// NOTE: this is basically the equivalent of /dev/null on Windows.
  ::HANDLE h_nul = ::CreateFileA("NUL",
    GENERIC_READ | GENERIC_WRITE,
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    &nul_sa,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    nullptr);

  DEFER({
    if (h_nul != INVALID_HANDLE_VALUE && h_nul != nullptr) {
      ::CloseHandle(h_nul);
    }
  });

  /// Redirect -- stdout
  if(output_dev.has_value() && !output_dev->is_invalid()) {
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdOutput = output_dev->value();
  } else if (h_nul != INVALID_HANDLE_VALUE) {
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdOutput = h_nul;
  }

  /// Redirect -- stderr
  if(error_dev.has_value() && !error_dev->is_invalid()) {
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdError = error_dev->value();
  } else if (h_nul != INVALID_HANDLE_VALUE) {
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdError = h_nul;
  }

  /// Redirect -- stdin
  if(input_dev.has_value() && !input_dev->is_invalid()) {
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdInput = input_dev->value();
  } else if (h_nul != INVALID_HANDLE_VALUE) {
    si.dwFlags |= STARTF_USESTDHANDLES;
    si.hStdInput = h_nul;
  }

  full_buff += static_cast<Char>(0);

  if(!::CreateProcessW(
    nullptr,
    full_buff.data(),
    nullptr,
    nullptr,
    TRUE,
    0,
    nullptr,
    working_dir.empty() ? nullptr : working_dir.c_str(),
    &si,
    &pi
  )) {
    return Error::from_native();
  }

  ::CloseHandle(pi.hThread);
  return NaiveProcess_(pi.hProcess);
}

NaiveProcess_::NaiveProcess_(ValueType vt) {
  this->value_ = vt;
}

auto NaiveProcess_::close() -> void {
  if (is_invalid()) return;
  if (get_exit_code().type == ExitCode::Type::NotExited) {
    ::TerminateProcess(value_, 0);
  }

  ::CloseHandle(value_);
  invalidate();
}

NaiveProcess_::NaiveProcess_(NaiveProcess_&& other) noexcept {
  this->value_ = other.value_;
  other.invalidate();
}

NaiveProcess_& NaiveProcess_::operator=(NaiveProcess_&& other) noexcept {
  this->value_ = other.value_;
  other.invalidate();
  return *this;
}

auto NaiveProcess_::get_id() -> size_t {
  return (size_t)(::GetProcessId(value_));
}

auto NaiveProcess_::exited() -> bool {
  return get_exit_code().type == ExitCode::Type::NotExited;
}

auto NaiveProcess_::get_exit_code() -> ExitCode {
  ::DWORD codeval = 0;
  if(!::GetExitCodeProcess(value_, &codeval)) {
    return {0, ExitCode::Type::Unknown};
  }
  if(codeval == STILL_ACTIVE) {
    return {codeval, ExitCode::Type::NotExited};
  }

  return {codeval, ExitCode::Type::Normal};
}

auto NaiveProcess_::wait() -> ExitCode {
  if(::WaitForSingleObject(value_, INFINITE) == WAIT_FAILED) {
    return {0, ExitCode::Type::Unknown};
  }

  const auto the_code = get_exit_code();
  ::CloseHandle(value_);
  invalidate();
  return the_code;
}

auto NaiveProcess_::invalidate() -> void {
  value_ = (::HANDLE)nullptr;
}

auto NaiveProcess_::is_invalid() -> bool {
  return value_ == (::HANDLE)nullptr;
}

#else // POSIX

auto NaiveProcess_::launch(
  std::vector<String>& args,
  String& pathname,
  String& working_dir,
  Maybe<IODevice> output_dev,
  Maybe<IODevice> error_dev,
  Maybe<IODevice> input_dev ) -> Result<NaiveProcess_>
{
  if(pathname.empty()) {
    return Error(ErrC::InvalidArg, "Empty pathname");
  }

  const pid_t pid = ::fork();
  if(pid < 0) {
    return Error::from_native();
  }

  if(pid > 0) {
    return NaiveProcess_(pid);
  }

  /// Change working directory
  if(!working_dir.empty()) {
    ::chdir(working_dir.c_str());
  }

  int fd_null = ::open("/dev/null", O_RDWR);

  /// redirect -- stdout
  if(output_dev.has_value() && !output_dev->is_invalid()) {
    ::dup2(output_dev->value(), STDOUT_FILENO);
  } else if(fd_null != -1) {
    ::dup2(fd_null, STDOUT_FILENO);
  }

  /// redirect -- stderr
  if(error_dev.has_value() && !error_dev->is_invalid()) {
    ::dup2(error_dev->value(), STDERR_FILENO);
  } else if(fd_null != -1) {
    ::dup2(fd_null, STDERR_FILENO);
  }

  /// redirect -- stdin
  if(input_dev.has_value() && !input_dev->is_invalid()) {
    ::dup2(input_dev->value(), STDIN_FILENO);
  } else if(fd_null != -1) {
    ::dup2(fd_null, STDIN_FILENO);
  }

  if(fd_null != -1) {
    ::close(fd_null);
  }

  pathname += static_cast<Char>(0);

  /// I hate that I have to do this
  std::vector<Char*> argv_ptrs;
  argv_ptrs.reserve(args.size() + 2);
  argv_ptrs.emplace_back(pathname.data());

  for(size_t i = 0; i < args.size(); i++) {
    args[i] += static_cast<Char>(0);
    argv_ptrs.emplace_back(args[i].data());
  }

  argv_ptrs.emplace_back(nullptr);
  ::execvp(pathname.c_str(), argv_ptrs.data());
  ::_exit(1);
}

NaiveProcess_::NaiveProcess_(NaiveProcess_&& other)
 noexcept : posix_cached_exit_code_(other.posix_cached_exit_code_)
{
  this->value_ = other.value_;
  other.invalidate();
}

NaiveProcess_& NaiveProcess_::operator=(NaiveProcess_&& other) noexcept {
  this->value_ = other.value_;
  this->posix_cached_exit_code_ = other.posix_cached_exit_code_;
  other.posix_cached_exit_code_ = ExitCode(-1, ExitCode::Type::Unknown);
  other.invalidate();
  return *this;
}

NaiveProcess_::NaiveProcess_(ValueType vt)
: posix_cached_exit_code_(-1, ExitCode::Type::Unknown)
{
  this->value_ = vt;
}

auto NaiveProcess_::close() -> void {
  if(is_invalid()) return;
  ::kill(value_, SIGTERM);
  invalidate();
}

auto NaiveProcess_::invalidate() -> void {
  value_ = -1;
}

auto NaiveProcess_::is_invalid() -> bool {
  return value_ == -1;
}

auto NaiveProcess_::posix_blocking_wait_impl_() -> ExitCode {
  int status = 0;
  const auto pid = ::waitpid(value_, &status, 0);

  /// Once waiting is complete, get the exit code.
  if(pid < 0) {
    return {-1, ExitCode::Type::Unknown};
  }
  if(pid == 0) {
    return {0, ExitCode::Type::NotExited};
  }

  if(pid == value_) {
    if(WIFEXITED(status)) {
      return {WEXITSTATUS(status), ExitCode::Type::Normal};
    }
    if(WIFSIGNALED(status)) {
      return {WTERMSIG(status), ExitCode::Type::FromSignal};
    }
    if(WIFSTOPPED(status)) {
      return {WSTOPSIG(status), ExitCode::Type::FromSignal};
    }
    return {-1, ExitCode::Type::Unknown};
  }

  UNREACHABLE_ASSERTION;
}

auto NaiveProcess_::posix_nohang_wait_impl_() -> ExitCode {
  int status         = 0;
  const pid_t result = ::waitpid(value_, &status, WNOHANG);

  if(result < 0) {
    return { -1, ExitCode::Type::Unknown };  // an error has occurred.
  }
  if(result == 0) {
    return { 0, ExitCode::Type::NotExited }; // The process is still running.
  }

  if(result == value_) {
    if(WIFEXITED(status)) {
      return {WEXITSTATUS(status), ExitCode::Type::Normal};
    }
    if(WIFSIGNALED(status)) {
      return {WTERMSIG(status), ExitCode::Type::FromSignal};
    }
    if(WIFSTOPPED(status)) {
      return {WSTOPSIG(status), ExitCode::Type::FromSignal};
    }
    return {-1, ExitCode::Type::Unknown};
  }

  UNREACHABLE_ASSERTION;
}

auto NaiveProcess_::wait() -> ExitCode {
  if(posix_cached_exit_code_.type == ExitCode::Type::Unknown) {
    posix_cached_exit_code_ = posix_blocking_wait_impl_();
  }
  return posix_cached_exit_code_;
}

auto NaiveProcess_::exited() -> bool {
  if(posix_cached_exit_code_.type != ExitCode::Type::Unknown) {
    return true;
  }

  const auto code = posix_nohang_wait_impl_();
  if(posix_cached_exit_code_.type == ExitCode::Type::Unknown
      && code.type != ExitCode::Type::NotExited
  ) {
    posix_cached_exit_code_ = code;
  }

  return code.type != ExitCode::Type::NotExited;
}

auto NaiveProcess_::get_exit_code() -> ExitCode {
  if(posix_cached_exit_code_.type != ExitCode::Type::Unknown) {
    return posix_cached_exit_code_;
  }

  const auto code = posix_nohang_wait_impl_();
  if(posix_cached_exit_code_.type == ExitCode::Type::Unknown
      && code.type != ExitCode::Type::NotExited
  ) {
    posix_cached_exit_code_ = code;
  }

  return code;
}

auto NaiveProcess_::get_id() -> size_t {
  return static_cast<size_t>(value_);
}

#endif //N19_WIN32

auto NaiveProcess_::Builder::launch() -> Result<NaiveProcess_> {
  Maybe<IODevice> out_to = Nothing;
  Maybe<IODevice> input_from = Nothing;
  Maybe<IODevice> errs_to = Nothing;

  if(!output_to_.is_invalid()) {
    out_to = output_to_;
  }
  if(!errs_to_.is_invalid()) {
    errs_to = errs_to_;
  }
  if(!input_from_.is_invalid()) {
    input_from = input_from_;
  }

  return NaiveProcess_::launch(
    args_,
    name_,
    working_directory_,
    out_to,
    errs_to,
    input_from);
}

END_NAMESPACE(n19::sys);
