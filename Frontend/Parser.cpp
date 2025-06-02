/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Frontend/Parser.hpp>
#include <Core/StringUtil.hpp>
#include <System/File.hpp>
#include <algorithm>
#include <utility>
#include <filesystem>
BEGIN_NAMESPACE(n19::detail_);

auto is_node_toplevel_valid_(const AstNode::Ptr<> &ptr) -> bool {
  switch (ptr->type_) {
  case AstNode::Type::Namespace:         FALLTHROUGH_;
  case AstNode::Type::Where:             FALLTHROUGH_;
  case AstNode::Type::ProcDecl:          FALLTHROUGH_;
  case AstNode::Type::Vardecl:           return true;
  default:                               return false;
  }
}

auto is_valid_subexpression_(const AstNode::Ptr<>& ptr) -> bool {
  switch (ptr->type_) {
  case AstNode::Type::Call:              FALLTHROUGH_;
  case AstNode::Type::QualifiedRef:      FALLTHROUGH_;
  case AstNode::Type::QualifiedRefThunk: FALLTHROUGH_;
  case AstNode::Type::EntityRef:         FALLTHROUGH_;
  case AstNode::Type::EntityRefThunk:    FALLTHROUGH_;
  case AstNode::Type::BinExpr:           FALLTHROUGH_;
  case AstNode::Type::ScalarLiteral:     FALLTHROUGH_;
  case AstNode::Type::AggregateLiteral:  FALLTHROUGH_;
  case AstNode::Type::UnaryExpr:         FALLTHROUGH_;
  case AstNode::Type::Subscript:         return true;
  default:                               return false;
  }
}

auto node_never_needs_terminal_(const AstNode::Ptr<>& ptr) -> bool {
  switch(ptr->type_) {
  case AstNode::Type::ProcDecl:          FALLTHROUGH_;
  case AstNode::Type::Namespace:         FALLTHROUGH_;
  case AstNode::Type::ScopeBlock:        FALLTHROUGH_;
  case AstNode::Type::For:               FALLTHROUGH_;
  case AstNode::Type::While:             FALLTHROUGH_;
  case AstNode::Type::Branch:            FALLTHROUGH_;
  case AstNode::Type::ConstBranch:       return true;
  default:                               return false;
  }
}

auto parse_begin_(
  ParseContext &ctx,
  bool nocheck_term, bool parse_single ) -> Result<AstNode::Ptr<>>
{
  const auto curr     = ctx.lxr.current();
  AstNode::Ptr<> expr = nullptr;

  ///
  /// Check if EOF has been reached.
  if(curr == TokenType::EndOfFile) {
    return Error(ErrC::None);
  }

  ///
  /// Check categories, recursive descent on da dih
  /// frm ts 🍆
  if(curr.cat_.isa(TokenCategory::Punctuator)) {
    expr = TRY(parse_punctuator_(ctx));
  }
  else if(curr.cat_.isa(TokenCategory::Literal)) {
    expr = TRY(parse_scalar_lit_(ctx));
  }
  else if(curr.type_ == TokenType::Identifier) {
    expr = TRY(parse_identifier_(ctx));
  }
  else if(curr.cat_.isa(TokenCategory::Keyword)) {
    expr = TRY(parse_keyword_(ctx));
  }
  else if(curr.cat_.isa(TokenCategory::UnaryOp | TokenCategory::ValidPrefix)) {
    expr = TRY(parse_unary_prefix_(ctx));
  }

  ///
  /// Illegal token, reject ts...
  else if(curr == TokenType::Illegal) {
    return Error{ErrC::BadToken, "Illegal token."};
  }

  ///
  /// gang wtf is this token ❓user tweaking 😭
  else {
    return Error{ErrC::BadToken, "Wtf is this shit bro"};
  }

  ///
  /// Special exceptions: these expressions NEVER
  /// require a terminal character (';' or ',')
  if(node_never_needs_terminal_(expr)) {
    return Result<AstNode::Ptr<>>(std::move(expr));
  }

  ///
  /// Check for postfixes
  while(ctx.lxr.current().cat_.isa(TokenCategory::ValidPostfix)) {
    expr = TRY(parse_postfix_(ctx, std::move(expr)));
  }

  while(!parse_single && ctx.lxr.current().cat_.isa(TokenCategory::BinaryOp)) {
    expr = TRY(parse_binexpr_(ctx, std::move(expr)));
  }

  ///
  /// Check if we're leaving a parenthesized expression
  if(ctx.lxr.current() == TokenType::RightParen) {
    if(!ctx.paren_level) {
      return Error{ErrC::BadToken, "Unexpected token."};
    }

    if(!parse_single) {
      --ctx.paren_level;
      ctx.lxr.consume(1);
    }
  }

  if(nocheck_term || parse_single) {
    return Result<AstNode::Ptr<>>(std::move(expr));
  }

  if(ctx.lxr.current().is_terminator()) {
    if(ctx.paren_level) {
      return Error{ErrC::BadToken, "Unexpected token inside parentheses."};
    }

    ctx.lxr.consume(1);
    return Result<AstNode::Ptr<>>(std::move(expr));
  }

  return Error{ErrC::BadToken, "Unexpected token."};
}

