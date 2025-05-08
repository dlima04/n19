/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Frontend/FrontendContext.hpp>
BEGIN_NAMESPACE(n19);

auto Context::get_version_info() -> VersionInfo {
  VersionInfo ver;
  ver.major = 0;
  ver.minor = 0;
  ver.patch = 0;
  ver.msg   = "The n19'th song sings of life.";

  /// TODO: get accurate architectural
  /// and system info.

  return ver;
}

END_NAMESPACE(n19);
