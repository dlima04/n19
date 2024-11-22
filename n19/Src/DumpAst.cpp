/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" License.
* See the LICENSE file located at this project's root directory for
* more information.
*/

#include <AstNodes.h>
#include <ConManip.h>
#include <Defer.h>
#include <print>

auto n19::AstNode::_print(
  const uint32_t depth,
  const std::string& node_name ) const -> void
{
  for(uint32_t i = 0; i < depth; i++) {
    std::print("  |");
  } if(depth) {
    std::print("_ ");
  }

  std::print(                   //----------------
    "{}{}{}{} ",                // The "title"
    manip_string(Con::Bold),    // Set bold
    manip_string(Con::Magenta), // Set fg: magenta
    node_name,                  // The AST node's name.
    manip_string(Con::Reset));  // Reset color
  std::print(                   //----------------
    "<{}{}{},{}{}{}> :: ",      // Line, position info.
    manip_string(Con::Yellow),  // Line number: Yellow.
    this->line_,                // Print line number.
    manip_string(Con::Reset),   // Reset color for ','.
    manip_string(Con::Yellow),  // File Position: Yellow.
    this->pos_,                 // Print file position.
    manip_string(Con::Reset)    // Reset color.
  );                            //----------------
}

auto n19::AstBranch::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "Branch");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }

  std::println(
    "{}has_else = {}{}",
    manip_string(Con::White),
    else_ ? "true" : "false",
    manip_string(Con::Reset)
  );

  if_->print(depth + 1, "Branch.If");
  if(else_) else_->print(depth + 1, "Branch.Else");
}

auto n19::AstConstBranch::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "ConstBranch");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }

  std::println(
    "{}has_otherwise = {}{}",
    manip_string(Con::White),
    otherwise_ ? "true" : "false",
    manip_string(Con::Reset)
  );

  where_->print(depth + 1, "ConstBranch.Where");
  if(otherwise_ != nullptr)
    otherwise_->print(depth + 1, "ConstBranch.Otherwise");
}

auto n19::AstIf::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "If");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }

  std::println("");
  condition_->print(depth + 1, "If.Condition");
  for(const auto& child : body_)
    child->print(depth + 1, std::nullopt);
}

auto n19::AstElse::print(
  const uint32_t depth,
  const Maybe<std::string> &alias ) const -> void
{
  _print(depth, "Else");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }

  std::println("");
  for(const auto& child : body_)
    child->print(depth + 1, std::nullopt);
}

auto n19::AstWhere::print(
  const uint32_t depth,
  const Maybe<std::string> &alias ) const -> void
{
  _print(depth, "Where");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }

  std::println("");
  condition_->print(depth + 1, "Where.Condition");
  for(const auto& child : body_)
    child->print(depth + 1, std::nullopt);
}

auto n19::AstOtherwise::print(
  const uint32_t depth,
  const Maybe<std::string> &alias ) const -> void
{
  _print(depth, "Otherwise");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }

  for(const auto& child : body_)
    child->print(depth + 1, std::nullopt);
}

auto n19::AstBreak::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "BreakStmt");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }
  std::println("");
}

auto n19::AstContinue::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "ContinueStmt");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }
  std::println("");
}

auto n19::AstReturn::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "ReturnStmt");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }

  std::println(
    "{}has_value = {}{}",
    manip_string(Con::White),
    value_ ? "true" : "false",
    manip_string(Con::Reset)
  );

  if(value_) {
    value_->print(depth + 1, "Return.Value");
  }
}

auto n19::AstCall::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "Call");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }

  std::println("");
  for(size_t i = 0; i < arguments_.size(); i++)
    arguments_.at(i)->print(depth + 1, fmt("Call.Args.{}", i + 1));
  target_->print(depth + 1, "Call.Target");
}

auto n19::AstDefer::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "Defer");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }

  std::println("");
  call_->print(depth + 1, "Defer.Target");
}

auto n19::AstDeferIf::print(
  const uint32_t depth,
  const Maybe<std::string> &alias ) const -> void
{
  _print(depth, "DeferIf");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }

  std::println("");
  condition_->print(depth + 1, "DeferIf.Condition");
  call_->print(depth + 1, "DeferIf.Target");
}

auto n19::AstVardecl::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "VarDecl");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }

  std::println("");
  name_->print(depth + 1, "VarDecl.Name");
  type_->print(depth + 1, "Vardecl.Type");
}

