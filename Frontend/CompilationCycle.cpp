/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Frontend/CompilationCycle.hpp>
#include <Frontend/FrontendContext.hpp>
#include <Frontend/Parser.hpp>
#include <IO/Console.hpp>
#include <Core/Panic.hpp>
#include <Core/Defer.hpp>
BEGIN_NAMESPACE(n19);

bool begin_global_compilation_cycles() {
  [[maybe_unused]] auto& inputs  = Context::the().inputs_;
  [[maybe_unused]] auto& outputs = Context::the().outputs_;
  
  ASSERT(inputs.size() == outputs.size());
  ASSERT(!outputs.empty() && !inputs.empty());
  
  /// Note: only handle a single input/output file for now.
  /// Ignore any additional passed ones. Parallel compilation
  /// is an undecided issue.

  [[maybe_unused]] InputFile& in  = inputs[0];
  [[maybe_unused]] OutputFile& out = outputs[0];

  ASSERT(in.kind == InputFileKind::CoreUnit);
  in.state = InputFileState::Finished;

  auto ref = sys::File::open(in.name, false, sys::File::Read);
  if (!ref.has_value()) {
    errs()
      << Con::RedFG
      << "Error:"
      << Con::Reset
      << " Could not open input file "
      << in.name
      << ".\n"
      << ref.error().msg
      << "\n";
    return false;
  }

  DEFER_IF(!ref->is_invalid(), {
    ref->close();
  });

  auto lxr = Lexer::create_shared(*ref);
  if (!lxr) {
    errs()
      << Con::RedFG
      << "Error:"
      << Con::Reset
      << " Could not open input file "
      << in.name
      << ".\n"
      << lxr.error().msg
      << "\n";
    return false;
  }

  if(Context::the().flags_ & Context::DumpCtx) {
    outs()
      << Con::Bold
      << "---- Frontend Context\n"
      << Con::Reset;
    Context::the().dump(outs());
  }

  if(Context::the().flags_ & Context::DumpToks) {
    (*lxr)->dump(outs());
    return true;
  }

  ErrorCollector errors;
#ifdef N19_WIN32
  EntityTable tbl(std::filesystem::absolute(ref->path()).wstring());
#else
  EntityTable tbl(std::filesystem::absolute(ref->path()).string());
#endif

  ParseContext ctx(in.id, errs(), errors, *(*lxr), tbl);

  if (!parse(ctx)) 
    return false;

  if ((Context::the().flags_ & Context::DumpAST) && !ctx.toplevel_decls_.empty()) {
    outs()
      << Con::Bold
      << "---- Abstract Syntax Tree\n"
      << Con::Reset;
    for (const auto& decl : ctx.toplevel_decls_) {
      decl->print(0, outs(), Nothing);
    }
    outs() << "\n";
  }
  
  if (Context::the().flags_ & Context::DumpEnts) {
    outs()
      << Con::Bold
      << "---- Pre Check Phase Entity Table\n"
      << Con::Reset;
    ctx.entities.dump(outs());
    ctx.entities.dump_structures(outs());
  }

  /// TODO: once the rest of the compiler is finished, 
  /// handle other compilation tasks like checking, codegen, 
  /// etc here.

  return true;
}

END_NAMESPACE(n19);
