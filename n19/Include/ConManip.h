#ifndef CONMANIP_H
#define CONMANIP_H
#include <cstdint>

namespace n19 {
  enum class ConFg : uint16_t {
    None   = 0,
    Green  = 32,
    Yellow = 33,
    Blue   = 34,
    Cyan   = 36,
    Red    = 91,
  };

  enum class ConStyle : uint16_t {
    Bold      = 1,
    Underline = 4,
  };

#if defined(N19_WIN32)
  auto are_vsequences_enabled()  -> bool;
  auto enable_vsequences()       -> void;
  auto maybe_enable_vsequences() -> void;
#endif
  auto set_console(ConFg fg)     -> void;
  auto set_console(ConStyle cs)  -> void;
  auto reset_console()           -> void;
}

#endif //CONMANIP_H
