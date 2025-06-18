/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <Core/Result.hpp>
#include <Frontend/AST/ASTNodes.hpp>
#include <Frontend/Common/StringPool.hpp>
#include <Core/ClassTraits.hpp>
#include <variant>
#include <utility>
BEGIN_NAMESPACE(n19::interp);

#define N19_INTERP_OPCODE_LIST        \
  X(PushI)  /* Push immediate      */ \
  X(Add)    /* Addition            */ \
  X(Sub)    /* Subtraction         */ \
  X(Mul)    /* Multiplication      */ \
  X(Div)    /* Division            */ \
  X(Neg)    /* Negation (unary)    */ \
  X(Eq)     /* Equality            */ \
  X(Neq)    /* Inequality          */ \
  X(Lt)     /* Less than           */ \
  X(Le)     /* Less than equals    */ \
  X(Gt)     /* Greater than        */ \
  X(Ge)     /* Greater than equals */ \
  X(Not)    /* Logical not         */ \
  X(And)    /* Logical and         */ \
  X(Or)     /* Logical or          */ \
  X(Jmp)    /* Unconditional jump  */ \
  X(JmpIf)  /* Conditional jump    */ \
  X(JmpNot) /* Jump if not         */ \
  X(LoadL)  /* Load local value    */ \
  X(StoreL) /* Store local         */ \
  X(Halt)   /* Halt!!!             */ \

enum class NullImmediate {};
using Operand = std::variant<
  double,            /// Float immediate
  NullImmediate,     /// Null immediate
  int64_t,           /// jump operand or integer immediate
  StringPool::Index, /// String immediate
  Nothing_           /// Empty (no operand)
>;

struct OpCode {
  N19_MAKE_COMPARABLE_MEMBER(OpCode, value);
public:
  #define X(VALUE) VALUE,
  enum Value : uint8_t {
    None,
    N19_INTERP_OPCODE_LIST
  };
  #undef X

  Value value = None;
  constexpr OpCode() = default;
  constexpr OpCode(Value v) : value(v) {}
};

struct Instruction {
  OpCode op;
  Operand operand = Nothing_{};
};

// struct StackFrame {
//
// };

END_NAMESPACE(n19::interp);
