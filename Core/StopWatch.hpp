/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef STOPWATCH_HPP
#define STOPWATCH_HPP
#include <chrono>

namespace n19 {
  template<
    class T = double,
    class Units = std::milli,
    class Clock = std::chrono::high_resolution_clock>
  class StopWatch;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T, class Units, class Clock>
class n19::StopWatch {
public:
  using Duration      = std::chrono::duration<T, Units>;
  using TimePoint     = std::chrono::time_point<Clock, Duration>;
  using MilliSeconds  = std::milli;
  using NanoSeconds   = std::nano;

  auto elapsed() const -> T;
  auto restart() -> void;

  StopWatch() : start_t_(Clock::now()) {}
  ~StopWatch() = default;
private:
  TimePoint start_t_;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T, class Units, class Clock>
auto n19::StopWatch<T, Units, Clock>::restart() -> void {
  start_t_ = Clock::now();
}

template<class T, class Units, class Clock>
auto n19::StopWatch<T, Units, Clock>::elapsed() const -> T {
  auto end = Clock::now();
  auto dur = std::chrono::duration_cast<Duration>(end - start_t_);
  return dur.count();
}

#endif //STOPWATCH_HPP
