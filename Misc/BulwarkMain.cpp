/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Core/ArgParse.hpp>
#include <IO/Console.hpp>
#include <Bulwark/Bulwark.hpp>
#include <Bulwark/BulwarkContext.hpp>
#include <cstdlib>
#include <utility>
using namespace n19;

#define ARGNUM_HARD_LIMIT 40

struct BulwarkArgParser : argp::Parser {
  bool& verbose   = arg<bool>("--verbose", "-v", "Enable verbose output.");
  bool& stopfail  = arg<bool>("--stop-on-failure", "-stop-fail", "Stop execution on the first failed case.");
  bool& debug     = arg<bool>("--debug-messages", "-debug", "enabled debug output.");
  bool& colours   = arg<bool>("--colours", "-colours", "Enables ASCII colour output.", true);
  bool& show_help = arg<bool>("--help", "-h", "print this help message and exit.");
  argp::PackType& to_skip = arg<argp::PackType>("--skip", "-skip", "Test suites to be skipped (optional)");
  argp::PackType& to_run  = arg<argp::PackType>("--run", "-run", "Run only these test suites (optional)");
};

#ifdef N19_WIN32
int main() {
  win32_init_console();

  ins().clear();
  outs().flush();
  errs().flush();
  return EXIT_SUCCESS;
}

#else //POSIX

int main(int argc, char** argv) {
  if(argc > ARGNUM_HARD_LIMIT) {
    outs() << "\nToo many command-line arguments passed.";
    outs() << Endl;
    return EXIT_FAILURE;
  }

  BulwarkArgParser parser;
  auto stream = OStream::from_stdout();
  if(argc > 1 && argv && !parser.take_argv(argc, argv).parse(stream)) {
    return EXIT_FAILURE;
  }

  if(parser.show_help) {
    parser.help(stream);
    return EXIT_SUCCESS;
  }

  auto& ctx = test::Context::the();
  if(parser.verbose)  ctx.flags_ |= test::Context::Verbose;
  if(parser.stopfail) ctx.flags_ |= test::Context::StopFail;
  if(parser.debug)    ctx.flags_ |= test::Context::Debug;
  if(parser.colours)  ctx.flags_ |= test::Context::Colours;

  if(!parser.to_skip.empty()) {
    ctx.suites_to_skip_ = std::move(parser.to_skip);
  } if(!parser.to_run.empty()) {
    ctx.suites_to_run_  = std::move(parser.to_run);
  }

  test::g_registry.run_all();

  ins().clear();
  outs().flush();
  errs().flush();
  return EXIT_SUCCESS;
}

#endif
