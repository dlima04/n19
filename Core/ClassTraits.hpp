/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef CLASSTRAITS_HPP
#define CLASSTRAITS_HPP

#define N19_MAKE_COMPARABLE_IMPL_(OP, MB, CLASS)          \
  bool operator OP (const CLASS& other) const {           \
    return other OP this->MB;                             \
  }                                                       \

#define N19_MAKE_COMPARABLE_MEMBER_IMPL_(SELF, MB, OP)    \
  bool operator OP (const SELF& other) const {            \
    return other.MB OP this->MB;                          \
  }                                                       \

#define N19_MAKE_COMPARABLE_ON(CLASS, MB)                 \
  public:                                                 \
    N19_MAKE_COMPARABLE_IMPL_(==, MB, CLASS)              \
    N19_MAKE_COMPARABLE_IMPL_(!=, MB, CLASS)              \

#define N19_MAKE_COMPARABLE_MEMBER(SELF, MB)              \
  public:                                                 \
    N19_MAKE_COMPARABLE_MEMBER_IMPL_(SELF, MB, ==)        \
    N19_MAKE_COMPARABLE_MEMBER_IMPL_(SELF, MB, !=)        \

#define N19_MAKE_NONCOPYABLE(C)                           \
  private:                                                \
    C(const C&) = delete;                                 \
    C& operator =(const C&) = delete                      \

#define N19_MAKE_NONMOVABLE(C)                            \
  private:                                                \
    C(C&&) = delete;                                      \
    C& operator=(C&&) = delete                            \

#endif //CLASSTRAITS_HPP
