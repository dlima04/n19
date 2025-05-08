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

namespace detail_ {
  enum class IncludeState : uint8_t {
    Pending  = 0, /// File needs to be parsed.
    Finished = 1, /// File has already been parsed.
  };

  struct IncludedFile {
    std::string name_;
    IncludeState state_ = IncludeState::Pending;
  };
}

struct ParseContext {
  Entity::ID      curr_namespace;
  OStream&        errstream;
  ErrorCollector& errors;
  Lexer&          lxr;
  uint16_t        paren_level_;
  EntityTable&    entities;

  std::vector<detail_::IncludedFile> includes_;
  std::vector<AstNode::Ptr<>> toplevel_decls_;

  ParseContext(
    OStream& errstream,
    ErrorCollector& errors,
    Lexer& lxr,
    EntityTable& entities
  ) : curr_namespace(N19_ROOT_ENTITY_ID)
    , errstream(errstream)
    , errors(errors)
    , lxr(lxr)
    , paren_level_(0)
    , entities(entities)
  {
    ASSERT(!lxr.src_.empty());
    ASSERT(!entities.map_.empty());
  }

  ~ParseContext() = default;
};

END_NAMESPACE(n19);
#endif //N19_PARSECONTEXT_HPP