auto parse_impl_(ParseContext &ctx) -> bool {
  do {
    while (true) {
      auto toplevel_decl = detail_::parse_begin_(ctx, false, false);

      /// An error has occurred, or EOF was reached. We're done.
      if (!toplevel_decl.has_value()) {
        if (ctx.lxr.current() != TokenType::EndOfFile) {
          ErrorCollector::display_error(
            toplevel_decl.error().msg,
            ctx.lxr, 
            ctx.errstream);
        }
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
        ErrorCollector::display_error(
          "Expression is invalid at the toplevel.",
          ctx.lxr.file_name_,
          ctx.lxr.src_,
          ctx.errstream,
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

auto parse_binexpr_(ParseContext& ctx, AstNode::Ptr<>&& operand) -> Result<AstNode::Ptr<>> {
  auto curr = ctx.lxr.current();
  ASSERT(curr.cat_.isa(TokenCategory::BinaryOp));

  auto node = AstNode::create<AstBinExpr>(
    curr.pos_,
    curr.line_,
    nullptr,
    ctx.lxr.file_name_);

  node->op_type_ = curr.type_;
  node->op_cat_  = curr.cat_;
  node->left_    = std::move(operand);
  node->left_->parent_ = node.get();

  ctx.lxr.consume(1);
  node->right_ = TRY(parse_begin_(ctx, true, true));
  node->right_->parent_ = node.get();

  if(!is_valid_subexpression_(node->right_)) {
    ctx.lxr.revert_before(curr);
    return Error{ErrC::BadExpr, "Invalid expression following binary operator."};
  }

  curr = ctx.lxr.current();
  while(curr.cat_.isa(TokenCategory::BinaryOp) && curr.type_.prec() <= node->op_type_.prec()) {
    node->right_ = TRY(parse_binexpr_(ctx, std::move(node->right_)));
    curr = ctx.lxr.current();
  }

  return Result<AstNode::Ptr<>>::create(std::move(node));
}

auto parse_scalar_lit_(ParseContext &ctx) -> Result<AstNode::Ptr<>> {
  const auto curr = ctx.lxr.current();
  ASSERT(curr.cat_.isa(TokenCategory::Literal));

  auto node = AstNode::create<AstScalarLiteral>(
    curr.pos_,
    curr.line_,
    nullptr,
    ctx.lxr.file_name_);

  auto val_ = curr.value(ctx.lxr);
  ASSERT(val_.has_value());
  uint64_t converted = 0;

  try {
    switch(curr.type_.value) {
    case TokenType::FloatLiteral:
      node->scalar_type_ = AstScalarLiteral::FloatLit;
      (void)std::stod(*val_);
      node->value_       = std::move(*val_);
      break;
    case TokenType::IntLiteral:
      node->scalar_type_ = AstScalarLiteral::IntLit;
      (void)std::stoull(*val_);
      node->value_       = std::move(*val_);
      break;
    case TokenType::BooleanLiteral:
      node->scalar_type_ = AstScalarLiteral::BoolLit;
      node->value_       = std::move(*val_);
      break;
    case TokenType::NullLiteral:
      node->scalar_type_ = AstScalarLiteral::NullLit;
      break;
    case TokenType::ByteLiteral:
      node->scalar_type_ = AstScalarLiteral::U8Lit;
      node->value_       = TRY(unescape_quoted_string(*val_));
      break;
    case TokenType::HexLiteral:
      node->scalar_type_ = AstScalarLiteral::IntLit;
      converted          = std::stoull(*val_, nullptr, 16);
      node->value_       = std::to_string(converted);
      break;
    case TokenType::OctalLiteral:
      node->scalar_type_ = AstScalarLiteral::IntLit;
      converted          = std::stoull(*val_, nullptr, 8);
      node->value_       = std::to_string(converted);
      break;
    case TokenType::StringLiteral:
      node->scalar_type_ = AstScalarLiteral::StringLit;
      node->value_       = TRY(unescape_quoted_string(*val_));
      break;
    default:
      PANIC("parse_scalar_lit_: unknown literal kind.");
    }
  }
  catch(const std::out_of_range&) {
    return Error(ErrC::BadToken, "Literal value is too large.");
  }
  catch(...) {
    return Error(ErrC::BadToken, "Invalid literal token.");
  }

  ctx.lxr.consume(1);
  return Result<AstNode::Ptr<>>::create(std::move(node));
}

auto parse_aggregate_lit_(ParseContext &ctx) -> Result<AstNode::Ptr<>> {
  ASSERT(ctx.lxr.current() == TokenType::LeftBrace);

  auto node = AstNode::create<AstAggregateLiteral>(
    ctx.lxr.current().pos_,
    ctx.lxr.current().line_,
    nullptr,
    ctx.lxr.file_name_
  );

  ctx.lxr.consume(1);
  while(ctx.lxr.current() != TokenType::RightBrace) {
    const auto curr = ctx.lxr.current();
    auto child      = TRY(parse_begin_(ctx, true, false));
    child->parent_  = node.get();

    node->children_.emplace_back(std::move(child));
    if(!is_valid_subexpression_(child)) {
      ctx.lxr.revert_before(curr);
      return Error{ErrC::BadExpr, "Invalid subexpression within aggregate literal."};
    }

    if(ctx.lxr.current() == TokenType::Comma)
      ctx.lxr.consume(1);
  }

  ctx.lxr.consume(1);
  return Result<AstNode::Ptr<>>::create(std::move(node));
}

auto parse_parens_(ParseContext& ctx) -> Result<AstNode::Ptr<>>{
  ASSERT(ctx.lxr.current() == TokenType::LeftParen);
  ++ctx.paren_level;
  ctx.lxr.consume(1);

  const auto curr = ctx.lxr.current();
  auto expr = TRY(parse_begin_(ctx, true, false));

  if(!is_valid_subexpression_(expr)) {
    ctx.lxr.revert_before(curr);
    return Error{ErrC::BadExpr, "Expression cannot be used inside of parentheses."};
  }

  return Result<AstNode::Ptr<>>::create(std::move(expr));
}

auto parse_punctuator_(ParseContext& ctx) -> Result<AstNode::Ptr<>> {
  const auto curr = ctx.lxr.current();

  switch(curr.type_.value) {
  case TokenType::At:        return parse_directive_(ctx);
  case TokenType::LeftBrace: return parse_aggregate_lit_(ctx);
  case TokenType::LeftParen: return parse_parens_(ctx);
  default: break;
  }

  return Error{ErrC::BadToken, "Unexpected token."};
}

auto parse_directive_(ParseContext& ctx) -> Result<AstNode::Ptr<>> {
  return Error{ErrC::NotImplimented};
}

auto parse_namespacedecl_(ParseContext& ctx) -> Result<AstNode::Ptr<>> {
  const Token begin = MUST(ctx.lxr.expect_type(TokenType::Namespace));
  const Token ident = TRY(ctx.lxr.expect_type(TokenType::Identifier, false));
  const std::string name = MUST(ident.value(ctx.lxr));

  const auto curr_ent = ctx.entities.find(ctx.curr_namespace);
  bool existed_before = false;
  Entity::ID old_id = ctx.curr_namespace;

  /// Check if the entity already exists
  for(const Entity::ID id : curr_ent->chldrn_) {
    const auto child = ctx.entities.find(id);
    if(child->lname_ == name) {
      existed_before = true;
      if(child->type_ == EntityType::PlaceHolder) {
        TRY(ctx.entities.swap_placeholder<Static>(
          child->id_,
          ctx.curr_namespace,
          begin.pos_,
          begin.line_,
          ctx.lxr.file_name_));
      }

      ctx.curr_namespace = child->id_;
      break;
    }
  }

  /// Create new namespace
  if(!existed_before) {
    const auto ns = ctx.entities.insert<Static>(
      ctx.curr_namespace,
      begin.pos_,
      begin.line_,
      ctx.lxr.file_name_,
      name);
    ctx.curr_namespace = ns->id_;
  }

  /// At this point, ctx.curr_namespace has been altered.
  /// Create an AST node and parse the body of the namespace,
  /// restore the old ctx.curr_namespace afterwards.

  ctx.lxr.consume(1);
  TRY(ctx.lxr.expect_type(TokenType::LeftBrace));

  auto node = AstNode::create<AstNamespace>(
    begin.pos_,
    begin.line_,
    nullptr,
    ctx.lxr.file_name_);

  node->id_ = ctx.curr_namespace;
  while(!ctx.on_type(TokenType::RightBrace)) {
    const auto curr = ctx.lxr.current();
    auto child      = TRY(parse_begin_(ctx, false, false));
    child->parent_  = node.get();

    if(!is_node_toplevel_valid_(child)) {
      ctx.lxr.revert_before(curr);
      return Error{ErrC::BadExpr, "Expression is invalid at the toplevel."};
    }

    node->body_.emplace_back(std::move(child));
  }

  ctx.lxr.consume(1);
  ctx.curr_namespace = old_id;
  return Result<AstNode::Ptr<>>::create(std::move(node));
}

auto parse_procdecl_(ParseContext& ctx) -> Result<AstNode::Ptr<> > {
  ASSERT(ctx.on_type(TokenType::Proc));
  return Error{ErrC::NotImplimented};
}

auto parse_keyword_(ParseContext& ctx) -> Result<AstNode::Ptr<>> {
  const auto curr = ctx.lxr.current();

  ASSERT(ctx.on(TokenCategory::Keyword));
  switch(curr.type_.value) {
  case TokenType::Proc:      return parse_procdecl_(ctx);
  case TokenType::Namespace: return parse_namespacedecl_(ctx);
  default: /* TODO */ break;
  }

  UNREACHABLE_ASSERTION;
}

auto parse_unary_prefix_(ParseContext &ctx) -> Result<AstNode::Ptr<>> {
  ASSERT(ctx.on(TokenCategory::UnaryOp));
  const auto begin = ctx.lxr.current();
  ctx.lxr.consume(1);

  auto node = AstNode::create<AstUnaryExpr>(
    begin.pos_,
    begin.line_,
    nullptr,
    ctx.lxr.file_name_
  );

  node->op_type_    = begin.type_;
  node->op_cat_     = begin.cat_;
  node->is_postfix_ = false;
  node->operand_    = TRY(parse_begin_(ctx, true, true));

  node->operand_->parent_ = node.get();
  if(!is_valid_subexpression_(node->operand_)) {
    ctx.lxr.revert_before(begin);
    return Error{ErrC::BadExpr, "Unexpected expression following unary operator."};
  }

  return Result<AstNode::Ptr<>>::create(std::move(node));
}

auto parse_subscript_(ParseContext &ctx, AstNode::Ptr<>&& operand) -> Result<AstNode::Ptr<>> {
  return Error{ErrC::NotImplimented};
}

auto parse_postfix_(ParseContext& ctx, AstNode::Ptr<>&& operand) -> Result<AstNode::Ptr<>> {
  auto curr = ctx.lxr.current();

  switch(curr.type_.value) {
  case TokenType::LeftParen: return parse_call_(ctx, std::move(operand));
  case TokenType::Dec: FALLTHROUGH_;
  case TokenType::Inc: {
    auto node = AstNode::create<AstUnaryExpr>(
      curr.pos_,
      curr.line_,
      nullptr,
      ctx.lxr.file_name_);

    node->op_type_    = curr.type_;
    node->op_cat_     = curr.cat_;
    node->is_postfix_ = true;
    node->operand_    = std::move(operand);
    node->operand_->parent_ = node.get();

    ctx.lxr.consume(1);
    return Result<AstNode::Ptr<>>::create(std::move(node));
  }

  default: break; /// I might be forgetting some...
  }

  UNREACHABLE_ASSERTION;
}

auto parse_call_(ParseContext& ctx, AstNode::Ptr<>&& operand) -> Result<AstNode::Ptr<>> {
  auto curr = ctx.lxr.current();
  ASSERT(curr == TokenType::LeftParen);

  auto node = AstNode::create<AstCall>(
    curr.pos_,
    curr.line_,
    nullptr,
    ctx.lxr.file_name_);

  node->target_ = std::move(operand);
  ctx.lxr.consume(1);

  if(ctx.lxr.current() == TokenType::RightParen) {
    ctx.lxr.consume(1);
    return Result<AstNode::Ptr<>>::create(std::move(node));
  }

  const uint16_t old_paren_lvl = ctx.paren_level++;
  while(old_paren_lvl < ctx.paren_level) {
    curr = ctx.lxr.current();
    auto expr = TRY(parse_begin_(ctx, true, false));
    if(!is_valid_subexpression_(expr)) {
      ctx.lxr.revert_before(curr);
      return Error{ErrC::BadExpr, "Invalid subexpression within call."};
    }

    node->arguments_.emplace_back(std::move(expr));
    if(old_paren_lvl >= ctx.paren_level) {
      break;
    }

    if(ctx.lxr.current().is_terminator()) {
      ctx.lxr.consume(1);
      if(ctx.lxr.current() == TokenType::RightParen) {
        --ctx.paren_level;
        ctx.lxr.consume(1);
      }
    }
  }

  return Result<AstNode::Ptr<>>::create(std::move(node));
}

auto parse_identifier_(ParseContext &ctx) -> Result<AstNode::Ptr<>> {
  auto curr = ctx.lxr.current();
  ASSERT(curr == TokenType::Identifier);

  auto node = AstNode::create<AstEntityRefThunk>(
    curr.pos_,
    curr.line_,
    nullptr,
    ctx.lxr.file_name_);

  auto val = curr.value(ctx.lxr);
  ASSERT(val.has_value());

  node->name_ = std::move(val.value());
  ctx.lxr.consume(1);
  return Result<AstNode::Ptr<>>::create(std::move(node));
}

auto get_next_include_(ParseContext& ctx) -> bool {
  if (ctx.includes_.empty()) {
    return false;
  }
  
  auto next = std::ranges::find_if(ctx.includes_, [](const IncludedFile& f) {
    return f.state_ == IncludeState::Pending;
  });

  if(next == ctx.includes_.end())
    return false;

  /// ts is so fucking retarded 🥀💔
  std::filesystem::path path(next->name_);

#ifdef N19_WIN32
  auto file = sys::File::open(path.wstring());
#else /// POSIX
  auto file = sys::File::open(path.string());
#endif

  if(!file.has_value()) {
    ctx.errstream
      << Con::RedFG
      << "\nError:"
      << Con::Reset
      << " could not open included file "
      << next->name_
      << ".\n\n";
    return false;
  }

  next->state_       = IncludeState::Finished;
  ctx.curr_namespace = N19_ROOT_ENTITY_ID;
  ctx.paren_level    = 0;

  ASSERT(ctx.lxr.reset(*file));
  return true;
}

END_NAMESPACE(n19::detail_);
BEGIN_NAMESPACE(n19);

auto parse(ParseContext& ctx) -> bool {
  return detail_::parse_impl_(ctx);
}

END_NAMESPACE(n19);
