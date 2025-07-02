/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Core/Common.hpp>
#include <Core/Platform.hpp>
#include <Core/ArgParse.hpp>
#include <Core/Console.hpp>
#include <Core/Defer.hpp>
#include <Core/Try.hpp>
#include <Core/Panic.hpp>
#include <Bulwark/Bulwark.hpp>
#include <Bulwark/BulwarkContext.hpp>
#include <System/SharedRegion.hpp>
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

  sys::String& shared_region_name = arg<sys::String>(
    _nstr("--shared-region-name"),
    _nstr("-shared-region-name"),
    _nstr("Shared memory region name"));

  uint64_t& shared_region_size = arg<uint64_t>(
    _nstr("--shared-region-size"),
    _nstr("-shared-region-size"),
    _nstr("Shared memory region size"));

  argp::PackType& to_skip = arg<argp::PackType>(
    _nstr("--skip"),
    _nstr("-skip"),
    _nstr("Test suites to be skipped (optional)"));

  argp::PackType& to_run  = arg<argp::PackType>(
    _nstr("--run"),
    _nstr("-run"),
    _nstr("Run only these test suites (optional)"));
};

static NOINLINE_ bool verify_args(BulwarkArgParser& parser) {
  auto& ctx = test::Context::the();
  if(parser.verbose)  ctx.flags_ |= test::Context::Verbose;
  if(parser.stopfail) ctx.flags_ |= test::Context::StopFail;
  if(parser.debug)    ctx.flags_ |= test::Context::Debug;
  if(parser.colours)  ctx.flags_ |= test::Context::Colours;

  detail_::allow_con_colours_ = parser.colours;
  outs() << Con::Reset;

  if(!parser.to_skip.empty()) {
    ctx.suites_to_skip_ = std::move(parser.to_skip);
  } if(!parser.to_run.empty()) {
    ctx.suites_to_run_ = std::move(parser.to_run);
  } if(!parser.shared_region_name.empty() && parser.shared_region_size > 0) {
    ASSERT(parser.shared_region_size >= sizeof(test::TallyBox));
    ctx.shared_region_ = MUST(sys::SharedRegion::open(parser.shared_region_name, parser.shared_region_size));
  }

  return true;
}

#ifdef N19_WIN32
#include <System/Win32.hpp>

int main() {
  win32_init_console();
  DEFER({
    ins().clear();
    outs().flush();
    errs().flush();
  });

  BulwarkArgParser parser;
  ::LPWSTR cmdline = ::GetCommandLineW();

  int arg_count = 0;
  ::LPWSTR* args = ::CommandLineToArgvW(cmdline, &arg_count);
  if(args == nullptr) {
    outs()
      << "Could not retrieve win32 argv. Error code="
      << ::GetLastError()
      << Endl;
    return EXIT_FAILURE;
  }

  if(arg_count > ARGNUM_HARD_LIMIT) {
    outs() << "Too many command-line arguments passed.";
    outs() << Endl;
    ::LocalFree(args);
    return EXIT_FAILURE;
  }
  
  /// Initialize context
  auto stream = OStream::from_stdout();
  if(arg_count > 1 && !parser.take_argv(arg_count, args).parse(stream)) {
    ::LocalFree(args);
    return EXIT_FAILURE;
  }

  ::LocalFree(args);
  if(parser.show_help) {
    parser.help(stream);
    return EXIT_SUCCESS;
  }

  if(!verify_args(parser)) {
    return EXIT_FAILURE;
  }

  test::g_registry.run_all();
  return EXIT_SUCCESS;
}

#else //POSIX

int main(int argc, char** argv) {
  DEFER({
    ins().clear();
    outs().flush();
    errs().flush();
  });

  if(argc > ARGNUM_HARD_LIMIT) {
    outs() << "Too many command-line arguments passed.\n";
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

  if(!verify_args(parser)) {
    return EXIT_FAILURE;
  }

  test::g_registry.run_all();
  return EXIT_SUCCESS;
}

#endif
