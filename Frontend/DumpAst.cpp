/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Frontend/AstNodes.hpp>
BEGIN_NAMESPACE(n19);

auto AstNode::print_(
  const uint32_t depth,
  OStream& stream,
  const std::string& node_name ) const -> void
{
  for(uint32_t i = 0; i < depth; i++) 
    stream << "  |";
  if(depth) 
    stream << "_ ";
  
  stream              ////////////////////////////////////
    << Con::Bold      // Begin "title":
    << Con::MagentaFG // Bold, magenta.
    << node_name      // The node's name.
    << Con::Reset;    // Reset colour.
  stream              //
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
  OStream& stream,
  const Maybe<std::string> &alias ) const -> void
{
  print_(depth, stream, "Branch");
  if(alias.has_value()) 
    stream
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;
  
  stream
    << Con::WhiteFG
    << "has_else = "
    << (else_ ? "true" : "false")
    << Con::Reset;

  if_->print(depth + 1, stream, "Branch.If");
  if(else_) else_->print(depth + 1, stream, "Branch.Else");
}

auto AstConstBranch::print(
  const uint32_t depth,
  OStream& stream,
  const Maybe<std::string> &alias ) const -> void
{
  print_(depth, stream, "ConstBranch");
  if(alias.has_value()) 
    stream
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  stream
    << Con::WhiteFG
    << "has_otherwise = "
    << (otherwise_ ? "true" : "false")
    << Con::Reset;

  where_->print(depth + 1, stream, "ConstBranch.Where");
  if(otherwise_ != nullptr)
    otherwise_->print(depth + 1, stream, "ConstBranch.Otherwise");
}

auto AstIf::print(
  const uint32_t depth,
  OStream& stream,
  const Maybe<std::string> &alias ) const -> void
{
  print_(depth, stream, "If");
  if(alias.has_value()) 
    stream
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  stream << '\n';
  condition_->print(depth + 1, stream, "If.Condition");
  for(const auto& child : body_)
    child->print(depth + 1, stream, Nothing);
}

auto AstElse::print(
  const uint32_t depth,
  OStream& stream,
  const Maybe<std::string> &alias ) const -> void
{
  print_(depth, stream, "Else");
  if(alias.has_value()) 
    stream
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  stream << '\n';
  for(const auto& child : body_)
    child->print(depth + 1, stream, Nothing);
}

auto AstWhere::print(
  const uint32_t depth,
  OStream& stream,
  const Maybe<std::string> &alias ) const -> void
{
  print_(depth, stream, "Where");
  if(alias.has_value()) 
    stream
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  stream << '\n';
  condition_->print(depth + 1, stream, "Where.Condition");
  for(const auto& child : body_)
    child->print(depth + 1, stream, Nothing);
}

auto AstOtherwise::print(
  const uint32_t depth,
  OStream& stream,
  const Maybe<std::string> &alias ) const -> void
{
  print_(depth, stream, "Otherwise");
  if(alias.has_value()) 
    stream
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  stream << '\n';
  for(const auto& child : body_)
    child->print(depth + 1, stream, Nothing);
}

auto AstBreak::print(
  const uint32_t depth,
  OStream& stream,
  const Maybe<std::string> &alias ) const -> void
{
  print_(depth, stream, "BreakStmt");
  if(alias.has_value()) 
    stream
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;
  stream << '\n';
}

auto AstContinue::print(
  const uint32_t depth,
  OStream& stream,
  const Maybe<std::string> &alias ) const -> void
{
  print_(depth, stream, "ContinueStmt");
  if(alias.has_value()) 
    stream
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;
  stream << '\n';
}

auto AstReturn::print(
  const uint32_t depth,
  OStream& stream,
  const Maybe<std::string> &alias ) const -> void
{
  print_(depth, stream, "ReturnStmt");
  if(alias.has_value()) 
    stream
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  stream
    << Con::WhiteFG
    << "has_value = "
    << (value_ ? "true" : "false")
    << Con::Reset;

  if(value_ != nullptr)
    value_->print(depth + 1, stream, "Return.Value");
}

auto AstCall::print(
  const uint32_t depth,
  OStream& stream,
  const Maybe<std::string> &alias ) const -> void
{
  print_(depth, stream, "Call");
  if(alias.has_value()) 
    stream
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  stream << '\n';
  for(size_t i = 0; i < arguments_.size(); i++) {
    arguments_.at(i)->print(depth + 1, stream, fmt("Call.Args.{}", i + 1));
  }
  target_->print(depth + 1, stream, "Call.Target");
}

