/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Core/Common.hpp>
#include <Core/Platform.hpp>
#include <Core/ArgParse.hpp>
#include <Core/Console.hpp>
#include <Core/Defer.hpp>
#include <Core/Maybe.hpp>
#include <Core/Try.hpp>
#include <Core/Panic.hpp>
#include <Bulwark/Bulwark.hpp>
#include <Bulwark/BulwarkContext.hpp>
#include <System/SharedRegion.hpp>
#include <System/Process.hpp>
#include <System/String.hpp>
#include <cstdlib>
#include <utility>
#include <filesystem>
#include <random>

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

  argp::PackType& to_run = arg<argp::PackType>(
    _nstr("--run"),
    _nstr("-run"),
    _nstr("Run only these test suites (optional)"));
};

/// Note: since the arguments have been verified already,
/// just forward them to the child process.
static NOINLINE_ bool do_runall(const int argc, sys::Char** argv) {
  sys::String sr_name = _nstr("n19TestSharedMem");
  constexpr size_t sr_size = sizeof(test::TallyBox) + 20u;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<uint32_t> dist(1, 500000);
  const uint32_t rannum = dist(gen);

  sr_name += sys::to_native_string(rannum);

  auto sr = MUST(sys::SharedRegion::create(
    sr_name,
    sr_size,
    sr_size));

  DEFER({
    sr.destroy();
  });

  auto tally = static_cast<volatile test::TallyBox*>(sr.get());
  tally->total_suites_ran     = 0;
  tally->total_suites_skipped = 0;
  tally->total_cases_exc      = 0;
  tally->total_cases_failed   = 0;
  tally->total_cases_passed   = 0;
  tally->total_cases_skipped  = 0;
  tally->total_cases_ran      = 0;

  /// Note: we are assuming that these exist in the working directory
#ifdef N19_WIN32
  const sys::Char* subprocess_names[] = {
    _nstr(".\\testrunner-core.exe"),
    _nstr(".\\testrunner-rl.exe"),
    _nstr(".\\testrunner-system.exe")
  };
#else
  const sys::Char* subprocess_names[] = {
    _nstr("./testrunner-core"),
    _nstr("./testrunner-rl"),
    _nstr("./testrunner-system")
  };
#endif

  /// Construct command-line arguments
  std::vector<sys::String> cl_args;
  cl_args.reserve(18);

  for(int i = 1; i < argc; i++) {
    cl_args.emplace_back(argv[i]);
  }

  /// Specify shared memory region for subprocesses.
  cl_args.emplace_back(_nstr("--shared-region-name"));
  cl_args.emplace_back(sr_name);

  cl_args.emplace_back(_nstr("--shared-region-size"));
  cl_args.emplace_back(sys::to_native_string(sr_size));

  /// For now just run each subprocess individually.
  /// TODO: make this better
  for(const sys::Char* name : subprocess_names) {
    if(!std::filesystem::exists(name)) {
      outs()
        << Con::RedFG
        << "\nError: Test executable \""
        << name
        << "\" not found within the current working directory!\n\n"
        << Con::Reset;
      continue;
    }

    auto proc = sys::NaiveProcess(name)
      .args(cl_args)
      .errs_to(errs().dev())
      .output_to(outs().dev())
      .input_from(ins().dev())
      .launch();

    if(!proc.has_value()) {
      outs()
        << Con::RedFG
        << "Failed to spawn process "
        << name
        << ". Error=\""
        << proc.error().msg
        << "\"\n"
        << Con::Reset;
      continue;
    }

    auto exit_code = proc->wait();
    if(exit_code.type != sys::ExitCode::Type::Normal || exit_code.value != 0) {
      outs()
        << Con::YellowFG
        << "Warning: process "
        << name
        << " seems to have exited abnormally. Exit code: "
        << exit_code.value
        << "\n"
        << Con::Reset;
    }
  }

  const test::TallyType total = tally->total_cases_exc
    + tally->total_cases_failed
    + tally->total_cases_passed
    + tally->total_cases_skipped;

  outs()
    << "\nRan " << tally->total_suites_ran
    << " out of "
    << tally->total_suites_ran + tally->total_suites_skipped
    << " suites.\n";
  outs()
    << total
    << " cases total,\n  "
    << tally->total_cases_passed
    << " passed,\n  "
    << tally->total_cases_failed
    << " failed,\n  "
    << tally->total_cases_exc
    << " interrupted by exceptions,\n  "
    << tally->total_cases_skipped
    << " skipped.\n";

  return true;
}

#ifdef N19_WIN32
#include <System/Win32.hpp>

int main() {
  win32_init_console();

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

  DEFER({
    ins().clear();
    outs().flush();
    errs().flush();
    ::LocalFree(args);
  });

  if(arg_count > ARGNUM_HARD_LIMIT) {
    outs() << "Too many command-line arguments passed.";
    outs() << Endl;
    return EXIT_FAILURE;
  }

  /// Initialize context
  auto stream = OStream::from_stdout();
  if(arg_count > 1 && !parser.take_argv(arg_count, args).parse(stream)) {
    return EXIT_FAILURE;
  }

  if(parser.show_help) {
    parser.help(stream);
    return EXIT_SUCCESS;
  }

  detail_::allow_con_colours_ = parser.colours;
  outs() << Con::Reset;

  if(!do_runall(arg_count, args)) {
    return EXIT_FAILURE;
  }

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

  detail_::allow_con_colours_ = parser.colours;
  outs() << Con::Reset;

  if(!do_runall(argc, argv)) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

#endif
