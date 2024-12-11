/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#include <Sys/LastError.hpp>
BEGIN_NAMESPACE(n19::sys);

#if defined(N19_WIN32)
auto last_error() -> String {
  DWORD err_code = GetLastError();
  DWORD result   = 0;
  LPWSTR outbuf  = nullptr;

  if(!FormatMessageW(
    FORMAT_MESSAGE_ALLOCATE_BUFFER             // Specifies that a buffer should be allocated for the message.
     | FORMAT_MESSAGE_FROM_SYSTEM              // Indicates that the system message table should be searched.
     | FORMAT_MESSAGE_IGNORE_INSERTS,          // Indicates that insert sequences (i.e. "%1") should be ignored.
    nullptr,                                   // Optional, pointer to the message definition
    err_code,                                  // The error code to be formatted.
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
    reinterpret_cast<LPWSTR>(&outbuf),         // Function will allocate buffer at this address.
    0,                                         // Specifies size of the output buffer. Not needed in this case.
    nullptr                                    // Optional va_list for insert sequences.
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
  if(strerror_r(errno, buffer, sizeof(buffer)) == 0) {
    return String{ buffer };
  }

  return _nstr("");
}

#endif
END_NAMESPACE(n19::sys);