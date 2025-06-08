/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef N19_PARSECONTEXT_HPP
#define N19_PARSECONTEXT_HPP
#include <Core/Panic.hpp>
#include <Frontend/ErrorCollector.hpp>
#include <Frontend/Lexer.hpp>
#include <Frontend/EntityTable.hpp>
#include <Frontend/AstNodes.hpp>
#include <IO/Stream.hpp>
#include <string>
#include <cstdint>
BEGIN_NAMESPACE(n19);

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
    , curr_namespace(N19_ROOT_ENTITY_ID)
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

END_NAMESPACE(n19);
#endif //N19_PARSECONTEXT_HPP
