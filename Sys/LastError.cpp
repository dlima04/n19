/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Sys/LastError.hpp>

#if defined(N19_WIN32)
#   include <windows.h>
#else
#   include <string.h>
#   include <errno.h>
#endif

BEGIN_NAMESPACE(n19::sys);
#if defined(N19_WIN32)

auto last_error() -> String {
  DWORD err_code = GetLastError();  ///
  DWORD result   = 0;               /// Unused.
  LPWSTR outbuf  = nullptr;         /// Output buffer for the formatted string.

  if(!FormatMessageW(
    FORMAT_MESSAGE_ALLOCATE_BUFFER
     | FORMAT_MESSAGE_FROM_SYSTEM
     | FORMAT_MESSAGE_IGNORE_INSERTS,
    nullptr,
    err_code,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    reinterpret_cast<LPWSTR>(&outbuf),
    0,
    nullptr
  )){
    return L"";
  }

  std::wstring out(outbuf);
  LocalFree(outbuf);
  return out;
}

#else // POSIX

auto last_error() -> String {
  Char buffer[256] = { 0 };
  if(strerror_r(errno, buffer, sizeof(buffer) - 2) == 0) {
    return String{ buffer };
  }

  return _nstr("");
}

#endif
END_NAMESPACE(n19::sys);