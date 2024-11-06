#ifndef ASTNODES_H
#define ASTNODES_H
#include <Token.h>
#include <TypeDescriptor.h>
#include <vector>
#include <memory>
#include <concepts>
#include <type_traits>
#include <optional>

#define N19_ASTNODE_TYPE_LIST   \
  X(Node)                       \
  X(Vardecl)                    \
  X(ProcDecl)                   \
  X(EntityRef)                  \
  X(EntityRefThunk)             \
  X(TypeRef)                    \
  X(TypeRefThunk)               \
  X(ScalarLiteral)              \
  X(AggregateLiteral)           \
  X(BinExpr)                    \
  X(UnaryExpr)                  \
  X(Branch)                     \
  X(Switch)                     \
  X(Case)                       \
  X(Default)                    \
  X(For)                        \
  X(While)                      \
  X(ConstBranch)                \
  X(ScopeBlock)                 \
  X(NamespaceBlock)             \
  X(Call)                       \
  X(Break)                      \
  X(Continue)                   \
  X(Return)                     \
  X(Defer)                      \
  X(DeferIf)                    \
  X(Sizeof)                     \
  X(Subscript)                  \

namespace n19 {
  #define X(NAME) class Ast##NAME;
  N19_ASTNODE_TYPE_LIST
  #undef X

  //
  // IsAstNode:
  // Constrain T to being an AstNode,
  // or a derived class of AstNode.
  //
  template<typename T>
  concept IsAstNode =
    std::derived_from<T, AstNode> ||
    std::is_same_v<T, AstNode>;
}

class n19::AstNode {
public:
  //
  // An enum to avoid unnecessary
  // dynamic_casts to check if an AstNode*
  // is actually derived.
  //
  #define X(NAME) NAME,
  enum class Type : uint16_t {
    N19_ASTNODE_TYPE_LIST
  };
  #undef X

  //
  // Pointer to an AstNode.
  // For polymorphism in containers.
  //
  template<typename T = AstNode> requires IsAstNode<T>
  using Ptr = std::shared_ptr<T>;

  //
  // The default container for storing child
  // nodes inside of parent AstNodes.
  // we can change this later if need be.
  //
  template<typename T = AstNode> requires IsAstNode<T>
  using Children = std::vector<Ptr<T>>;

  //
  // The public members of AstNode.
  // we need to store a pointer to the node's parent,
  // as well as some necessary info such as the file it
  // belongs to and it's location.
  //
  std::weak_ptr<AstNode> parent_;
  size_t pos_;
  uint32_t line_;
  std::string file_;
  Type type_;

  //
  // Default virtual destructor so that
  // we get a Vtable for this...
  //
  virtual ~AstNode() = default;
  AstNode(
    const size_t pos,
    const uint32_t line,
    const std::string &file,
    const Type type
  ) : pos_(pos),
    line_(line),
    file_(file),
    type_(type) {}
};

class n19::AstBinExpr final : public AstNode {
public:
  TokenType op_type_    = TokenType::None;
  TokenCategory op_cat_ = 0;
  AstNode::Ptr<> left_  = nullptr;
  AstNode::Ptr<> right_ = nullptr;

  ~AstBinExpr() override = default;
  AstBinExpr(
    const size_t pos,
    const uint32_t line,
    const std::string& file
  ) : AstNode(pos, line, file, Type::BinExpr) {}
};

class n19::AstUnaryExpr final : public AstNode {
public:
  TokenType op_type_      = TokenType::None;
  TokenCategory op_cat_   = 0;
  AstNode::Ptr<> operand_ = nullptr;
  bool is_postfix         = false; // only relevant for '--' and '++'

  ~AstUnaryExpr() override = default;
  AstUnaryExpr(
    const size_t pos,
    const uint32_t line,
    const std::string& file
  ) : AstNode(pos, line, file, Type::UnaryExpr) {}
};

class n19::AstScalarLiteral final : public AstNode {
public:
  std::string value_;
  enum : uint8_t {
    None,
    NullLit,
    IntLit,
    StringLit,
    U8Lit,
    FloatLit,
  } type_ = None;

