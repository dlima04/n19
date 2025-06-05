/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#define CURRENT_TEST "/Users/Diago/Desktop/compiler_tests/test2.txt"

#include <Frontend/Lexer.hpp>
#include <Frontend/FrontendContext.hpp>
#include <Frontend/CompilationCycle.hpp>
#include <IO/Console.hpp>
#include <IO/Fmt.hpp>
#include <Core/ArgParse.hpp>
#include <cstdlib>
#include <utility>
#include <Core/Result.hpp>
#include <Core/Try.hpp>
#include <Core/StringUtil.hpp>
#include <Core/Defer.hpp>
#include <iostream>

#include "Frontend/EntityTable.hpp"
#include "System/BackTrace.hpp"

#define ARGNUM_HARD_LIMIT 40

using namespace n19;

struct MainArgParser : argp::Parser {
  argp::PackType& inputs  = arg<argp::PackType>(
    _nstr("--input"),
    _nstr("-i"),
    _nstr("Input file(s)."));

  argp::PackType& outputs = arg<argp::PackType>(
    _nstr("--output"),
    _nstr("-o"),
    _nstr("Output file(s)."));

  bool& verbose = arg<bool>(
    _nstr("--verbose"),
    _nstr("-v"),
    _nstr("Enable verbose output."));

  bool& dump_ast = arg<bool>(
    _nstr("--dump-ast"),
    _nstr("-dump-ast"),
    _nstr("Dump the program's AST."));

  bool& dump_toks = arg<bool>(
    _nstr("--dump-tokens"),
    _nstr("-dump-tokens"),
    _nstr("Dump the program's tokens only, do not compile it."));

  bool& dump_ents = arg<bool>(
    _nstr("--dump-entities"),
    _nstr("-dump-entities"),
    _nstr("Dump the entity table."));

  bool& dump_ir = arg<bool>(
    _nstr("--dump-ir"),
    _nstr("-dump-ir"),
    _nstr("Dump the program's lowered IR."));

  bool& show_help = arg<bool>(
    _nstr("--help"),
    _nstr("-h"),
    _nstr("Show this help message and exit."));

  bool& version = arg<bool>(
    _nstr("--version"),
    _nstr("-v"),
    _nstr("Display the n19 compiler version and exit."));
};

static auto verify_args(MainArgParser& parser) -> bool {
  auto stream = OStream::from_stdout();
  if (parser.show_help) {
    parser.help(stream);
    return false;
  }

  if (parser.version) {
    auto ver = Context::get_version_info();
    outs()
      << "n19 compiler -- version "
      << n19::fmt("{}.{}.{}\n", ver.major, ver.minor, ver.patch)
      << n19::fmt("Target: {} ({})\n", ver.arch, ver.os)
#ifdef __DATE__
      << "Compiled: " << __DATE__ << "\n";
#else
      ;
#endif
    return false;
  }

  if (parser.inputs.empty()) {
    outs() 
      << Con::RedFG 
      << "No input files provided." 
      << Con::Reset 
      << " Exiting..."
      << "\n";
    return false;
  }

  if (parser.outputs.empty()) {
    outs() 
      << Con::RedFG 
      << "No output files provided." 
      << Con::Reset
      << " Exiting..."
      << "\n";
    return false;
  }

  if (parser.inputs.size() != parser.outputs.size()) {
    outs()
      << Con::RedFG
      << "Error:"
      << Con::Reset
      << " Number of output files does not match"
      << " the number of inputs."
      << "\n";
    return false;
  }

  auto& context = Context::the();
  if (parser.dump_ast)  context.flags_ |= Context::DumpAST;
  if (parser.dump_ents) context.flags_ |= Context::DumpEnts;
  if (parser.dump_toks) context.flags_ |= Context::DumpToks;
  if (parser.dump_ir)   context.flags_ |= Context::DumpIR;
  if (parser.verbose)   context.flags_ |= Context::Verbose;

  Context::the().inputs_ = std::move(parser.inputs);
  Context::the().outputs_ = std::move(parser.outputs);

  return true;
}

#ifdef N19_WIN32
#include <System/Win32.hpp>

int main() {
  win32_init_console();
  outs() << Con::Reset;

  DEFER({
    ins().clear();
    outs().flush();
    errs().flush();
  });

  MainArgParser parser;
  LPWSTR cmdline = ::GetCommandLineW();

  int arg_count = 0;
  LPWSTR* args = ::CommandLineToArgvW(cmdline, &arg_count);
  if (args == nullptr) {
    outs()
      << "Could not retrieve win32 argv. Error code="
      << ::GetLastError()
      << "\n";
    return EXIT_FAILURE;
  }

  if (arg_count > ARGNUM_HARD_LIMIT) {
    outs() << "Too many command-line arguments passed.";
    outs() << "\n";
    ::LocalFree(args);
    return EXIT_FAILURE;
  }

  /// Initialize context
  auto stream = OStream::from_stdout();
  if (arg_count > 1 && args && !parser.take_argv(arg_count, args).parse(stream)) {
    ::LocalFree(args);
    return EXIT_FAILURE;
  }

  ::LocalFree(args);
  if (!verify_args(parser)) {
    return EXIT_FAILURE;
  }

  if (!begin_global_compilation_cycles()) {
    errs() << "Build failed.\n";
    return EXIT_FAILURE;
  }

  outs() << "Build complete.\n";
  return EXIT_SUCCESS;
}

#else /// POSIX
int main(int argc, char** argv){
  outs() << Con::Reset;
  DEFER({
    ins().clear();
    outs().flush();
    errs().flush();
  });

  if(argc > ARGNUM_HARD_LIMIT) {
    outs() << "Too many command-line arguments passed.";
    outs() << "\n";
    return EXIT_FAILURE;
  }

  MainArgParser parser;
  auto stream = OStream::from_stdout();
  if(argc > 1 && argv && !parser.take_argv(argc, argv).parse(stream)) {
    return EXIT_FAILURE;
  }

  if (!verify_args(parser)) {
    return EXIT_FAILURE;
  }

  if (!begin_global_compilation_cycles()) {
    errs() << "Build failed.\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

#endif
