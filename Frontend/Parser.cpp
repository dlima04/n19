/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Frontend/Parser.hpp>
#include <Sys/File.hpp>
#include <algorithm>
#include <filesystem>
BEGIN_NAMESPACE(n19::detail_);

auto parse_begin_(
  ParseContext &ctx,
  bool nocheck_term, bool parse_single ) -> Result<AstNode::Ptr<>>
{
  const auto curr     = ctx.lxr.current();
  AstNode::Ptr<> expr = nullptr;

  ///
  /// Check if EOF has been reached.
  if(curr == TokenType::EndOfFile) {
    return Result<AstNode::Ptr<>>(nullptr);
  }

  ///
  /// Check categories, recursive descent on da dih
  /// frm ts 🍆
  if(curr.cat_.isa(TokenCategory::Punctuator)) {
    expr = TRY(parse_starting_punctuator_(ctx));
  }
  else if(curr.cat_.isa(TokenCategory::Literal)) {
    expr = TRY(parse_scalar_lit_(ctx));
  }
  else if(curr.cat_.isa(TokenCategory::Keyword)) {
    expr = TRY(parse_keyword_(ctx));
  }
  else if(curr.cat_.isa(TokenCategory::UnaryOp | TokenCategory::ValidPrefix)) {
    expr = TRY(parse_unary_prefix_(ctx));
  }
  else if(curr.type_ == TokenType::Identifier) {
    expr = TRY(parse_identifier_(ctx, std::move(expr)));
  }

  ///
  /// Illegal token, reject ts...
  else if(curr == TokenType::Illegal) {
    ErrorCollector::display_error(
      "Illegal token.",
      ctx.lxr,
      curr,
      ctx.errstream);
    return Error{ErrC::BadToken};
  }

  ///
  /// gang wtf is this token ❓user tweaking 😭
  else {
    ErrorCollector::display_error(
      "Wtf is this shit bro",
      ctx.lxr,
      curr,
      ctx.errstream);
    return Error{ErrC::BadToken};
  }

  ///
  /// Special exceptions: these expressions NEVER
  /// require a terminal character (';' or ',')
  switch(expr->type_) {
    case AstNode::Type::ProcDecl:
    case AstNode::Type::Namespace:
    case AstNode::Type::ScopeBlock:
    case AstNode::Type::For:
    case AstNode::Type::While:
    case AstNode::Type::Branch:
    case AstNode::Type::ConstBranch:
    return Result<AstNode::Ptr<>>(std::move(expr));
    default: break;
  }

  ///
  /// Check for postfixes (this is quite messy, pls refactor)
  while(true) {
    if(expr == nullptr) {
      return Result<AstNode::Ptr<>>(std::move(expr));
    }

    const auto pfcurr = ctx.lxr.current();
    if(pfcurr == TokenType::LeftSqBracket) {
      expr = TRY(parse_subscript_(ctx, std::move(expr)));
      continue;
    }

    if(pfcurr == TokenType::LeftParen) {
      expr = TRY(parse_call_(ctx, std::move(expr)));
      continue;
    }

    if((pfcurr == TokenType::Dot
      || pfcurr == TokenType::NamespaceOperator
      || pfcurr == TokenType::SkinnyArrow) && !parse_single
    ){
      expr = TRY(parse_binary_expression_(ctx, std::move(expr)));
      continue;
    }

    break;
  }

  ///
  /// Check if we're leaving a parenthesized expression
  if(ctx.lxr.current() == TokenType::RightParen) {
    if(!ctx.paren_level_) {
      ErrorCollector::display_error(
        "Unexpected token",
        ctx.lxr,
        ctx.lxr.current(),
        ctx.errstream);
      return Error{ErrC::BadToken};
    }

    if(!parse_single) {
      --ctx.paren_level_;
      ctx.lxr.consume(1);
    }
  }

  if(nocheck_term || parse_single) {
    return Result<AstNode::Ptr<>>(std::move(expr));
  }

  if(ctx.lxr.current().is_terminator()) {
    if(ctx.paren_level_) {
      ErrorCollector::display_error(
        "Unexpected token inside of parenthesized expression",
        ctx.lxr,
        ctx.lxr.current(),
        ctx.errstream);
      return Error{ErrC::BadToken};
    }

    ctx.lxr.consume(1);
    return Result<AstNode::Ptr<>>(std::move(expr));
  }

  ErrorCollector::display_error(
    "Unexpected token",
    ctx.lxr,
    ctx.lxr.current(),
    ctx.errstream);
  return Error{ErrC::BadToken};
}

