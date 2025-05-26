/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Frontend/FrontendContext.hpp>

#ifdef N19_WIN32
#include <System/Win32.hpp>
#else
#include <sys/utsname.h>
#endif

BEGIN_NAMESPACE(n19);

auto Context::get_version_info() -> VersionInfo {
  VersionInfo ver;
  ver.major = 0;
  ver.minor = 0;
  ver.patch = 0;

#ifdef N19_WIN32
  ::SYSTEM_INFO sysinfo{};
  ::GetNativeSystemInfo(&sysinfo);

  switch(sysinfo.wProcessorArchitecture) {
  case PROCESSOR_ARCHITECTURE_AMD64:
    ver.arch = "x64";
    break;
  case PROCESSOR_ARCHITECTURE_ARM:
    ver.arch = "ARM";
    break;
  case PROCESSOR_ARCHITECTURE_ARM64:
    ver.arch = "ARM64";
    break;
  case PROCESSOR_ARCHITECTURE_INTEL:
    ver.arch = "legacy x86";
    break;
  default:
    ver.arch = "<unknown architecture>";
    break;
  }

#else // POSIX
  struct ::utsname buff{};
  constexpr size_t i
    = (sizeof(buff.machine) / sizeof(buff.machine[0])) - 1;

  if(::uname(&buff) == 0) {
    buff.machine[ i ] = '\0';
    ver.arch = buff.machine;
  }
#endif

#if defined(N19_WIN32)
  ver.os = "microsoft-windows";
#elif defined(N19_DARWIN)
  ver.os = "apple-darwin";
#elif defined(N19_LINUX)
  ver.os = "linux";
#else
  ver.os = "<unknown operating system>";
#endif

  return ver;
}

END_NAMESPACE(n19);