auto AstDefer::print(
  const uint32_t depth,
  OStream& stream,
  const Maybe<std::string> &alias ) const -> void
{
  print_(depth, stream, "Defer");
  if(alias.has_value()) 
    stream
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  stream << '\n';
  call_->print(depth + 1, stream, "Defer.Target");
}

auto AstDeferIf::print(
  const uint32_t depth,
  OStream& stream,
  const Maybe<std::string> &alias ) const -> void
{
  print_(depth, stream, "DeferIf");
  if(alias.has_value()) 
    stream
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  stream << '\n';
  condition_->print(depth + 1, stream, "DeferIf.Condition");
  call_->print(depth + 1, stream, "DeferIf.Target");
}

auto AstVardecl::print(
  const uint32_t depth,
  OStream& stream,
  const Maybe<std::string> &alias ) const -> void
{
  print_(depth, stream, "VarDecl");
  if(alias.has_value()) 
    stream
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  stream << '\n';
  name_->print(depth + 1, stream, "VarDecl.Name");
  type_->print(depth + 1, stream, "Vardecl.Type");
}

auto AstProcDecl::print(
  const uint32_t depth,
  OStream& stream,
  const Maybe<std::string> &alias ) const -> void
{
  print_(depth, stream, "VarDecl");
  if(alias.has_value()) 
    stream
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  stream << '\n';
  name_->print(depth + 1, stream, "ProcDecl.Name");

  for(size_t i = 0; i < arg_decls_.size(); i++)
    arg_decls_.at(i)->print(depth + 1, stream, fmt("ProcDecl.Arg.{}", i + 1));

  for(const auto& child : body_)
    child->print(depth + 1, stream, Nothing);
}

auto AstCase::print(
  const uint32_t depth,
  OStream& stream,
  const Maybe<std::string> &alias ) const -> void
{
  print_(depth, stream, "Case");
  if(alias.has_value()) 
    stream
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  stream
    << Con::WhiteFG
    << "is_fallthrough = "
    << (is_fallthrough ? "True\n" : "False\n")
    << Con::Reset;

  value_->print(depth + 1, stream, "Case.Value");
  for(const auto& child : children_)
    child->print(depth + 1, stream, Nothing);
}

auto AstDefault::print(
  const uint32_t depth,
  OStream& stream,
  const Maybe<std::string> &alias ) const -> void
{
  print_(depth, stream, "Default");
  if(alias.has_value()) 
    stream
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  stream << '\n';
  for(const auto& child : children_)
    child->print(depth + 1, stream, Nothing);
}

auto AstSwitch::print(
  const uint32_t depth,
  OStream& stream,
  const Maybe<std::string> &alias ) const -> void
{
  print_(depth, stream, "Switch");
  if(alias.has_value()) 
    stream
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  stream
    << "num_cases = "
    << Con::BlueFG
    << cases_.size()
    << Con::Reset
    << Endl;

  target_->print(depth + 1, stream, "Switch.Target");
  dflt_->print(depth + 1, stream, "Switch.Default");

  for(size_t i = 0; i < cases_.size(); i++)
    cases_.at(i)->print(depth + 1, stream, fmt("Switch.Case.{}", i + 1));
}

auto AstScopeBlock::print(
  const uint32_t depth,
  OStream& stream,
  const Maybe<std::string> &alias ) const -> void
{
  print_(depth, stream, "ScopeBlock");
  if(alias.has_value()) 
    stream
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  stream << '\n';
  for(const auto& child : children_)
    child->print(depth + 1, stream, Nothing);
}

auto AstFor::print(
  const uint32_t depth,
  OStream& stream,
  const Maybe<std::string> &alias ) const -> void
{
  print_(depth, stream, "For");
  if(alias.has_value()) 
    stream
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  stream << Con::WhiteFG;
  if(init_) stream   << "Init ";
  if(cond_) stream   << "Cond ";
  if(update_) stream << "Update ";
  stream << Con::Reset;

  stream << '\n';
  if(init_) init_->print(depth + 1, stream, "For.Init");
  if(cond_) cond_->print(depth + 1, stream, "For.Cond");
  if(update_) update_->print(depth + 1, stream, "For.Update");
}

