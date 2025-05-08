/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Frontend/CompilationCycle.hpp>
#include <Frontend/FrontendContext.hpp>
#include <IO/Console.hpp>
BEGIN_NAMESPACE(n19);

// Begin compilation cycles for all source
// files provided as input. Note: we assume that the
// global context object has been correctly initialized
// at this point.
auto begin_global_compilation_cycles() -> void {
  outs() << "bla." << Endl;
}

END_NAMESPACE(n19);