  ~AstScalarLiteral() override = default;
  AstScalarLiteral(
    const size_t pos,
    const uint32_t line,
    const std::string& file
  ) : AstNode(pos, line, file, Type::ScalarLiteral) {}
};

class n19::AstAggregateLiteral final : public AstNode {
public:
  AstNode::Children<> children_;

  ~AstAggregateLiteral() override = default;
  AstAggregateLiteral(
    const size_t pos,
    const uint32_t line,
    const std::string& file
  ) : AstNode(pos, line, file, Type::AggregateLiteral) {}
};

class n19::AstEntityRef final : public AstNode {
public:
  Entity::ID id_= N19_INVALID_ENTITY_ID;

  ~AstEntityRef() override = default;
  AstEntityRef(
    const size_t pos,
    const uint32_t line,
    const std::string& file
  ) : AstNode(pos, line, file, Type::EntityRef) {}
};

class n19::AstEntityRefThunk final : public AstNode {
public:
  std::vector<std::string> name_;

  ~AstEntityRefThunk() override = default;
  AstEntityRefThunk(
    const size_t pos,
    const uint32_t line,
    const std::string& file
  ) : AstNode(pos, line, file, Type::EntityRefThunk) {}
};

class n19::AstTypeRef final : public AstNode {
public:
  TypeDescriptor descriptor_;

  ~AstTypeRef() override = default;
  AstTypeRef(
    const size_t pos,
    const uint32_t line,
    const std::string& file
  ) : AstNode(pos, line, file, Type::TypeRef) {}
};

class n19::AstTypeRefThunk final : public AstNode {
public:
  TypeDescriptorThunk descriptor_;

  ~AstTypeRefThunk() override = default;
  AstTypeRefThunk(
    const size_t pos,
    const uint32_t line,
    const std::string& file
  ) : AstNode(pos, line, file, Type::TypeRefThunk) {}
};

class n19::AstBranch final : public AstNode {
public:
  AstNode::Ptr<> if_   = nullptr; // If condition.
  AstNode::Ptr<> then_ = nullptr; // If block.
  AstNode::Ptr<> else_ = nullptr; // Can be null!

  ~AstBranch() override = default;
  AstBranch(
    const size_t pos,
    const uint32_t line,
    const std::string& file
  ) : AstNode(pos, line, file, Type::Branch) {}
};

class n19::AstConstBranch final : public AstNode {
public:
  AstNode::Ptr<> where_     = nullptr; // Where condition.
  AstNode::Ptr<> then_      = nullptr; // Where block.
  AstNode::Ptr<> otherwise_ = nullptr; // Can be null!

  ~AstConstBranch() override = default;
  AstConstBranch(
    const size_t pos,
    const uint32_t line,
    const std::string& file
  ) : AstNode(pos, line, file, Type::ConstBranch) {}
};

class n19::AstCase final : public AstNode {
public:
  bool is_fallthrough = false;
  AstNode::Ptr<> value_ = nullptr;
  AstNode::Children<> children_;

  ~AstCase() override = default;
  AstCase(
    const size_t pos,
    const uint32_t line,
    const std::string& file
  ) : AstNode(pos, line, file, Type::Case) {}
};

class n19::AstDefault final : public AstNode {
public:
  AstNode::Children<> children_;

  ~AstDefault() override = default;
  AstDefault(
    const size_t pos,
    const uint32_t line,
    const std::string& file
  ) : AstNode(pos, line, file, Type::Default) {}
};

class n19::AstSwitch final : public AstNode {
public:
  AstNode::Ptr<> target_         = nullptr;
  AstNode::Ptr<AstDefault> dflt_ = nullptr;
  AstNode::Children<AstCase> cases_;

  ~AstSwitch() override = default;
  AstSwitch(
    const size_t pos,
    const uint32_t line,
    const std::string& file
  ) : AstNode(pos, line, file, Type::Switch) {}
};

class n19::AstScopeBlock final : public AstNode {
public:
  AstNode::Children<> children_;

  ~AstScopeBlock() override = default;
  AstScopeBlock(
    const size_t pos,
    const uint32_t line,
    const std::string& file
  ) : AstNode(pos, line, file, Type::ScopeBlock) {}
};

