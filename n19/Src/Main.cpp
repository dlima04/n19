#include <print>
#include <iostream>
#include <Lexer.h>
#include <Result.h>
#include <ResultMacros.h>
#include <AstNodes.h>
#include <EntityQualifier.h>
#include <ConManip.h>
#include <memory>
#include <StopWatch.h>

#define CURRENT_TEST "C:\\Users\\diago\\Desktop\\compiler_tests\\test1.txt"
using namespace n19;

Result<AstNode::Ptr<>> foo() {
  auto branch = std::make_unique<AstBranch>(
    30, 3, ""
  );

  auto _if = std::make_unique<AstIf>(21, 300, "");
  auto _else = std::make_unique<AstElse>(33,1,"");
  auto cond =  std::make_unique<AstScalarLiteral>(121,333, "");

  _else->body_.emplace_back(std::make_unique<AstReturn>(1,2,""));
  _if->body_.emplace_back(std::make_unique<AstBreak>(123,22,""));
  cond->value_    = "true";
  _if->condition_ = std::move(cond);

  branch->if_ = std::move(_if);
  //branch->else_ = std::move(_else);
  return make_result<AstNode::Ptr<>>(std::move(branch));
}

int main() {


  /*try {
    const auto file = MUST(FileRef::create(CURRENT_TEST));
    auto lxr = MUST(Lexer::create(*file));
    do {
      lxr->advance(1);
      std::cout << lxr->current().format() << std::flush;
    } while(lxr->current() != TokenType::EndOfFile && lxr->current() != TokenType::Illegal);

    if(lxr->current() == TokenType::Illegal) {
      lxr->error("Illegal token!");
    }
  } catch(const std::exception& e) {
    std::cerr << "EXCEPTION: " << e.what() << std::endl;
    return 1;
  }*/

  std::cout.flush();
  return 0;
}
