/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <Core/Result.hpp>
#include <Core/Try.hpp>
#include <Frontend/Parser/ParseContext.hpp>
#include <Frontend/AST/ASTNodes.hpp>

///
/// Public parsing functions
BEGIN_NAMESPACE(rl);
auto parse(ParseContext& ctx) -> bool;
END_NAMESPACE(rl);

///
/// Internal parsing functions
BEGIN_NAMESPACE(rl::detail_);

auto parse_begin_(
  ParseContext& ctx,
  bool nocheck_term,
  bool parse_single
) -> Result<AstNode::Ptr<>>;

/// Utility
auto get_next_include_(ParseContext&) -> bool;
auto is_node_toplevel_valid_(const AstNode::Ptr<>&)   -> bool;
auto node_never_needs_terminal_(const AstNode::Ptr<>&) -> bool;
auto is_valid_subexpression_(const AstNode::Ptr<>&)   -> bool;
bool parse_impl_(ParseContext&);

///
/// Node producers
auto parse_punctuator_(ParseContext&)    -> Result<AstNode::Ptr<>>;
auto parse_scalar_lit_(ParseContext&)    -> Result<AstNode::Ptr<>>;
auto parse_aggregate_lit_(ParseContext&) -> Result<AstNode::Ptr<>>;
auto parse_keyword_(ParseContext&)       -> Result<AstNode::Ptr<>>;
auto parse_unary_prefix_(ParseContext&)  -> Result<AstNode::Ptr<>>;
auto parse_parens_(ParseContext&)        -> Result<AstNode::Ptr<>>;
auto parse_directive_(ParseContext&)     -> Result<AstNode::Ptr<>>;
auto parse_identifier_(ParseContext&)    -> Result<AstNode::Ptr<>>;
auto parse_procdecl_(ParseContext&)      -> Result<AstNode::Ptr<>>;
auto parse_namespacedecl_(ParseContext&) -> Result<AstNode::Ptr<>>;
auto parse_deep_ident_(ParseContext&)    -> Result<Entity::ID>;

auto parse_scope_(ParseContext&)         -> Result<AstNode::Ptr<>>;
auto parse_ret_(ParseContext&)           -> Result<AstNode::Ptr<>>;
auto parse_cont_(ParseContext&)          -> Result<AstNode::Ptr<>>;
auto parse_qualtype_(ParseContext&)      -> Result<AstNode::Ptr<>>;
auto parse_break_(ParseContext&)         -> Result<AstNode::Ptr<>>;
auto parse_usingstmt_(ParseContext&)     -> Result<AstNode::Ptr<>>;

auto parse_postfix_(ParseContext&, AstNode::Ptr<>&&)    -> Result<AstNode::Ptr<>>;
auto parse_subscript_(ParseContext&, AstNode::Ptr<> &&) -> Result<AstNode::Ptr<>>;
auto parse_call_(ParseContext&, AstNode::Ptr<>&&)       -> Result<AstNode::Ptr<>>;
auto parse_binexpr_(ParseContext&, AstNode::Ptr<>&&)    -> Result<AstNode::Ptr<>>;

END_NAMESPACE(rl::detail_);