class n19::AstCall final : public AstNode {
public:
  AstNode::Ptr<> target_ = nullptr;
  AstNode::Children<> arguments_;

  ~AstCall() override = default;
  AstCall(
    const size_t pos,
    const uint32_t line,
    const std::string& file
  ) : AstNode(pos, line, file, Type::Call) {}
};

class n19::AstDefer final : public AstNode {
public:
  AstNode::Ptr<> call_ = nullptr;       // Should ALWAYS be AstCall under the hood

  ~AstDefer() override = default;
  AstDefer(
    const size_t pos,
    const uint32_t line,
    const std::string& file
  ) : AstNode(pos, line, file, Type::Defer) {}
};

class n19::AstDeferIf final : public AstNode {
public:
  AstNode::Ptr<> call_ = nullptr;       // Should ALWAYS be AstCall under the hood
  AstNode::Ptr<> condition_ = nullptr;  // The condition on which we call this.

  ~AstDeferIf() override = default;
  AstDeferIf(
    const size_t pos,
    const uint32_t line,
    const std::string& file
  ) : AstNode(pos, line, file, Type::DeferIf) {}
};

class n19::AstVardecl final : public AstNode {
public:
  AstNode::Ptr<> name_ = nullptr;  // EntityRef or EntityRefThunk
  AstNode::Ptr<> type_ = nullptr;  // TypeRef or TypeRefThunk

  ~AstVardecl() override = default;
  AstVardecl(
    const size_t pos,
    const uint32_t line,
    const std::string& file
  ) : AstNode(pos, line, file, Type::Vardecl) {}
};

class n19::AstProcDecl final : public AstNode {
public:
  AstNode::Ptr<> name_ = nullptr; // EntityRef or EntityRefThunk
  AstNode::Children<> arg_decls_; // The parameter declarations (if any)
  AstNode::Children<> body_;      // The body of the procedure

  ~AstProcDecl() override = default;
  AstProcDecl(
    const size_t pos,
    const uint32_t line,
    const std::string& file
  ) : AstNode(pos, line, file, Type::ProcDecl) {}
};

class n19::AstReturn final : public AstNode {
public:
  AstNode::Ptr<> value_ = nullptr; // Can be null!

  ~AstReturn() override = default;
  AstReturn(
    const size_t pos,
    const uint32_t line,
    const std::string& file
  ) : AstNode(pos, line, file, Type::Return) {}
};

class n19::AstBreak final : public AstNode {
public:
  ~AstBreak() override = default;
  AstBreak(
    const size_t pos,
    const uint32_t line,
    const std::string& file
  ) : AstNode(pos, line, file, Type::Break) {}
};

class n19::AstContinue final : public AstNode {
public:
  ~AstContinue() override = default;
  AstContinue(
    const size_t pos,
    const uint32_t line,
    const std::string& file
  ) : AstNode(pos, line, file, Type::Continue) {}
};

class n19::AstFor final : public AstNode {
public:
  AstNode::Children<> body_;
  AstNode::Ptr<> init_    = nullptr; // Can be null!
  AstNode::Ptr<> update_  = nullptr; // Can be null!
  AstNode::Ptr<> cond_    = nullptr; // Can be null!

  ~AstFor() override = default;
  AstFor(
    const size_t pos,
    const uint32_t line,
    const std::string& file
  ) : AstNode(pos, line, file, Type::For) {}
};

class n19::AstWhile final : public AstNode {
public:
  AstNode::Children<> body_;
  AstNode::Ptr<> cond_ = nullptr; // The loop condition
  bool is_dowhile      = false;   // If true: the loop is a do-while.

  ~AstWhile() override = default;
  AstWhile(
    const size_t pos,
    const uint32_t line,
    const std::string& file
  ) : AstNode(pos, line, file, Type::While) {}
};

class n19::AstSubscript final : public AstNode {
public:
  AstNode::Ptr<> operand_ = nullptr; // The thing being subscripted.
  AstNode::Ptr<> value_   = nullptr; // The index value.

  ~AstSubscript() override = default;
  AstSubscript(
    const size_t pos,
    const uint32_t line,
    const std::string& file
  ) : AstNode(pos, line, file, Type::Subscript) {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif //ASTNODES_H
