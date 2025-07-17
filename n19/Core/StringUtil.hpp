/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <n19/Core/Common.hpp>
#include <n19/Core/Maybe.hpp>
#include <n19/Core/Result.hpp>
#include <n19/System/String.hpp>
BEGIN_NAMESPACE(n19);

/// For strings that begin and end with ".
auto unescape_string(std::string_view)        -> Result<std::string>;
auto unescape_quoted_string(std::string_view) -> Result<std::string>;

/// For strings that begin and end with `.
auto unescape_raw_string(std::string_view)    -> Result<std::string>;
auto unescape_raw_quoted_string(std::string_view) -> Result<std::string>;

END_NAMESPACE(n19);