auto parse_impl_(ParseContext &ctx) -> bool {
  do {
    while (true) {
      auto toplevel_decl = detail_::parse_begin_(ctx, false, false);

      /// An error has occurred. We're done.
      if (!toplevel_decl.has_value()) {
        break;
      }

      /// Note: a returned value of nullptr indicates that the
      /// parser has encountered a valid sequence of tokens, but those tokens
      /// do not produce an AST node. An example would be certain
      /// "@" directives, or type declarations.
      if (*toplevel_decl == nullptr) {
        continue;
      }

      /// Verify that the returned node is valid at the toplevel
      /// (i.e. can exist at the global scope).
      if (!detail_::is_node_toplevel_valid_(*toplevel_decl)) {
        ctx.errors.store_error(
          "Expression is invalid at the toplevel.",
          ctx.lxr.file_name_,
          (*toplevel_decl)->pos_,
          (*toplevel_decl)->line_);
        return false;
      }

      /// Store the toplevel node within the parsing context.
      ctx.toplevel_decls_.emplace_back(std::move(*toplevel_decl));
    }

    /// We expect the final token to be an EOF. If this isn't the case,
    /// an error has occurred and parsing has failed.
    if (ctx.lxr.current() != TokenType::EndOfFile) {
      return false;
    }

  } while (detail_::get_next_include_(ctx));

  return true;
}

auto parse_starting_punctuator_(ParseContext &ctx) -> Result<AstNode::Ptr<>> {
  /// TODO: things like ';', '(', etc
  return Error{ErrC::NotImplimented};
}

auto parse_scalar_lit_(ParseContext &ctx) -> Result<AstNode::Ptr<>> {
  const auto curr = ctx.lxr.current();
  ASSERT(curr.cat_.isa(TokenCategory::Literal));

  auto node = AstNode::create<AstScalarLiteral>(
    curr.pos_,
    curr.line_,
    nullptr,
    ctx.lxr.file_name_);

  // std::string value_;
  // enum : uint8_t {
  //   None,
  //   NullLit,
  //   IntLit,
  //   StringLit,
  //   U8Lit,
  //   FloatLit,
  // } scalar_type_ = None;

  auto val_ = curr.value(ctx.lxr);
  ASSERT(val_.has_value());
  node->value_ = std::move(*val_);

  switch(curr.type_.value) {
  case TokenType::FloatLiteral:
    break;
  case TokenType::IntLiteral:
    break;
  case TokenType::BooleanLiteral:
    break;
  case TokenType::NullLiteral:
    break;
  case TokenType::ByteLiteral:
    break;
  case TokenType::HexLiteral:
    break;
  case TokenType::OctalLiteral:
    break;
  case TokenType::StringLiteral:
    break;
  default:
    break;
  }

  return Error{ErrC::NotImplimented};
}

auto parse_aggregate_lit_(ParseContext &ctx) -> Result<AstNode::Ptr<>> {
  /// TODO: scalar literals
  return Error{ErrC::NotImplimented};
}

auto parse_keyword_(ParseContext &ctx) -> Result<AstNode::Ptr<>> {
  /// TODO: keywords
  return Error{ErrC::NotImplimented};
}

auto parse_unary_prefix_(ParseContext &ctx) -> Result<AstNode::Ptr<>> {
  /// TODO: unary expressions
  return Error{ErrC::NotImplimented};
}

auto parse_subscript_(
  ParseContext &ctx, AstNode::Ptr<> &&operand) -> Result<AstNode::Ptr<>>
{
  return Error{ErrC::NotImplimented};
}

auto parse_call_(
  ParseContext &ctx, AstNode::Ptr<> &&operand) -> Result<AstNode::Ptr<>>
{
  return Error{ErrC::NotImplimented};
}

auto parse_binary_expression_(
  ParseContext &ctx, AstNode::Ptr<> &&operand) -> Result<AstNode::Ptr<>>
{
  return Error{ErrC::NotImplimented};
}

auto parse_identifier_(
  ParseContext &ctx, AstNode::Ptr<> &&operand) -> Result<AstNode::Ptr<>>
{
  return Error{ErrC::NotImplimented};
}

auto is_node_toplevel_valid_(const AstNode::Ptr<>& ptr) -> bool {
  switch (ptr->type_) {
    case AstNode::Type::Namespace:
    case AstNode::Type::Where:
    case AstNode::Type::Vardecl: return true;
    default: return false;
  }
}

auto get_next_include_(ParseContext& ctx) -> bool {
  auto next = std::ranges::find_if(ctx.includes_, [](const IncludedFile& f) {
    return f.state_ == IncludeState::Pending;
  });

  if(next == ctx.includes_.end() || !std::filesystem::exists(next->name_))
    return false;

  /// ts is so fucking retarded 🥀💔
  std::filesystem::path path(next->name_);

#ifdef N19_WIN32
  auto file = sys::File::open(path.wstring());
#else /// POSIX
  auto file = sys::File::open(path.string());
#endif

  if(!file.has_value()) return false;
  ASSERT(ctx.lxr.reset(*file));

  next->state_ = IncludeState::Finished;
  return true;
}

END_NAMESPACE(n19::detail_);
BEGIN_NAMESPACE(n19);

auto parse(ParseContext& ctx) -> bool {
  if(!detail_::parse_impl_(ctx)) {
    ctx.errors.emit(ctx.errstream);
  }

  return true;
}

END_NAMESPACE(n19);
