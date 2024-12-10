/*
* Copyright (c) 2024 Diago Lima
* All rights reserved.
*
* This software is licensed under the BSD 3-Clause "New" or "Revised" license
* found in the LICENSE file in the root directory of this project's source tree.
*/

#ifndef MEMORY_HPP
#define MEMORY_HPP
#include <Core/ClassTraits.hpp>
#include <Core/Concepts.hpp>
BEGIN_NAMESPACE(n19);

template<typename T>
struct __DeleterFor {
  constexpr void operator()(T* t) { delete t; }
  constexpr __DeleterFor() = default;
};

template<typename T>
struct __DeleterFor<T[]> {
  constexpr void operator()(T* t) { delete[] t; }
  constexpr __DeleterFor() = default;
};

template<class T, class D = __DeleterFor<T>>
class Box {
N19_MAKE_NONCOPYABLE(Box);
public:

};

END_NAMESPACE(n19);
#endif //MEMORY_HPP
