/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <n19/System/Error.hpp>

#if defined(N19_WIN32)
BEGIN_NAMESPACE(n19::sys);

NODISCARD_ auto last_error() -> std::string {
  DWORD err_code = GetLastError();
  DWORD flags = 0;

  /// Error message formatting flags
  flags |= FORMAT_MESSAGE_ALLOCATE_BUFFER;
  flags |= FORMAT_MESSAGE_FROM_SYSTEM;
  flags |= FORMAT_MESSAGE_IGNORE_INSERTS;

  LPSTR outbuf  = nullptr;
  if(!::FormatMessageA(
    flags,
    nullptr,
    err_code,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    reinterpret_cast<LPSTR>(&outbuf),
    0, nullptr
  )){
    return "";
  }

  /// Copy the returned string and free the buffer
  std::string out(outbuf);
  LocalFree(outbuf);
  return out;
}

NODISCARD_ auto translate_native_error(ErrorCode err) -> std::string {
  DWORD flags = 0;
  flags |= FORMAT_MESSAGE_ALLOCATE_BUFFER;
  flags |= FORMAT_MESSAGE_FROM_SYSTEM;
  flags |= FORMAT_MESSAGE_IGNORE_INSERTS;

  LPSTR outbuf  = nullptr;
  if(!::FormatMessageA(
    flags,
    nullptr,
    err,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    reinterpret_cast<LPSTR>(&outbuf),
    0, nullptr
  )){
    return "";
  }

  /// Copy the returned string and free the buffer
  std::string out(outbuf);
  LocalFree(outbuf);
  return out;
}

END_NAMESPACE(n19::sys);
#else // POSIX

#include <string.h>
BEGIN_NAMESPACE(n19::sys);

NODISCARD_ auto last_error() -> String {
  Char buffer[256]{};
  if(::strerror_r(errno, buffer, sizeof(buffer) - 2) == 0) {
    return String{ buffer };
  }

  return _nstr("");
}

NODISCARD_ auto translate_native_error(ErrorCode err) -> String {
  Char buffer[256]{};
  if(::strerror_r(err, buffer, sizeof(buffer) - 2) == 0) {
    return String{ buffer };
  }

  return _nstr("");
}

END_NAMESPACE(n19::sys);
#endif
