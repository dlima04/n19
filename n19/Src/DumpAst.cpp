#include <AstNodes.h>
#include <ConManip.h>
#include <Defer.h>
#include <print>

/*
  Interlapping, disconnected, related branches
  of cells within cells within cells
  within cells within one leaf within one stem within one
  branch within one trunk within one structure
*/

auto n19::AstNode::print_(
  const uint32_t depth,
  const std::string& node_name ) const -> void
{
  for(uint32_t i = 0; i < depth; i++) {
    std::print("  |");
  }

  std::print(                   //----------------
    "{}{}_ {}{} ",              // The "title"
    manip_string(Con::Bold),    // Set bold
    manip_string(Con::Magenta), // Set fg: magenta
    node_name,                  // The AST node's name.
    manip_string(Con::Reset));  // Reset color
  std::print(                   //----------------
    "<{}{}{},{}{}{}> :: ",      // Line, position, address info.
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
  print_(depth, "Branch");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  } if(else_ != nullptr) {
    set_console(Con::White);
    std::print("has_else = true");
    set_console(Con::Reset);
  }

  std::println("");
  if_->print(depth + 1, "Branch.If");
  then_->print(depth + 1, "Branch.Then");
  if(else_) else_->print(depth + 1, "Branch.Else");
}

auto n19::AstBreak::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  print_(depth, "BreakStmt");
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
  print_(depth, "ContinueStmt");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }
  std::println("");
}

auto n19::AstCall::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  print_(depth, "Call");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }

  std::println("");
  for(size_t i = 0; i < arguments_.size(); i++) {
    arguments_[i]->print(depth + 1, fmt("Call.Args.{}", i + 1));
  }
  target_->print(depth + 1, "Call.Target");
}

auto n19::AstCase::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  print_(depth, "Case");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }

  std::print(
    "{}is_fallthrough = {}{}\n",         // Title
    manip_string(Con::White),            // Clad in white.
    is_fallthrough ? "True" : "False",   // Fallthrough = C style case
    manip_string(Con::Reset)
  );

  value_->print(depth + 1, "Case.Value");
  children_->print(depth + 1, "Case.Children");
}

auto n19::AstFor::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  print_(depth, "For");
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
  print_(depth, "While");
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
  for(const auto& child : body_) {
    child->print(depth + 1, std::nullopt);
  }
}

auto n19::AstBinExpr::print(
  const uint32_t depth,
  const Maybe<std::string>& alias ) const -> void
{
  print_(depth, "BinExpr");
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
  print_(depth, "UnaryExpr");
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
  print_(depth, "ScalarLit");
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
  print_(depth, "AggregateLit");
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
  print_(depth, "EntityRef");
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
  print_(depth, "EntityRefThunk");
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
  print_(depth, "TypeRef");
  if(alias.has_value()) {
    set_console(Con::Green);
    std::print("\"{}\" ", *alias);
    set_console(Con::Reset);
  }


}

