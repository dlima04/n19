/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#include <print>
#include <iostream>
#include <cstdlib>
#include <csignal>
#include <cstdint>
#include <Frontend/ShitLexer.hpp>
#include <Native/String.hpp>
#include <Native/LastError.hpp>
#include <Core/Panic.hpp>
#include <Core/ArgParse.hpp>
#include <Core/ResultMacros.hpp>
#include <Core/Bytes.hpp>
#include <Frontend/ErrorCollector.hpp>
#include "Frontend/AstNodes.hpp"

/* THINGS THAT NEED IMPROVEMENT!
  1. The lexer. In every aspect.
  2. Token representation. We shouldn't use string_view.
  3.

*/

#define CURRENT_TEST "/Users/Diago/Desktop/compiler_tests/test2.txt"
using namespace n19;

static auto handle_kb_interrupt(const int signal) -> void {
  std::println("\nKeyboard Interrupt! Exiting...");
  ::std::exit(signal);
}

#if defined(N19_WIN32)
#include <windows.h>
#include <shellapi.h>

int main() {
  // Windows-specific command line parsing.
  // We need to get passed arguments as UTF16 encoded
  // strings. The most reliable way to do this is to use
  // GetCommandLineW() alongside CommandLineToArgvW().
  // It's also possible to define main as int wmain() instead,
  // but from what I can tell this method is more portable.

  std::vector<native::String> args;
  int argc = 0;
  wchar_t** argv = nullptr;

  argv = ::CommandLineToArgvW(GetCommandLineW(), &argc);
  if(argv == nullptr) {
    native::outs() << native::last_error() << _nchr('\n');
    PANIC("Failed to retrieve command line arguments.");
  }

  for(int i = 1; i < argc; i++) {
    args.emplace_back(argv[i]);
  }

  LocalFree(argv);
  argv = nullptr;

#else // IF POSIX
int main(int argc, char** argv) {
  // For POSIX platforms, this shit is light work.
  // Simply parse out command line arguments in the
  // same way you'd normally do it: argc and argv.
  // These strings should be UTF8 encoded in most cases.
  std::vector<native::String> args;
  for(int i = 1; i < argc; i++) {
    args.emplace_back(argv[i]);
  }

#endif // #IF defined(N19_WIN32)
  //
  // std::vector<native::StringView> strs
  // = { "--output-directory", "--demangle-funcs=true", "-a", "3123", "-z" };
  //
  // argp::Parser parser;
  // parser
  //   .add_param(argp::Parameter::create("--output-directory", "-f", "The foo argument" ))
  //   .add_param(argp::Parameter::create("--demangle-funcs", "-b", "The bar argument", false, argp::Value{"thedefault"}))
  //   .add_param(argp::Parameter::create("--time-actions", "-a", "The asshole argument", true))
  //   .add_param(argp::Parameter::create("--jobs", "-z", "idk lol", true));
  //
  // if(!parser.parse(strs)) {
  //    return 1;
  // }
  //
  // parser.debug_print();
  // parser.print();
  //
  // const auto val = parser.get_arg("--time-actions");
  // if(val) {
  //   std::println("Value: \"{}\"", val->value_);
  // }

  try {
    const auto file = MUST(FileRef::open(CURRENT_TEST));
    auto lxr = Lexer::create(*file);
    if(!lxr) {
      return 1;
    }

    do {
      lxr->advance(1);
      std::cout << lxr->current().format() << std::flush;
    } while(lxr->current() != TokenType::EndOfFile && lxr->current() != TokenType::Illegal);

    if(lxr->current() == TokenType::Illegal) {
      lxr->error("Illegal token!");
    }

  } catch(const std::exception& e) {
    std::cerr << "EXCEPTION: " << e.what() << std::endl;
  }

  return 0;
}


