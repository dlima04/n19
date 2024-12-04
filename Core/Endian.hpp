/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef ENDIAN_HPP
#define ENDIAN_HPP
#include <bit>
#include <utility>

namespace n19 {
  using __Endian = std::underlying_type_t<std::endian>;
  enum class Endian : __Endian {
    Big     = static_cast<__Endian>(std::endian::big),
    Little  = static_cast<__Endian>(std::endian::little),
    Native  = static_cast<__Endian>(std::endian::native),
  };

  template<class T> constexpr auto swap_bytes_if(Endian e, T val) -> T;
  template<class T> constexpr auto swap_bytes(T val) -> T;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class T>
constexpr auto n19::swap_bytes(T val) -> T {
  static_assert(std::is_integral_v<T>);
  return std::byteswap( std::forward<T>(val) );
}

template<class T>
constexpr auto n19::swap_bytes_if(const Endian e, T val) -> T {
  static_assert(std::is_integral_v<T>);
  return e == Endian::Native ? std::byteswap(val) : val;
}

#endif //ENDIAN_HPP
