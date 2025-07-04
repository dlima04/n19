/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#define LR_KEYWORDS                                             \
  KEYWORD_X("return", Return, Keyword)                          \
  KEYWORD_X("break", Break, Keyword)                            \
  KEYWORD_X("continue", Continue, Keyword | ControlFlow)        \
  KEYWORD_X("for", For, Keyword | ControlFlow)                  \
  KEYWORD_X("while", While, Keyword | ControlFlow)              \
  KEYWORD_X("do", Do, Keyword | ControlFlow)                    \
  KEYWORD_X("if", If, Keyword | ControlFlow)                    \
  KEYWORD_X("else", Else, Keyword | ControlFlow)                \
  KEYWORD_X("struct", Struct, Keyword)                          \
  KEYWORD_X("switch", Switch, Keyword | ControlFlow)            \
  KEYWORD_X("case", Case, Keyword | ControlFlow)                \
  KEYWORD_X("default", Default, Keyword | ControlFlow)          \
  KEYWORD_X("scope", Scope, Keyword)                            \
  KEYWORD_X("defer", Defer, Keyword)                            \
  KEYWORD_X("defer_if", DeferIf, Keyword)                       \
  KEYWORD_X("sizeof", Sizeof, Keyword | UnaryOp | ValidPrefix)  \
  KEYWORD_X("typeof", Typeof, Keyword | UnaryOp | ValidPrefix)  \
  KEYWORD_X("fallthrough", Fallthrough, Keyword | ControlFlow)  \
  KEYWORD_X("namespace", Namespace, Keyword)                    \
  KEYWORD_X("compose", Compose, Keyword)                        \
  KEYWORD_X("compeval", CompEval, Keyword)                      \
  KEYWORD_X("proc", Proc, Keyword)                              \
  KEYWORD_X("let", Let, Keyword)                                \
  KEYWORD_X("const", Const, Keyword)                            \
  KEYWORD_X("as", As, Keyword | BinaryOp)                       \
  KEYWORD_X("using", Using, Keyword)                            \
  KEYWORD_X("true", BooleanLiteral, Literal)                    \
  KEYWORD_X("false", BooleanLiteral, Literal)                   \
  KEYWORD_X("null", NullLiteral, Literal)                       \

