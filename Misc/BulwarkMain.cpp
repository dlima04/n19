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
  bool& verbose = arg<bool>(
    _nstr("--verbose"),
    _nstr("-v"),
    _nstr("Enable verbose output."));

  bool& stopfail = arg<bool>(
    _nstr("--stop-on-failure"),
    _nstr("-stop-fail"),
    _nstr("Stop execution on the first failed case."));

  bool& debug = arg<bool>(
    _nstr("--debug-messages"),
    _nstr("-debug"),
    _nstr("enabled debug output."));

  bool& colours = arg<bool>(
    _nstr("--colours"),
    _nstr("-colours"),
    _nstr("Enables ASCII colour output."), true);

  bool& show_help = arg<bool>(
    _nstr("--help"),
    _nstr("-h"),
    _nstr("print this help message and exit."));

  argp::PackType& to_skip = arg<argp::PackType>(
    _nstr("--skip"),
    _nstr("-skip"),
    _nstr("Test suites to be skipped (optional)"));

  argp::PackType& to_run  = arg<argp::PackType>(
    _nstr("--run"),
    _nstr("-run"),
    _nstr("Run only these test suites (optional)"));
};

#ifdef N19_WIN32
#include <System/Win32.hpp>

int main() {
  win32_init_console();
  outs() << Con::Reset;

  BulwarkArgParser parser;
  LPWSTR cmdline = ::GetCommandLineW();

  int arg_count = 0;
  LPWSTR* args = ::CommandLineToArgvW(cmdline, &arg_count);
  if (args == nullptr) {
    outs()
      << "Could not retrieve win32 argv. Error code="
      << ::GetLastError()
      << Endl;
    return EXIT_FAILURE;
  }

  if (arg_count > ARGNUM_HARD_LIMIT) {
    outs() << "Too many command-line arguments passed.";
    outs() << Endl;
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
  if(parser.show_help) {
    parser.help(stream);
    return EXIT_SUCCESS;
  }

  auto& ctx = test::Context::the();
  if (parser.verbose)  ctx.flags_ |= test::Context::Verbose;
  if (parser.stopfail) ctx.flags_ |= test::Context::StopFail;
  if (parser.debug)    ctx.flags_ |= test::Context::Debug;
  if (parser.colours)  ctx.flags_ |= test::Context::Colours;

  if (!parser.to_skip.empty()) {
    ctx.suites_to_skip_ = std::move(parser.to_skip);
  } if (!parser.to_run.empty()) {
    ctx.suites_to_run_ = std::move(parser.to_run);
  }

  test::g_registry.run_all();

  ins().clear();
  outs().flush();
  errs().flush();
  return EXIT_SUCCESS;
}

#else //POSIX

int main(int argc, char** argv) {
  outs() << Con::Reset;
  if(argc > ARGNUM_HARD_LIMIT) {
    outs() << "Too many command-line arguments passed.";
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
  }
  if(!parser.to_run.empty()) {
    ctx.suites_to_run_  = std::move(parser.to_run);
  }

  detail_::allow_con_colours_ = parser.colours;
  test::g_registry.run_all();

  ins().clear();
  outs().flush();
  errs().flush();
  return EXIT_SUCCESS;
}

#endif
