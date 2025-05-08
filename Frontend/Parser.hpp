/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef N19_HIR_PARSER_HPP
#define N19_HIR_PARSER_HPP
#include <Core/Result.hpp>
#include <Core/Try.hpp>
#include <Frontend/ParseContext.hpp>
#include <Frontend/AstNodes.hpp>

///
/// Public parsing functions
BEGIN_NAMESPACE(n19);
auto parse(ParseContext& ctx) -> bool;
END_NAMESPACE(n19);

///
/// Internal parsing functions
BEGIN_NAMESPACE(n19::detail_);

auto parse_begin_(
  ParseContext& ctx,
  bool nocheck_term,
  bool parse_single
) -> Result<AstNode::Ptr<>>;

/// Utility
auto get_next_include_(ParseContext&) -> bool;
auto is_node_toplevel_valid_(const AstNode::Ptr<>&) -> bool;
bool parse_impl_(ParseContext&);

///
/// Node producers
Result<AstNode::Ptr<>> parse_starting_punctuator_(ParseContext&);
Result<AstNode::Ptr<>> parse_scalar_lit_(ParseContext&);
Result<AstNode::Ptr<>> parse_aggregate_lit_(ParseContext&);
Result<AstNode::Ptr<>> parse_keyword_(ParseContext&);
Result<AstNode::Ptr<>> parse_unary_prefix_(ParseContext&);

Result<AstNode::Ptr<>> parse_subscript_(ParseContext&, AstNode::Ptr<>&&);
Result<AstNode::Ptr<>> parse_call_(ParseContext&, AstNode::Ptr<>&&);
Result<AstNode::Ptr<>> parse_binary_expression_(ParseContext&, AstNode::Ptr<>&&);
Result<AstNode::Ptr<>> parse_identifier_(ParseContext&, AstNode::Ptr<>&&);

END_NAMESPACE(n19::detail_);
#endif //N19_HIR_PARSER_HPP
