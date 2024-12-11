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
#include <iostream>
#include <print>

struct foo {
  int x = 0;
  int y = 123;
  double b = 3.11;
};

using namespace n19;
int main(int argc, char** argv){
  //std::wcout << fmt(L"this is a test {}", L"motherfucker!!") << std::endl;
  //std::cout << fmt("hello bitch!!!! {} {}", "yes", 3.1e1) << std::endl;

  // auto thing = COStream::from_stdout();
  // if(!thing) {
  //   std::cerr << "failed: " << thing.error().msg;
  // }

  try {
    const auto file = MUST(FileRef::open(CURRENT_TEST));
    auto lxr = Lexer::create(*file);
    if(!lxr) {
      return 1;
    }

    lxr.value()->dump();
  } catch(const std::exception& e) {
    std::cerr << "EXCEPTION: " << e.what() << std::endl;
  }

  return 0;
}