auto AstWhile::print(
  const uint32_t depth,
  OStream& stream,
  const Maybe<std::string> &alias ) const -> void
{
  print_(depth, stream, "While");
  if(alias.has_value()) 
    stream
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  stream
    << Con::WhiteFG
    << "is_dowhile = "
    << (is_dowhile ? "True\n" : "False\n")
    << Con::Reset;

  cond_->print(depth + 1, stream, "While.Cond");
  for(const auto& child : body_)
    child->print(depth + 1, stream, Nothing);
}

auto AstSubscript::print(
  const uint32_t depth,
  OStream& stream,
  const Maybe<std::string> &alias ) const -> void
{
  print_(depth, stream, "Subscript");
  if(alias.has_value()) 
    stream
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;
  
  stream << '\n';
  operand_->print(depth + 1, stream, "Subscript.Operand");
  value_->print(depth + 1, stream, "Subscript.Value");
}

auto AstBinExpr::print(
  const uint32_t depth,
  OStream& stream,
  const Maybe<std::string> &alias ) const -> void
{
  print_(depth, stream, "BinExpr");
  if(alias.has_value()) 
    stream
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;
  
  const auto as_str = op_type_.to_string();
  stream
    << Con::BlueFG
    << as_str
    << Con::Reset
    << '\n';

  right_->print(depth + 1, stream, "Binexpr.Right");
  left_->print(depth + 1, stream, "Binexpr.Left");
}

auto AstUnaryExpr::print(
  const uint32_t depth,
  OStream& stream,
  const Maybe<std::string> &alias ) const -> void
{
  print_(depth, stream, "UnaryExpr");
  if(alias.has_value()) 
    stream
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  const auto as_str = op_type_.to_string();

  stream // -------------
    << Con::BlueFG
    << as_str
    << Con::Reset
    << Con::WhiteFG
    << " is_postfix = "
    << (is_postfix ? "True\n" : "False\n")
    << Con::Reset;

  operand_->print(depth + 1, stream, "UnaryExpr.Operand");
}

auto AstScalarLiteral::print(
  const uint32_t depth,
  OStream& stream,
  const Maybe<std::string> &alias ) const -> void
{
  print_(depth, stream, "ScalarLit");
  if(alias.has_value()) 
    stream
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  stream
    << Con::BlueFG
    << value_
    << '\n'
    << Con::Reset;
}

auto AstAggregateLiteral::print(
  const uint32_t depth,
  OStream& stream,
  const Maybe<std::string> &alias ) const -> void
{
  print_(depth, stream, "AggregateLit");
  if(alias.has_value())
    stream
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  stream << '\n';
  for(const auto& child : children_)
    child->print(depth + 1, stream, Nothing);
}

auto AstEntityRef::print(
  const uint32_t depth,
  OStream& stream,
  const Maybe<std::string> &alias ) const -> void
{
  print_(depth, stream, "EntityRef");
  if(alias.has_value()) 
    stream
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  stream
    << Con::BlueFG
    << "ID = "
    << id_
    << Con::Reset
    << Endl;
}

auto AstEntityRefThunk::print(
  const uint32_t depth,
  OStream& stream,
  const Maybe<std::string> &alias ) const -> void
{
  print_(depth, stream, "EntityRefThunk");
  if(alias.has_value()) 
    stream
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  stream << Con::BlueFG;
  for(const auto& str : name_) {
    stream << str << ' ';
  }

  stream << Con::Reset << '\n';
}

auto AstTypeRef::print(
  const uint32_t depth,
  OStream& stream,
  const Maybe<std::string> &alias ) const -> void
{
  print_(depth, stream, "TypeRef");
  if(alias.has_value()) 
    stream
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  const auto formatted = descriptor_.format();
  stream << formatted << '\n';
}

auto AstTypeRefThunk::print(
  const uint32_t depth,
  OStream& stream,
  const Maybe<std::string> &alias ) const -> void
{
  print_(depth, stream, "TypeRefThunk");
  if(alias.has_value()) 
    stream
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  const auto formatted = descriptor_.format();
  stream << formatted << '\n';
}

auto AstNamespace::print(
  const uint32_t depth,
  OStream& stream,
  const Maybe<std::string> &alias ) const -> void
{
  print_(depth, stream, "NamespaceBlock");
  if(alias.has_value()) 
    stream
      << Con::GreenFG
      << fmt("\"{}\" ", *alias)
      << Con::Reset;

  stream << '\n';
  for(const auto& child : body_)
    child->print(depth + 1, stream, Nothing);
}

END_NAMESPACE(n19);