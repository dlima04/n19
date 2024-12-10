/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/


#define CURRENT_TEST "/Users/Diago/Desktop/compiler_tests/test2.txt"

#include <Core/NewResult.hpp>
#include <Core/Try.hpp>
#include <Core/Callback.hpp>
#include <print>

using namespace n19;

N19_NOINLINE Result<int> foo(const int x) {
  if(x >= 10) {
    return make_error(ErrC::InvalidArg, "fuck you");
  }

  return make_result<int>(x + 20);
}

N19_NOINLINE Result<int> bar(const int z) {
  auto val = TRY(foo(z));
  std::println("bar made it past!");
  std::println("Int={}", *val);
  return val;
}

int main(int argc, char** argv){

  auto res = bar(7);
  if(res.has_value()) {
    std::println("success!\n");
  } else {
    std::println("ERROR: {}", res.error().msg);
  }

  //std::wcout << fmt(L"this is a test {}", L"motherfucker!!") << std::endl;
  //std::cout << fmt("hello bitch!!!! {} {}", "yes", 3.1e1) << std::endl;

  // std::vector<sys::StringView> strs
  // = { "--output-directory", "--demangle-funcs=true", "-a", "3123", "-z" };
  //
  //  argp::Parser parser;
  //  parser
  //    .add_param(argp::Parameter::create("--output-directory", "-f", "The foo argument" ))
  //    .add_param(argp::Parameter::create("--demangle-funcs", "-b", "The bar argument", false, argp::Value{"thedefault"}))
  //    .add_param(argp::Parameter::create("--time-actions", "-a", "The asshole argument", true))
  //    .add_param(argp::Parameter::create("--jobs", "-z", "idk lol", true));
  //
  //  if(!parser.parse(strs)) {
  //     return 1;
  //  }
  //
  //  parser.debug_print();
  //  parser.print();
  //
  //  const auto val = parser.get_arg("--time-actions");
  //  if(val) {
  //    std::println("Value: \"{}\"", val->value_);
  //  }

  //try {
  //  const auto file = MUST(FileRef::open(CURRENT_TEST));
  //  auto lxr = Lexer::create(*file);
  //  if(!lxr) {
  //    return 1;
  //  }
//
  //  lxr->get()->dump();
  //} catch(const std::exception& e) {
  //  std::cerr << "EXCEPTION: " << e.what() << std::endl;
  //}

  return 0;
}


