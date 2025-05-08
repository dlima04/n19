/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef N19_CLASSTRAITS_HPP
#define N19_CLASSTRAITS_HPP
#include <compare>

#define N19_MAKE_COMPARABLE_IMPL_(OP, MB, CLASS)      \
  bool operator OP (const CLASS& other) const {       \
    return other OP this->MB;                         \
  }                                                   \

#define N19_MAKE_COMPARABLE_MEMBER_IMPL_(C, MB, OP)   \
  bool operator OP (const C& other) const {           \
    return other.MB OP this->MB;                      \
  }                                                   \

#define N19_MAKE_COMPARABLE_ON(CLASS, MB)             \
  public:                                             \
    N19_MAKE_COMPARABLE_IMPL_(==, MB, CLASS)          \
    N19_MAKE_COMPARABLE_IMPL_(!=, MB, CLASS)          \

#define N19_MAKE_COMPARABLE_MEMBER(SELF, MB)          \
  public:                                             \
    N19_MAKE_COMPARABLE_MEMBER_IMPL_(SELF, MB, ==)    \
    N19_MAKE_COMPARABLE_MEMBER_IMPL_(SELF, MB, !=)    \

#define N19_MAKE_NONCOPYABLE(C)                       \
  private:                                            \
    C(const C&) = delete;                             \
    C& operator =(const C&) = delete                  \

#define N19_MAKE_NONMOVABLE(C)                        \
  private:                                            \
    C(C&&) = delete;                                  \
    C& operator=(C&&) = delete                        \

#define N19_MAKE_DEFAULT_COPY_CONSTRUCTIBLE(C)        \
  public: C(const C& other) = default;                \

#define N19_MAKE_DEFAULT_MOVE_CONSTRUCTIBLE(C)        \
  public: C(C&& other) = default;                     \

#define N19_MAKE_DEFAULT_COPY_ASSIGNABLE(C)           \
  public: C& operator=(const C& other) = default;     \

#define N19_MAKE_DEFAULT_MOVE_ASSIGNABLE(C)           \
  public: C& operator=(C&& other) = default;          \

#define N19_MAKE_DEFAULT_ASSIGNABLE(C)                \
  N19_MAKE_DEFAULT_COPY_ASSIGNABLE(C)                 \
  N19_MAKE_DEFAULT_MOVE_ASSIGNABLE(C)                 \

#define N19_MAKE_DEFAULT_CONSTRUCTIBLE(C)             \
  N19_MAKE_DEFAULT_MOVE_CONSTRUCTIBLE(C)              \
  N19_MAKE_DEFAULT_COPY_CONSTRUCTIBLE(C)              \

#define N19_MAKE_SPACESHIP(C)                         \
  public: auto operator<=>(const C&) const = default; \

#endif //CLASSTRAITS_HPP
