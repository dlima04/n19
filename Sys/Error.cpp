/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Sys/Error.hpp>

#if defined(N19_WIN32)
#include <windows.h>
BEGIN_NAMESPACE(n19::sys);

NODISCARD_ auto last_error() -> String {
  DWORD err_code = GetLastError();
  DWORD result = 0, flags = 0;

  /// Error message formatting flags
  flags |= FORMAT_MESSAGE_ALLOCATE_BUFFER;
  flags |= FORMAT_MESSAGE_FROM_SYSTEM;
  flags |= FORMAT_MESSAGE_IGNORE_INSERTS;

  LPWSTR outbuf  = nullptr;
  if(!FormatMessageW(
    flags,
    nullptr,
    err_code,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    reinterpret_cast<LPWSTR>(&outbuf),
    0, nullptr
  )){
    return L"";
  }

  /// Copy the returned string and free the buffer
  std::wstring out(outbuf);
  LocalFree(outbuf);
  return out;
}

NODISCARD_ auto translate_native_error(ErrorCode err) -> String {
  DWORD flags = 0;
  flags |= FORMAT_MESSAGE_ALLOCATE_BUFFER;
  flags |= FORMAT_MESSAGE_FROM_SYSTEM;
  flags |= FORMAT_MESSAGE_IGNORE_INSERTS;

  LPWSTR outbuf  = nullptr;
  if(!FormatMessageW(
    flags,
    nullptr,
    err,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    reinterpret_cast<LPWSTR>(&outbuf),
    0, nullptr
  )){
    return L"";
  }

  /// Copy the returned string and free the buffer
  std::wstring out(outbuf);
  LocalFree(outbuf);
  return out;
}

END_NAMESPACE(n19::sys);
#else // POSIX

#include <string.h>
#include <errno.h>
BEGIN_NAMESPACE(n19::sys);

NODISCARD_ auto last_error() -> String {
  Char buffer[256] = { 0 };
  if(strerror_r(errno, buffer, sizeof(buffer) - 2) == 0) {
    return String{ buffer };
  }

  return _nstr("");
}

NODISCARD_ auto translate_native_error(ErrorCode err) -> String {
  Char buffer[256] = { 0 };
  if(strerror_r(err, buffer, sizeof(buffer) - 2) == 0) {
    return String{ buffer };
  }

  return _nstr("");
}

END_NAMESPACE(n19::sys);
#endif
