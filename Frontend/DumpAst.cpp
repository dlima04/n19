/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#include <Frontend/AstNodes.hpp>
#include <Core/ConManip.hpp>
#include <print>
BEGIN_NAMESPACE(n19);

auto AstNode::_print(
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

auto AstBranch::print(
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

auto AstConstBranch::print(
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

auto AstIf::print(
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

auto AstElse::print(
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

auto AstWhere::print(
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

auto AstOtherwise::print(
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

auto AstBreak::print(
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

auto AstContinue::print(
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

auto AstReturn::print(
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

auto AstCall::print(
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
  for(size_t i = 0; i < arguments_.size(); i++) {
    arguments_.at(i)->print(depth + 1, fmt("Call.Args.{}", i + 1));
  }
  target_->print(depth + 1, "Call.Target");
}

auto AstDefer::print(
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

auto AstDeferIf::print(
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

auto AstVardecl::print(
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

auto AstProcDecl::print(
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

auto AstCase::print(
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

auto AstDefault::print(
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

auto AstSwitch::print(
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

auto AstScopeBlock::print(
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

auto AstFor::print(
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

auto AstWhile::print(
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

auto AstSubscript::print(
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

auto AstBinExpr::print(
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

auto AstUnaryExpr::print(
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

auto AstScalarLiteral::print(
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

auto AstAggregateLiteral::print(
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

auto AstEntityRef::print(
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

auto AstEntityRefThunk::print(
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

auto AstTypeRef::print(
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

auto AstTypeRefThunk::print(
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

auto AstNamespace::print(
  const uint32_t depth,
  const Maybe<std::string> &alias ) const -> void
{
  _print(depth, "NamespaceBlock");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }

  std::println("");
  for(const auto& child : body_)
    child->print(depth + 1, std::nullopt);
}

END_NAMESPACE(n19);