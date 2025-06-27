/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <Core/Common.hpp>
#include <Core/Platform.hpp>
#include <Core/Maybe.hpp>
#include <Core/Result.hpp>
#include <Core/Console.hpp>
#include <Core/Concepts.hpp>
#include <Core/Poly.hpp>
#include <Frontend/Lexer/Token.hpp>
#include <Frontend/Entities/Entity.hpp>
#include <Frontend/FrontendContext.hpp>
#include <System/String.hpp>
#include <vector>
#include <memory>

#define RL_ASTNODE_TYPE_LIST   \
  ASTNODE_X(Node)              \
  ASTNODE_X(Vardecl)           \
  ASTNODE_X(ProcDecl)          \
  ASTNODE_X(EntityRef)         \
  ASTNODE_X(EntityRefThunk)    \
  ASTNODE_X(QualifiedRef)      \
  ASTNODE_X(ScalarLiteral)     \
  ASTNODE_X(AggregateLiteral)  \
  ASTNODE_X(BinExpr)           \
  ASTNODE_X(UnaryExpr)         \
  ASTNODE_X(Branch)            \
  ASTNODE_X(If)                \
  ASTNODE_X(Else)              \
  ASTNODE_X(Switch)            \
  ASTNODE_X(Case)              \
  ASTNODE_X(Default)           \
  ASTNODE_X(For)               \
  ASTNODE_X(While)             \
  ASTNODE_X(ConstBranch)       \
  ASTNODE_X(ConstIf)           \
  ASTNODE_X(ConstElse)         \
  ASTNODE_X(ScopeBlock)        \
  ASTNODE_X(Namespace)         \
  ASTNODE_X(Call)              \
  ASTNODE_X(Break)             \
  ASTNODE_X(Continue)          \
  ASTNODE_X(Return)            \
  ASTNODE_X(Defer)             \
  ASTNODE_X(DeferIf)           \
  ASTNODE_X(Subscript)         \

BEGIN_NAMESPACE(rl);
using namespace n19;

class AstNode {
public:
  #define ASTNODE_X(NAME) NAME,
  enum class Type : uint16_t {
    RL_ASTNODE_TYPE_LIST
  };
  #undef ASTNODE_X

  // Type aliases //
  //////////////////////////////////////////
  template<typename T = AstNode>
  using Ptr = std::unique_ptr<T>;

  template<typename T = AstNode>
  using Children = std::vector<Ptr<T>>;

  auto print_(
    uint32_t depth,
    OStream& stream,
    const std::string& node_name
  ) const -> void;

  virtual auto print(
    uint32_t depth,
    OStream& stream,
    const Maybe<std::string> &alias
  ) const -> void = 0;

  template<typename T>
  static auto create(
    size_t pos,
    uint32_t line,
    AstNode* parent,
    InputFile::ID file
  ) -> Ptr<T>;

  // Public fields //
  //////////////////////////////////////////
  AstNode* parent_ = nullptr;
  size_t pos_      = 0;
  uint32_t line_   = 1;
  InputFile::ID file_ = RL_INVALID_INFILE_ID;
  Type type_;
  //////////////////////////////////////////

  virtual ~AstNode() = default;
};

class AstBinExpr final : public AstNode {
public:
  TokenType op_type_    = TokenType::None;
  TokenCategory op_cat_ = TokenCategory::NonCategorical;
  AstNode::Ptr<> left_  = nullptr;
  AstNode::Ptr<> right_ = nullptr;

  auto print(uint32_t depth,
    OStream& stream,
    const Maybe<std::string> &alias
  ) const -> void override;
  
  ~AstBinExpr() override = default;
  AstBinExpr() = default;
};

class AstUnaryExpr final : public AstNode {
public:
  TokenType op_type_      = TokenType::None;
  TokenCategory op_cat_   = TokenCategory::NonCategorical;
  AstNode::Ptr<> operand_ = nullptr;
  bool is_postfix_        = false; // only relevant for '--' and '++'

  auto print(uint32_t depth,
    OStream& stream,
    const Maybe<std::string> &alias
  ) const -> void override;

  ~AstUnaryExpr() override = default;
  AstUnaryExpr() = default;
};

class AstScalarLiteral final : public AstNode {
public:
  std::string value_;
  enum : uint8_t {
    None,
    NullLit,
    IntLit,
    StringLit,
    U8Lit,
    FloatLit,
    BoolLit,
  } scalar_type_ = None;

  auto print(uint32_t depth,
    OStream& stream,
    const Maybe<std::string> &alias
  ) const -> void override;

  ~AstScalarLiteral() override = default;
  AstScalarLiteral() = default;
};

class AstAggregateLiteral final : public AstNode {
public:
  AstNode::Children<> children_;

