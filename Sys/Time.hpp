/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef SYS_TIME_HPP
#define SYS_TIME_HPP
#include <Core/Result.hpp>
#include <Core/ClassTraits.hpp>
#include <string>

#ifdef N19_WIN32
#  include <windows.h>
#else // POSIX
#  include <time.h>
#endif

BEGIN_NAMESPACE(n19::sys);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Begin class definitions.

struct __STFormatter {
  const class SystemTime& time_;
  [[nodiscard]] auto format()  const -> std::string;
  [[nodiscard]] auto month()   const -> std::string;
  [[nodiscard]] auto weekday() const -> std::string;
  __STFormatter(const SystemTime& t) : time_(t) {}
};

class SystemTime {
N19_MAKE_DEFAULT_ASSIGNABLE(SystemTime);
N19_MAKE_DEFAULT_CONSTRUCTIBLE(SystemTime);
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

  auto strings() const     -> __STFormatter;
  static auto from_local() -> Result<SystemTime>;
  static auto from_utc()   -> Result<SystemTime>;

 ~SystemTime() = default;
  SystemTime() = default;
};

END_NAMESPACE(n19::sys);
#endif //SYS_TIME_HPP
