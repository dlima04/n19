/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#define CURRENT_TEST "/Users/Diago/Desktop/compiler_tests/test2.txt"

#include <Core/Try.hpp>
#include <Frontend/Lexer.hpp>
#include <Core/Bytes.hpp>
#include <IO/Console.hpp>
#include <Sys/Time.hpp>
#include <iostream>
#include <Core/Panic.hpp>
#include <Core/ArgParse.hpp>
#include <Core/TypeTraits.hpp>
#include <Core/RingQueue.hpp>
#include <Frontend/ErrorCollector.hpp>
#include <type_traits>
#include <Core/Ref.hpp>
#include <Core/Tuple.hpp>

using namespace n19;

struct MyArgs : argp::Parser {
  int64_t& num_jobs  = arg<int64_t>("--num-jobs", "-j", "numba of jobs");
  sys::String& name  = arg<sys::String>("--input", "-i", "the input file.", "Default value!!!");
  bool& verbose      = arg<bool>("--verbose", "-v", "verbose mode");
  argp::PackType& arr = arg<argp::PackType>("--blabla", "-b", "idk lol");
};

auto test() -> Tuple<std::string, double> {
  auto z = n19::make_tuple(std::string("lololol"), 314);
  return z;
}

int main(int argc, char** argv){

  auto res = test();
  outs() << res.at<0>() << '\n';

  // try {
  //   const auto file = MUST(FileRef::open(CURRENT_TEST));
  //   auto lxr = Lexer::create_shared(file);
  //   if(!lxr) {
  //     return 1;
  //   }
  //
  //   lxr->get()->dump(outs());
  // } catch(const std::exception& e) {
  //   std::cerr << "EXCEPTION: " << e.what() << std::endl;
  // }

  outs().flush();
  errs().flush();
  return 0;
}


