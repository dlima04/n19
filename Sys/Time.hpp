/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef SYS_TIME_HPP
#define SYS_TIME_HPP
#include <Core/Platform.hpp>
#include <Core/Result.hpp>
#include <Core/ClassTraits.hpp>
#include <string>

#ifdef N19_WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else // POSIX
#include <time.h>
#endif

BEGIN_NAMESPACE(n19::sys);

struct STFormatter_ {
  const class SystemTime& time_;
  NODISCARD_ auto format()  const -> std::string;
  NODISCARD_ auto month()   const -> std::string;
  NODISCARD_ auto weekday() const -> std::string;
  STFormatter_(const SystemTime& t) : time_(t) {}
};

class SystemTime {
N19_MAKE_DEFAULT_ASSIGNABLE(SystemTime);
N19_MAKE_DEFAULT_CONSTRUCTIBLE(SystemTime);
public:
#ifdef N19_WIN32
  using SysRepr_ = ::SYSTEMTIME;
  using Epoch_   = ::time_t;
  using Value_   = ::WORD;
#else // POSIX
  using SysRepr_ = struct ::tm;
  using Epoch_   = ::time_t;
  using Value_   = int;
#endif

  Value_ second_  = 0;
  Value_ minute_  = 0;
  Value_ hour_    = 0;
  Value_ day_     = 0;
  Value_ weekday_ = 0;
  Value_ month_   = 0;
  Value_ year_    = 0;

  auto strings() const     -> STFormatter_;
  static auto from_local() -> Result<SystemTime>;
  static auto from_utc()   -> Result<SystemTime>;

 ~SystemTime() = default;
  SystemTime() = default;
};

END_NAMESPACE(n19::sys);
#endif //SYS_TIME_HPP
