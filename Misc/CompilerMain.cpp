/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#define CURRENT_TEST "/Users/Diago/Desktop/compiler_tests/test2.txt"

#include <Frontend/Lexer.hpp>
#include <Frontend/FrontendContext.hpp>
#include <IO/Console.hpp>
#include <IO/Fmt.hpp>
#include <Core/ArgParse.hpp>
#include <cstdlib>
#include <utility>
#include <Core/Result.hpp>
#include <Core/Try.hpp>
#include <Core/StringUtil.hpp>
#include <iostream>

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

#ifdef N19_WIN32
int main() {
  win32_init_console();

  ins().clear();
  outs().flush();
  errs().flush();
  return EXIT_SUCCESS;
}

#else /// POSIX
int main(int argc, char** argv){
  if(argc > ARGNUM_HARD_LIMIT) {
    outs() << "\nToo many command-line arguments passed.";
    outs() << Endl;
    return EXIT_FAILURE;
  }

  MainArgParser parser;
  auto stream = OStream::from_stdout();
  if(argc > 1 && argv && !parser.take_argv(argc, argv).parse(stream)) {
    return EXIT_FAILURE;
  }

  if(parser.show_help) {
    parser.help(stream);
    return EXIT_SUCCESS;
  }

  if(parser.version) {
    auto ver = Context::get_version_info();
    outs()
      << "n19 compiler -- version "
      << n19::fmt("{}.{}.{}\n", ver.major, ver.minor, ver.patch)
      << n19::fmt("Target: {} ({})\n", ver.arch, ver.os)
      << ver.msg
      << Endl;
    return EXIT_SUCCESS;
  }

  if(parser.inputs.empty()) {
    outs() << "\nNo input files provided. Exiting..." << Endl;
    return EXIT_FAILURE;
  }

  if(parser.outputs.empty()) {
    outs() << "\nNo output files provided. Exiting..." << Endl;
    return EXIT_FAILURE;
  }

  auto& context = Context::the();
  if(parser.dump_ast)  context.flags_ |= Context::DumpAST;
  if(parser.dump_ents) context.flags_ |= Context::DumpEnts;
  if(parser.dump_ir)   context.flags_ |= Context::DumpIR;
  if(parser.verbose)   context.flags_ |= Context::Verbose;

  Context::the().inputs_  = std::move(parser.inputs);
  Context::the().outputs_ = std::move(parser.outputs);

  ins().clear();
  outs().flush();
  errs().flush();
  return EXIT_SUCCESS;
}

#endif