/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <n19/Core/Common.hpp>
#include <n19/Core/Panic.hpp>
#include <n19/Frontend/Diagnostics/ErrorCollector.hpp>
#include <n19/Frontend/Lexer/Lexer.hpp>
#include <n19/Frontend/Entities/EntityTable.hpp>
#include <n19/Frontend/AST/ASTNodes.hpp>
#include <n19/Core/Stream.hpp>
#include <string>
#include <cstdint>
BEGIN_NAMESPACE(rl);

using namespace n19;

struct ParseContext {
  InputFile::ID   curr_file;
  Entity::ID      curr_namespace;
  OStream&        errstream;
  ErrorCollector& errors;
  Lexer&          lxr;
  uint16_t        paren_level;
  EntityTable&    entities;

  std::vector<AstNode::Ptr<>> toplevel_decls_;

  ParseContext(
    InputFile::ID inf_id,
    OStream& errstream,
    ErrorCollector& errors,
    Lexer& lxr,
    EntityTable& entities
  ) : curr_file(inf_id)
    , curr_namespace(RL_ROOT_ENTITY_ID)
    , errstream(errstream)
    , errors(errors)
    , lxr(lxr)
    , paren_level(0)
    , entities(entities)
  {
    ASSERT(!lxr.src_.empty());
    ASSERT(!entities.map_.empty());
  }

  bool on(TokenCategory cat) { return lxr.current().cat_.isa(cat); }
  bool on_type(TokenType ty) { return lxr.current() == ty; }

  ~ParseContext() = default;
};

END_NAMESPACE(rl);
