/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Sys/LastError.hpp>
#if defined(N19_WIN32)
#  include <windows.h>
#else
#  include <string.h>
#  include <errno.h>
#endif

BEGIN_NAMESPACE(n19::sys);

#if defined(N19_WIN32)
auto last_error() -> String {
  DWORD err_code = GetLastError();             ///
  DWORD result   = 0;                          /// Unused.
  LPWSTR outbuf  = nullptr;                    /// Output buffer for the formatted string.

  if(!FormatMessageW(
    FORMAT_MESSAGE_ALLOCATE_BUFFER             /// Specifies that a buffer should be allocated for the message.
     | FORMAT_MESSAGE_FROM_SYSTEM              /// Indicates that the system message table should be searched.
     | FORMAT_MESSAGE_IGNORE_INSERTS,          /// Indicates that insert sequences (i.e. "%1") should be ignored.
    nullptr,                                   /// Optional, pointer to the message definition
    err_code,                                  /// The error code to be formatted.
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /// Default language
    reinterpret_cast<LPWSTR>(&outbuf),         /// Function will write a pointer to the buffer here.
    0,                                         /// Specifies size of the output buffer. Not needed in this case.
    nullptr                                    /// Optional va_list for insert sequences.
  )){
    return L"";                                /// On failure, return an empty string.
  }                                            ///

  std::wstring out(outbuf);
  LocalFree(outbuf);                           /// Free the buffer FormatMessageW allocated for us.
  return out;                                  ///
}

#else // POSIX
auto last_error() -> String {                  ///
  Char buffer[256] = { 0 };                    /// Static buffer of 256 bytes should be enough.
  if(strerror_r(errno, buffer, sizeof(buffer) - 2) == 0)
    return String{ buffer };
                                               ///
  return _nstr("");                            /// On failure, return an empty string.
}

#endif
END_NAMESPACE(n19::sys);