auto n19::AstProcDecl::print(
  const uint32_t depth,
  const Maybe<std::string> &alias ) const -> void
{
  _print(depth, "VarDecl");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }

  std::println("");
  name_->print(depth + 1, "ProcDecl.Name");

  for(size_t i = 0; i < arg_decls_.size(); i++) {
    arg_decls_.at(i)->print(depth + 1, fmt("ProcDecl.Arg.{}", i + 1));
  }
  for(const auto& child : body_) {
    child->print(depth + 1, std::nullopt);
  }
}

auto n19::AstCase::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "Case");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }

  std::print(
    "{}is_fallthrough = {}{}\n",         // Title
    manip_string(Con::White),            // Clad in white.
    is_fallthrough ? "True" : "False",   // Fallthrough = C style case
    manip_string(Con::Reset)             // Reset color
  );

  value_->print(depth + 1, "Case.Value");
  for(const auto& child : children_) {
    child->print(depth + 1, std::nullopt);
  }
}

auto n19::AstDefault::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "Default");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }

  std::println("");
  for(const auto& child : children_)
    child->print(depth + 1, std::nullopt);
}

auto n19::AstSwitch::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "Switch");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }

  std::println(
    "num_cases = {}{}{}",
    manip_string(Con::Blue),
    cases_.size(),
    manip_string(Con::Reset)
  );

  target_->print(depth + 1, "Switch.Target");
  dflt_->print(depth + 1, "Switch.Default");
  for(size_t i = 0; i < cases_.size(); i++) {
    cases_.at(i)->print(depth + 1, fmt("Switch.Case.{}", i + 1));
  }
}

auto n19::AstScopeBlock::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "ScopeBlock");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }

  std::println("");
  for(const auto& child : children_)
    child->print(depth + 1, std::nullopt);
}

auto n19::AstFor::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "For");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }

  set_console(Con::White);
  if(init_) std::print("Init ");
  if(cond_) std::print("Cond ");
  if(update_) std::print("Update ");
  set_console(Con::Reset);

  std::println("");
  if(init_) init_->print(depth + 1, "For.Init");
  if(cond_) cond_->print(depth + 1, "For.Cond");
  if(update_) update_->print(depth + 1, "For.Update");
}

auto n19::AstWhile::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "While");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }

  std::print(
    "{}is_dowhile = {}{}\n",
    manip_string(Con::White),
    is_dowhile ? "True" : "False",
    manip_string(Con::Reset)
  );

  cond_->print(depth + 1, "While.Cond");
  for(const auto& child : body_)
    child->print(depth + 1, std::nullopt);
}

auto n19::AstSubscript::print(
  const uint32_t depth,
  const Maybe<std::string> &alias ) const -> void
{
  _print(depth, "Subscript");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }

  std::println("");
  operand_->print(depth + 1, "Subscript.Operand");
  value_->print(depth + 1, "Subscript.Value");
}

auto n19::AstBinExpr::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "BinExpr");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }
  
  set_console(Con::Blue);
  std::print("{}\n", op_type_.to_string());
  set_console(Con::Reset);

  right_->print(depth + 1, "Binexpr.Right");
  left_->print(depth + 1, "Binexpr.Left");
}

auto n19::AstUnaryExpr::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "UnaryExpr");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }

  std::print(
    "{}{}{} {}is_postfix = {}\n",
    manip_string(Con::Blue),
    op_type_.to_string(),
    manip_string(Con::Reset),
    manip_string(Con::White),
    is_postfix ? "True" : "False"
  );

  set_console(Con::Reset);
  operand_->print(depth + 1, "UnaryExpr.Operand");
}

auto n19::AstScalarLiteral::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "ScalarLit");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }

  set_console(Con::Blue);
  std::print("{}\n", value_);
  set_console(Con::Reset);
}

auto n19::AstAggregateLiteral::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "AggregateLit");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\"\n", *alias);
    set_console(Con::Reset);
  } else {
    std::println("");
  }

  for(const auto& child : children_)
    child->print(depth + 1, std::nullopt);
}

auto n19::AstEntityRef::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "EntityRef");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }

  set_console(Con::Blue);
  std::println("ID = {}", (uint32_t)id_);
  set_console(Con::Reset);
}

auto n19::AstEntityRefThunk::print(
  const uint32_t depth,
  const Maybe<std::string> &alias ) const -> void
{
  _print(depth, "EntityRefThunk");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }

  set_console(Con::Blue);
  for(const auto& str : name_) {
    std::print("{}", str);
  }

  set_console(Con::Reset);
  std::println("");
}

auto n19::AstTypeRef::print(
  const uint32_t depth,
  const Maybe<std::string> &alias ) const -> void
{
  _print(depth, "TypeRef");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }

  std::println("{}", descriptor_.format());
}

auto n19::AstTypeRefThunk::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "TypeRefThunk");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }

  std::println("{}", descriptor_.format());
}



