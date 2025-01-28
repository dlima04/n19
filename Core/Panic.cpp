/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#include <Core/Panic.hpp>
#include <Core/ConIO.hpp>
#include <Core/Fmt.hpp>
#include <cstdlib>
BEGIN_NAMESPACE(n19);

auto panic_impl_(
  const std::string &file,
  const int line,
  const std::string &msg ) -> void
{
  auto stream = OStream::from_stderr();
  stream
    << Con::RedFG
    << Con::Bold
    << "PANIC :: "
    << msg
    << Con::Reset
    << fmt("In file \"{}\" at line {}.", file, line)
    << Endl;
  ::exit(1); /// For now, just die.
}

auto fatal_impl_(const std::string &msg) -> void {
  auto stream = OStream::from_stderr();
  stream << Con::RedFG << Con::Bold;
  stream << "FATAL :: " << msg;
  ::exit(1);
}

END_NAMESPACE(n19);