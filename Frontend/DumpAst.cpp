/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#include <Frontend/AstNodes.hpp>
#include <Core/ConIO.hpp>
#include <print>
BEGIN_NAMESPACE(n19);

auto AstNode::_print(
  const uint32_t depth, const std::string& node_name ) const -> void
{
  for(uint32_t i = 0; i < depth; i++) 
    outs() << "  |";
  if(depth) 
    outs() << "_ ";
  
  outs()              ////////////////////////////////////
    << Con::Bold      // Begin "title":
    << Con::MagentaFG // Bold, magenta.
    << node_name      // The node's name.
    << Con::Reset;    // Reset colour.
  outs()              //
    << " <"           // Line, position info.
    << Con::YellowFG  // Line number: yellow.
    << this->line_    // 
    << Con::Reset     // 
    << ','            // Reset colour for ','.
    << Con::YellowFG  //
    << this->pos_     // Print position in yellow.
    << Con::Reset     // Reset the console once more.
    << "> :: ";       ////////////////////////////////////
}

auto AstBranch::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "Branch");
  if(alias.has_value()) 
    outs()
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;
  
  outs()
    << Con::WhiteFG
    << "has_else = "
    << (else_ ? "true" : "false")
    << Con::Reset;

  if_->print(depth + 1, "Branch.If");
  if(else_) else_->print(depth + 1, "Branch.Else");
}

auto AstConstBranch::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "ConstBranch");
  if(alias.has_value()) 
    outs()
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  outs()
    << Con::WhiteFG
    << "has_otherwise = "
    << (otherwise_ ? "true" : "false")
    << Con::Reset;

  where_->print(depth + 1, "ConstBranch.Where");
  if(otherwise_ != nullptr)
    otherwise_->print(depth + 1, "ConstBranch.Otherwise");
}

auto AstIf::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "If");
  if(alias.has_value()) 
    outs()
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  outs() << '\n';
  condition_->print(depth + 1, "If.Condition");
  for(const auto& child : body_)
    child->print(depth + 1, Nothing);
}

auto AstElse::print(
  const uint32_t depth,
  const Maybe<std::string> &alias ) const -> void
{
  _print(depth, "Else");
  if(alias.has_value()) 
    outs()
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  outs() << '\n';
  for(const auto& child : body_)
    child->print(depth + 1, Nothing);
}

auto AstWhere::print(
  const uint32_t depth,
  const Maybe<std::string> &alias ) const -> void
{
  _print(depth, "Where");
  if(alias.has_value()) 
    outs()
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  outs() << '\n';
  condition_->print(depth + 1, "Where.Condition");
  for(const auto& child : body_)
    child->print(depth + 1, Nothing);
}

auto AstOtherwise::print(
  const uint32_t depth,
  const Maybe<std::string> &alias ) const -> void
{
  _print(depth, "Otherwise");
  if(alias.has_value()) 
    outs()
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  outs() << '\n';
  for(const auto& child : body_)
    child->print(depth + 1, Nothing);
}

auto AstBreak::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "BreakStmt");
  if(alias.has_value()) 
    outs()
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;
  outs() << '\n';
}

auto AstContinue::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "ContinueStmt");
  if(alias.has_value()) 
    outs()
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;
  outs() << '\n';
}

auto AstReturn::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "ReturnStmt");
  if(alias.has_value()) 
    outs()
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  outs()
    << Con::WhiteFG
    << "has_value = "
    << (value_ ? "true" : "false")
    << Con::Reset;

  if(value_ != nullptr)
    value_->print(depth + 1, "Return.Value");
}

auto AstCall::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "Call");
  if(alias.has_value()) 
    outs()
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  outs() << '\n';
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
  if(alias.has_value()) 
    outs()
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  outs() << '\n';
  call_->print(depth + 1, "Defer.Target");
}

auto AstDeferIf::print(
  const uint32_t depth,
  const Maybe<std::string> &alias ) const -> void
{
  _print(depth, "DeferIf");
  if(alias.has_value()) 
    outs()
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  outs() << '\n';
  condition_->print(depth + 1, "DeferIf.Condition");
  call_->print(depth + 1, "DeferIf.Target");
}

auto AstVardecl::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "VarDecl");
  if(alias.has_value()) 
    outs()
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  outs() << '\n';
  name_->print(depth + 1, "VarDecl.Name");
  type_->print(depth + 1, "Vardecl.Type");
}

auto AstProcDecl::print(
  const uint32_t depth,
  const Maybe<std::string> &alias ) const -> void
{
  _print(depth, "VarDecl");
  if(alias.has_value()) 
    outs()
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  outs() << '\n';
  name_->print(depth + 1, "ProcDecl.Name");

  for(size_t i = 0; i < arg_decls_.size(); i++)
    arg_decls_.at(i)->print(depth + 1, fmt("ProcDecl.Arg.{}", i + 1));

  for(const auto& child : body_)
    child->print(depth + 1, Nothing);
}