  auto print(uint32_t depth,
    OStream& stream,
    const Maybe<std::string> &alias
  ) const -> void override;

  ~AstAggregateLiteral() override = default;
  AstAggregateLiteral() = default;
};

class AstEntityRef final : public AstNode {
public:
  Entity::ID id_= RL_INVALID_ENTITY_ID;

  auto print(uint32_t depth,
    OStream& stream,
    const Maybe<std::string> &alias
  ) const -> void override;

  ~AstEntityRef() override = default;
  AstEntityRef() = default;
};

class AstEntityRefThunk final : public AstNode {
public:
  std::string name_;

  auto print(uint32_t depth,
    OStream& stream,
    const Maybe<std::string> &alias
  ) const -> void override;

  ~AstEntityRefThunk() override = default;
  AstEntityRefThunk() = default;
};

class AstQualifiedRef final : public AstNode {
public:
  EntityQualifier descriptor_;

  auto print(uint32_t depth,
    OStream& stream,
    const Maybe<std::string> &alias
  ) const -> void override;

  ~AstQualifiedRef() override = default;
  AstQualifiedRef() = default;
};

class AstIf final : public AstNode {
public:
  AstNode::Children<> body_;
  AstNode::Ptr<> condition_ = nullptr;

  auto print(uint32_t depth,
    OStream& stream,
    const Maybe<std::string> &alias
  ) const -> void override;

  ~AstIf() override = default;
  AstIf() = default;
};

class AstElse final : public AstNode {
public:
  AstNode::Children<> body_;

  auto print(uint32_t depth,
    OStream& stream,
    const Maybe<std::string> &alias
  ) const -> void override;

  ~AstElse() override = default;
  AstElse() = default;
};

class AstNamespace final : public AstNode {
public:
  AstNode::Children<> body_;
  Entity::ID id_ = RL_INVALID_ENTITY_ID;

  auto print(uint32_t depth,
    OStream& stream,
    const Maybe<std::string> &alias
  ) const -> void override;

  ~AstNamespace() override = default;
  AstNamespace() = default;
};

class AstConstIf final : public AstNode {
public:
  AstNode::Children<> body_;
  AstNode::Ptr<> condition_ = nullptr;

  auto print(uint32_t depth,
    OStream& stream,
    const Maybe<std::string> &alias
  ) const -> void override;

  ~AstConstIf() override = default;
  AstConstIf() = default;
};

class AstConstElse final : public AstNode {
public:
  AstNode::Children<> body_;

  auto print(uint32_t depth,
    OStream& stream,
    const Maybe<std::string> &alias
  ) const -> void override;

  ~AstConstElse() override = default;
  AstConstElse() = default;
};

class AstBranch final : public AstNode {
public:
  AstNode::Ptr<AstIf> if_     = nullptr; // If condition + block
  AstNode::Ptr<AstElse> else_ = nullptr; // Can be null!

  auto print(uint32_t depth,
    OStream& stream,
    const Maybe<std::string> &alias
  ) const -> void override;

  ~AstBranch() override = default;
  AstBranch() = default;
};

class AstConstBranch final : public AstNode {
public:
  AstNode::Ptr<AstConstIf> if_ = nullptr;
  AstNode::Ptr<AstConstElse> else_ = nullptr; // Can be null!

  auto print(uint32_t depth,
    OStream& stream,
    const Maybe<std::string> &alias
  ) const -> void override;

  ~AstConstBranch() override = default;
  AstConstBranch() = default;
};

class AstCase final : public AstNode {
public:
  bool is_fallthrough = false;
  AstNode::Ptr<> value_ = nullptr;
  AstNode::Children<> children_;

  auto print(uint32_t depth,
    OStream& stream,
    const Maybe<std::string> &alias
  ) const -> void override;

  ~AstCase() override = default;
  AstCase() = default;
};

class AstDefault final : public AstNode {
public:
  AstNode::Children<> children_;

  auto print(uint32_t depth,
    OStream& stream,
    const Maybe<std::string> &alias
  ) const -> void override;

  ~AstDefault() override = default;
  AstDefault() = default;
};

class AstSwitch final : public AstNode {
public:
  AstNode::Ptr<> target_         = nullptr;
  AstNode::Ptr<AstDefault> dflt_ = nullptr;
  AstNode::Children<AstCase> cases_;

  auto print(uint32_t depth,
    OStream& stream,
    const Maybe<std::string> &alias
  ) const -> void override;

  ~AstSwitch() override = default;
  AstSwitch() = default;
};

class AstScopeBlock final : public AstNode {
public:
  AstNode::Children<> children_;

  auto print(uint32_t depth,
    OStream& stream,
    const Maybe<std::string> &alias
  ) const -> void override;

  ~AstScopeBlock() override = default;
  AstScopeBlock() = default;
};

