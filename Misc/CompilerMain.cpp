/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#define CURRENT_TEST "/Users/Diago/Desktop/compiler_tests/test2.txt"

#include <Core/Try.hpp>
#include <Frontend/Lexer.hpp>
#include <IO/Console.hpp>
#include <Sys/Time.hpp>
#include <Sys/BackTrace.hpp>
#include <iostream>
#include <Core/Panic.hpp>
#include <Core/Defer.hpp>
#include <Core/ArgParse.hpp>
#include <Core/Tuple.hpp>
#include <Sys/File.hpp>
#include <Core/Defer.hpp>
#include <Core/Murmur3.hpp>
#include <Frontend/EntityTable.hpp>
#include <Frontend/AstNodes.hpp>

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

  EntityTable tbl("MyTable");
  auto ptr1 = tbl.insert<Variable>(N19_ROOT_ENTITY_ID, 12, 1, "MyFile.txt", "MyVar");
  auto ptr2 = tbl.insert<Struct>(N19_ROOT_ENTITY_ID, 12, 1, "MyFile.txt", "MyStruct");
  auto ptr3 = tbl.insert<Type>(ptr2->id_, 12, 1, "MyFile.txt", "MyAlias");
  auto ptr4 = tbl.insert<Struct>(ptr2->id_, 12, 1, "MyFile.txt", "SecondStruct");

  EntityQualifierBase crazyass_qualifier;
  crazyass_qualifier.ptr_depth_ = 1;
  crazyass_qualifier.flags_ |= EntityQualifierBase::Volatile;
  crazyass_qualifier.arr_lengths_.emplace_back(10);

  ptr4->members_.emplace_back("FirstMember", crazyass_qualifier, ptr3->id_);
  ptr4->members_.emplace_back("SecondMember", crazyass_qualifier, ptr3->id_);

  ptr2->members_.emplace_back("Idk", EntityQualifierBase{}, ptr3->id_);


  tbl.dump();
  outs() << "\n";
  tbl.dump_structures();

  // try {
  //   auto file = MUST(sys::File::open(CURRENT_TEST));
  //   auto lxr = Lexer::create_shared(file);
  //   if(!lxr) {
  //     return 1;
  //   }
  //
  //   lxr->get()->dump(outs());
  //
  //   // auto file2 = MUST(sys::File::open(CURRENT_TEST));
  //   //
  //   // ErrorCollector::display_error("idk lol", file2, outs(), 12, 2, true );
  //   // file2.close();
  //   file.close();
  // } catch(const std::exception& e) {
  //   std::cerr << "EXCEPTION: " << e.what() << std::endl;
  // }

  ins().clear();
  outs().flush();
  errs().flush();
  return 0;
}


