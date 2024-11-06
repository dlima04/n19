#include <print>
#include <iostream>
#include <Lexer.h>
#include <Result.h>
#include <ResultMacros.h>
#define CURRENT_TEST "C:\\Users\\diago\\Desktop\\compiler_tests\\test1.txt"
using namespace n19;

int main() {
  try {
    const auto file = MUST(FileRef::create(CURRENT_TEST));
    auto lxr = MUST(Lexer::create(*file));
    do {
      lxr->advance(1);
      std::cout << lxr->current().format() << std::flush;
    } while(lxr->current() != TokenType::EndOfFile && lxr->current() != TokenType::Illegal);

    if(lxr->current() == TokenType::Illegal) {
      lxr->error("Illegal token!!");
      return 1;
    }
  } catch(const std::exception& e) {
    std::cerr << "EXCEPTION: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
