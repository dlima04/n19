/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#include <Core/Time.hpp>
#include <Core/Fmt.hpp>
#include <Sys/LastError.hpp>
BEGIN_NAMESPACE(n19::time);

auto __STFormatter::weekday() const -> std::string {
  switch(time_.weekday_) {
    case 7  : [[fallthrough]];
    case 0  : return "Sun";
    case 1  : return "Mon";
    case 2  : return "Tue";
    case 3  : return "Wed";
    case 4  : return "Thu";
    case 5  : return "Fri";
    case 6  : return "Sat";
    default : return "???";
  }

  UNREACHABLE;
}

auto __STFormatter::month() const -> std::string {
  #ifndef N19_WIN32
    const auto val = time_.month_ + 1;
  #else   // WINDOWS
    const auto val = time_.month_;
  #endif

  switch(val) {
    case 1  : return "Jan";
    case 2  : return "Feb";
    case 3  : return "Mar";
    case 4  : return "Apr";
    case 5  : return "May";
    case 6  : return "Jun";
    case 7  : return "Jul";
    case 8  : return "Aug";
    case 9  : return "Sep";
    case 10 : return "Oct";
    case 11 : return "Nov";
    case 12 : return "Dec";
    default : return "???";
  }

  UNREACHABLE;
}

auto __STFormatter::format() const -> std::string {
  return fmt("{}, {} {} {} - {}:{}:{}",
    this->weekday(),   /// Format weekday to human readable.
    this->month(),     /// Format month to human readable.
    time_.day_,        /// day - unchanged
    time_.year_,       /// year - unchanged
    time_.hour_ + 1,   /// offset hour by one to make readable
    time_.minute_,     /// minute - unchanged
    time_.second_);    /// second - unchanged
}

auto SystemTime::strings() const -> __STFormatter {
  return __STFormatter{ *this };
}

#ifdef N19_WIN32
auto LocalTime::from_utc() -> Result<LocalTime> {
  LocalTime time = {}; /// To return.
  __SysRepr repr = {}; /// Windows SYSTEMTIME.

  ::GetSystemTime(&repr);
  time.second_  = repr.wSecond;
  time.minute_  = repr.wMinute;
  time.hour_    = repr.wHour;
  time.day_     = repr.wDay;
  time.weekday_ = repr.wDayOfWeek;
  time.month_   = repr.wMonth;
  time.year_    = repr.wYear;
  return time;
}

auto LocalTime::from_local() -> Result<LocalTime> {
  LocalTime time = {}; /// To return.
  __SysRepr repr = {}; /// Windows SYSTEMTIME.

  ::GetLocalTime(&repr);
  time.second_  = repr.wSecond;
  time.minute_  = repr.wMinute;
  time.hour_    = repr.wHour;
  time.day_     = repr.wDay;
  time.weekday_ = repr.wDayOfWeek;
  time.month_   = repr.wMonth;
  time.year_    = repr.wYear;
  return time;
}

#else // POSIX
auto SystemTime::from_utc() -> Result<SystemTime> {
  SystemTime time = {}; /// To return.
  __Epoch epoch  = {}; /// UNIX epoch.
  __SysRepr repr = {}; /// POSIX time struct.

  ::time(&epoch);
  if(::gmtime_r(&epoch, &repr) == nullptr) {
    return make_error(ErrC::Native, sys::last_error());
  }

  time.second_  = repr.tm_sec;
  time.minute_  = repr.tm_min;
  time.hour_    = repr.tm_hour;
  time.day_     = repr.tm_mday;
  time.weekday_ = repr.tm_wday;
  time.month_   = repr.tm_mon;
  time.year_    = repr.tm_year + 1900;
  return time;
}

auto SystemTime::from_local() -> Result<SystemTime> {
  SystemTime time = {}; /// To return.
  __Epoch epoch  = {}; /// UNIX epoch.
  __SysRepr repr = {}; /// POSIX time struct.

  ::time(&epoch);
  if(::localtime_r(&epoch, &repr) == nullptr) {
    return make_error(ErrC::Native, sys::last_error());
  }

  time.second_  = repr.tm_sec;
  time.minute_  = repr.tm_min;
  time.hour_    = repr.tm_hour;
  time.day_     = repr.tm_mday;
  time.weekday_ = repr.tm_wday;
  time.month_   = repr.tm_mon;
  time.year_    = repr.tm_year + 1900;
  return time;
}

#endif // N19_WIN32
END_NAMESPACE(n19::time);