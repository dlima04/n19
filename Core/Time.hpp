/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef TIME_HPP
#define TIME_HPP
#include <Core/Platform.hpp>
#include <Core/Result.hpp>
#include <Core/ClassTraits.hpp>
#include <chrono>
#include <string>

#ifdef N19_WIN32
#  include <windows.h>
#else // POSIX
#  include <time.h>
#endif

BEGIN_NAMESPACE(n19::time);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Begin class definitions.

struct __LTFormatter {
  const class LocalTime& time_;
  [[nodiscard]] auto format()  const -> std::string;
  [[nodiscard]] auto month()   const -> std::string;
  [[nodiscard]] auto weekday() const -> std::string;
  __LTFormatter(const LocalTime& t) : time_(t) {}
};

class LocalTime {
N19_MAKE_DEFAULT_ASSIGNABLE(LocalTime);
N19_MAKE_DEFAULT_CONSTRUCTIBLE(LocalTime);
public:
#ifdef N19_WIN32
  using __SysRepr = ::SYSTEMTIME;
  using __Epoch   = ::time_t;
  using __Value   = ::WORD;
#else // POSIX
  using __SysRepr = struct ::tm;
  using __Epoch   = ::time_t;
  using __Value   = int;
#endif

  __Value second_  = 0;
  __Value minute_  = 0;
  __Value hour_    = 0;
  __Value day_     = 0;
  __Value weekday_ = 0;
  __Value month_   = 0;
  __Value year_    = 0;

  auto strings() const     -> __LTFormatter;
  static auto from_local() -> Result<LocalTime>;
  static auto from_utc()   -> Result<LocalTime>;

 ~LocalTime() = default;
  LocalTime() = default;
};

template<typename T, typename Units, typename Clock>
class __StopWatch final {
public:
  using Duration     = std::chrono::duration<T, Units>;
  using TimePoint    = std::chrono::time_point<Clock, Duration>;
  using MilliSeconds = std::milli;
  using NanoSeconds  = std::nano;

  auto elapsed() const -> T;
  auto restart() -> void;

  __StopWatch() : start_t_(Clock::now()) {}
 ~__StopWatch() = default;
private:
  TimePoint start_t_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Begin Type aliases.

using SystemWatch    = __StopWatch<double, std::milli, std::chrono::system_clock>;
using MonotonicWatch = __StopWatch<double, std::milli, std::chrono::steady_clock>;
using PreciseWatch   = __StopWatch<double, std::milli, std::chrono::high_resolution_clock>;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Begin inline / header functions.

template<typename T, typename Units, typename Clock>
auto __StopWatch<T, Units, Clock>::elapsed() const -> T {
  auto end = Clock::now();
  auto dur = std::chrono::duration_cast<Duration>(end - start_t_);
  return dur.count();
}

template<typename T, typename Units, typename Clock>
auto __StopWatch<T, Units, Clock>::restart() -> void {
  start_t_ = Clock::now();
}

END_NAMESPACE(n19::time);
#endif //TIME_HPP