auto AstCase::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "Case");
  if(alias.has_value()) 
    outs()
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  outs()
    << Con::WhiteFG
    << "is_fallthrough = "
    << (is_fallthrough ? "True\n" : "False\n")
    << Con::Reset;

  value_->print(depth + 1, "Case.Value");
  for(const auto& child : children_)
    child->print(depth + 1, Nothing);
}

auto AstDefault::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "Default");
  if(alias.has_value()) 
    outs()
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  outs() << '\n';
  for(const auto& child : children_)
    child->print(depth + 1, Nothing);
}

auto AstSwitch::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "Switch");
  if(alias.has_value()) 
    outs()
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  outs()
    << "num_cases = "
    << Con::BlueFG
    << cases_.size()
    << Con::Reset
    << Endl;

  target_->print(depth + 1, "Switch.Target");
  dflt_->print(depth + 1, "Switch.Default");

  for(size_t i = 0; i < cases_.size(); i++)
    cases_.at(i)->print(depth + 1, fmt("Switch.Case.{}", i + 1));
}

auto AstScopeBlock::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "ScopeBlock");
  if(alias.has_value()) 
    outs()
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  outs() << '\n';
  for(const auto& child : children_)
    child->print(depth + 1, Nothing);
}

auto AstFor::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "For");
  if(alias.has_value()) 
    outs()
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  outs() << Con::WhiteFG;
  if(init_) outs()   << "Init ";
  if(cond_) outs()   << "Cond ";
  if(update_) outs() << "Update ";
  outs() << Con::Reset;

  outs() << '\n';
  if(init_) init_->print(depth + 1, "For.Init");
  if(cond_) cond_->print(depth + 1, "For.Cond");
  if(update_) update_->print(depth + 1, "For.Update");
}

auto AstWhile::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "While");
  if(alias.has_value()) 
    outs()
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  outs()
    << Con::WhiteFG
    << "is_dowhile = "
    << (is_dowhile ? "True\n" : "False\n")
    << Con::Reset;

  cond_->print(depth + 1, "While.Cond");
  for(const auto& child : body_)
    child->print(depth + 1, Nothing);
}

auto AstSubscript::print(
  const uint32_t depth,
  const Maybe<std::string> &alias ) const -> void
{
  _print(depth, "Subscript");
  if(alias.has_value()) 
    outs()
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  outs() << '\n';
  operand_->print(depth + 1, "Subscript.Operand");
  value_->print(depth + 1, "Subscript.Value");
}

auto AstBinExpr::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "BinExpr");
  if(alias.has_value()) 
    outs()
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;
  
  const auto as_str = op_type_.to_string();

  outs()
    << Con::BlueFG
    << as_str
    << Con::Reset
    << '\n';

  right_->print(depth + 1, "Binexpr.Right");
  left_->print(depth + 1, "Binexpr.Left");
}

auto AstUnaryExpr::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "UnaryExpr");
  if(alias.has_value()) 
    outs()
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  const auto as_str = op_type_.to_string();

  outs() // -------------
    << Con::BlueFG
    << as_str
    << Con::Reset
    << Con::WhiteFG
    << (is_postfix ? "True\n" : "False\n")
    << Con::Reset;

  operand_->print(depth + 1, "UnaryExpr.Operand");
}

auto AstScalarLiteral::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "ScalarLit");
  if(alias.has_value()) 
    outs()
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  outs()
    << Con::BlueFG
    << value_
    << '\n'
    << Con::Reset;
}

auto AstAggregateLiteral::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "AggregateLit");
  if(alias.has_value())
    outs()
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  outs() << '\n';
  for(const auto& child : children_)
    child->print(depth + 1, Nothing);
}

auto AstEntityRef::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "EntityRef");
  if(alias.has_value()) 
    outs()
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  outs()
    << Con::BlueFG
    << "ID = "
    << id_
    << Con::Reset
    << Endl;
}

auto AstEntityRefThunk::print(
  const uint32_t depth,
  const Maybe<std::string> &alias ) const -> void
{
  _print(depth, "EntityRefThunk");
  if(alias.has_value()) 
    outs()
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  outs() << Con::BlueFG;
  for(const auto& str : name_) {
    outs() << str << ' ';
  }

  outs() << Con::Reset << '\n';
}

auto AstTypeRef::print(
  const uint32_t depth,
  const Maybe<std::string> &alias ) const -> void
{
  _print(depth, "TypeRef");
  if(alias.has_value()) 
    outs()
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  const auto formatted = descriptor_.format();
  outs() << formatted << '\n';
}

auto AstTypeRefThunk::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  _print(depth, "TypeRefThunk");
  if(alias.has_value()) 
    outs()
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  const auto formatted = descriptor_.format();
  outs() << formatted << '\n';
}

auto AstNamespace::print(
  const uint32_t depth,
  const Maybe<std::string> &alias ) const -> void
{
  _print(depth, "NamespaceBlock");
  if(alias.has_value()) 
    outs()
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  outs() << '\n';
  for(const auto& child : body_)
    child->print(depth + 1, Nothing);
}

END_NAMESPACE(n19);