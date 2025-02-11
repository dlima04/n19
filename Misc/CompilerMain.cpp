/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#define CURRENT_TEST "/Users/Diago/Desktop/compiler_tests/test2.txt"

#include <Core/Try.hpp>
#include <Frontend/Lexer.hpp>
#include <Core/Bytes.hpp>
#include <Core/ConIO.hpp>
#include <Sys/Time.hpp>
#include <iostream>
#include <Core/ArgParse.hpp>
#include <Core/TypeTraits.hpp>

using namespace n19;

struct MyArgs : argp::Parser {
  int64_t& num_jobs  = arg<int64_t>("--num-jobs", "-j", "numba of jobs");
  sys::String& name  = arg<sys::String>("--input", "-i", "the input file.", "Default value!!!");
  bool& verbose      = arg<bool>("--verbose", "-v", "verbose mode");
  argp::PackType& arr = arg<argp::PackType>("--blabla", "-b", "idk lol");
};

int main(int argc, char** argv){

   const auto time = sys::SystemTime::from_utc();
   if(!time) {
     return 1;
   }

   std::cout << std::boolalpha;
   std::cout << IsVoid<int> << std::endl;
   std::cout << IsVoid<void> << std::endl;

   std::cout << time->strings().format() << std::endl;

   //std::vector<sys::String> strs = { "--num-jobs=42069", "--verbose=true", "-b=one,two,three" };
//
   //MyArgs args;
   //auto res = args.style(argp::ArgStyle::UNIX)
   //  .take_argv(std::move(strs))
   //  .parse(outs());
//
   //if(!res) {
   //  return 1;
   //}
//
   //std::cout << "num_jobs=" << args.num_jobs << '\n';
   //std::cout << "name=" << args.name << '\n';
   //std::cout << std::boolalpha << "verbose=" << args.verbose << std::endl;
//
   //for(const auto& str : args.arr) {
   //  std::cout << str << std::endl;
   //}
//
   //args.help(outs());

  try {
    const auto file = MUST(FileRef::open(CURRENT_TEST));
    auto lxr = Lexer::create_shared(*file);
    if(!lxr) {
      return 1;
    }

    lxr.value()->dump();
  } catch(const std::exception& e) {
    std::cerr << "EXCEPTION: " << e.what() << std::endl;
  }

  outs().flush();
  errs().flush();
  return 0;
}