class AstCall final : public AstNode {
public:
  AstNode::Ptr<> target_ = nullptr;
  AstNode::Children<> arguments_;

  auto print(uint32_t depth,
    OStream& stream,
    const Maybe<std::string> &alias
  ) const -> void override;

  ~AstCall() override = default;
  AstCall() = default;
};

class AstDefer final : public AstNode {
public:
  AstNode::Ptr<> call_ = nullptr;       // Should ALWAYS be AstCall under the hood

  auto print(uint32_t depth,
    OStream& stream,
    const Maybe<std::string> &alias
  ) const -> void override;

  ~AstDefer() override = default;
  AstDefer() = default;
};

class AstDeferIf final : public AstNode {
public:
  AstNode::Ptr<> call_ = nullptr;       // Should ALWAYS be AstCall under the hood
  AstNode::Ptr<> condition_ = nullptr;  // The condition on which we call this.

  auto print(uint32_t depth,
    OStream& stream,
    const Maybe<std::string> &alias
  ) const -> void override;

  ~AstDeferIf() override = default;
  AstDeferIf() = default;
};

class AstVardecl final : public AstNode {
public:
  AstNode::Ptr<> name_ = nullptr;  // EntityRef or EntityRefThunk
  AstNode::Ptr<> type_ = nullptr;  // TypeRef or TypeRefThunk

  auto print(uint32_t depth,
    OStream& stream,
    const Maybe<std::string> &alias
  ) const -> void override;

  ~AstVardecl() override = default;
  AstVardecl() = default;
};

class AstProcDecl final : public AstNode {
public:
  Entity::ID id_ = RL_INVALID_ENTITY_ID;
  AstNode::Children<> arg_decls_; // The parameter declarations (if any)
  AstNode::Children<> body_;      // The body of the procedure

  auto print(uint32_t depth,
    OStream& stream,
    const Maybe<std::string> &alias
  ) const -> void override;

  ~AstProcDecl() override = default;
  AstProcDecl() = default;
};

class AstReturn final : public AstNode {
public:
  AstNode::Ptr<> value_ = nullptr; // Can be null!

  auto print(uint32_t depth,
    OStream& stream,
    const Maybe<std::string> &alias
  ) const -> void override;

  ~AstReturn() override = default;
  AstReturn() = default;
};

class AstBreak final : public AstNode {
public:
  auto print(uint32_t depth,
    OStream& stream,
    const Maybe<std::string> &alias
  ) const -> void override;

  ~AstBreak() override = default;
  AstBreak() = default;
};

class AstContinue final : public AstNode {
public:
  auto print(uint32_t depth,
    OStream& stream,
    const Maybe<std::string> &alias
  ) const -> void override;
  ~AstContinue() override = default;
};

class AstFor final : public AstNode {
public:
  AstNode::Ptr<> body_    = nullptr;
  AstNode::Ptr<> init_    = nullptr; // Can be null!
  AstNode::Ptr<> update_  = nullptr; // Can be null!
  AstNode::Ptr<> cond_    = nullptr; // Can be null!

  auto print(uint32_t depth,
    OStream& stream,
    const Maybe<std::string> &alias
  ) const -> void override;

  ~AstFor() override = default;
  AstFor() = default;
};

class AstWhile final : public AstNode {
public:
  AstNode::Children<> body_;
  AstNode::Ptr<> cond_ = nullptr; // The loop condition
  bool is_dowhile      = false;   // If true: the loop is a do-while.

  auto print(uint32_t depth,
    OStream& stream,
    const Maybe<std::string> &alias
  ) const -> void override;

  ~AstWhile() override = default;
  AstWhile() = default;
};

class AstSubscript final : public AstNode {
public:
  AstNode::Ptr<> operand_ = nullptr; // The thing being subscripted.
  AstNode::Ptr<> value_   = nullptr; // The index value.

  auto print(uint32_t depth,
    OStream& stream,
    const Maybe<std::string> &alias
  ) const -> void override;

  ~AstSubscript() override = default;
  AstSubscript() = default;
};

///////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
auto AstNode::create(
  const size_t pos,
  const uint32_t line,
  AstNode* parent,
  const InputFile::ID file ) -> Ptr<T>
{
  auto ptr     = std::make_unique<T>();
  ptr->parent_ = parent;
  ptr->pos_    = pos;
  ptr->line_   = line;
  ptr->file_   = file;
  ptr->parent_ = parent;

  #define ASTNODE_X(NAME)              \
  if constexpr(IsSame<T, Ast##NAME>) { \
    ptr->type_ = Type::NAME;           \
  }

  RL_ASTNODE_TYPE_LIST
  #undef ASTNODE_X

  return ptr;
}

END_NAMESPACE(rl);