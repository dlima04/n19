/*
* Copyright (c) 2024 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef N19_TUPLE_HPP
#define N19_TUPLE_HPP
#include <Core/Concepts.hpp>
#include <Core/ClassTraits.hpp>
#include <Core/Platform.hpp>
#include <utility>
#include <concepts>
#include <cstdint>
BEGIN_NAMESPACE(n19);

template<size_t n, typename ...Types>
struct NthTypeAccessor;

template<size_t n, typename T, typename ...Types>
struct NthTypeAccessor<n, T, Types...> {
  using Type = typename NthTypeAccessor<n - 1, Types...>::Type;
};

template<typename T, typename ...Types>
struct NthTypeAccessor<0, T, Types...> {
  using Type = T;
};

template<size_t n, typename ...Types>
using NthType = typename NthTypeAccessor<n, Types...>::Type;

template<typename ...Tail>
struct TupleStorage;

template<typename T, typename ...Tail>
struct TupleStorage<T, Tail...> {
  constexpr TupleStorage() = default;
  ~TupleStorage() = default;

  TupleStorage(T&& head, Tail&&... tail)
  : head_(std::move(head)), tail_(std::move(tail)...) {}

  TupleStorage(const T& head, const Tail&... tail)
  : head_(head), tail_(tail...) {}

  template<typename ...OTs>
  TupleStorage(const TupleStorage<OTs...>& other)
  : head_(other.head_), tail_(other.tail_) {}

  template<typename ...OTs>
  TupleStorage(TupleStorage<OTs...>&& other)
  : head_(std::move(other.head_)), tail_(std::move(other.tail_)) {}

  T head_;
  TupleStorage<Tail...> tail_;
};

template<>
struct TupleStorage<> {
  constexpr TupleStorage() = default;
  ~TupleStorage() = default;
};

template<size_t n, typename ...Types>
struct NthItemAccessor;

template<size_t n, typename T, typename ...Types>
struct NthItemAccessor<n, T, Types...> {
  static auto access(const TupleStorage<T, Types...>& store) -> const NthType<n - 1, Types...>& {
    return NthItemAccessor<n - 1, Types...>::access(store.tail_);
  }

  static auto access(TupleStorage<T, Types...>& store) -> NthType<n - 1, Types...>& {
    return NthItemAccessor<n - 1, Types...>::access(store.tail_);
  }
};

template<typename T, typename ...Types>
struct NthItemAccessor<0, T, Types...> {
  static auto access(TupleStorage<T, Types...>& store) -> T& {
    return store.head_;
  }

  static auto access(const TupleStorage<T, Types...>& store) -> const T& {
    return store.head_;
  }
};

template<typename ...Types>
class Tuple /*Impl*/ {
public:
  template<typename ...OTs>
  friend class Tuple;

  template<size_t n, typename ...OTs>
  struct ConstructibleWith_ {
    static constexpr bool value
      = std::constructible_from<NthType<n, Types...>, NthType<n, OTs...>>
      && ConstructibleWith_<n - 1, OTs...>::value;
  };

  template<typename ...OTs>
  struct ConstructibleWith_<0, OTs...> {
    static constexpr bool value
      = std::constructible_from<NthType<0, Types...>, NthType<0, OTs...>>;
  };

  template<size_t i> NODISCARD_
  typename NthTypeAccessor<i, Types...>::Type& at() {
    static_assert(i < sizeof...(Types), "Out of bounds tuple access!");
    return NthItemAccessor<i, Types...>::access(store_);
  }

  template<size_t i> NODISCARD_
  const typename NthTypeAccessor<i, Types...>::Type& at() const {
    static_assert(i < sizeof...(Types), "Out of bounds tuple access!");
    return NthItemAccessor<i, Types...>::access(store_);
  }

  template<typename ...OTs>
    requires ConstructibleWith_<sizeof...(OTs) - 1, OTs...>::value
  Tuple(const Tuple<OTs...>& other) : store_(other.store_) {}

  template<typename ...OTs>
    requires ConstructibleWith_<sizeof...(OTs) - 1, OTs...>::value
  Tuple(Tuple<OTs...>&& other) noexcept : store_(std::move(other.store_)) {}

  template<typename ...OTs>
    requires ConstructibleWith_<sizeof...(OTs) - 1, OTs...>::value
  Tuple(OTs&&... args) : store_(std::forward<OTs>(args)...) {}

 ~Tuple() = default;
  Tuple() = default;
private:
  TupleStorage<Types...> store_;
};

template<typename T>
class Tuple<T> /*Single*/ {
public:
  template<typename ...OTs>
  friend class Tuple;

  Tuple(const Tuple& other) : value_(other.value_) {}
  Tuple(Tuple&& other) noexcept : value_(other.value_) {}

  Tuple(const T& val) : value_(val) {}
  Tuple(T&& val) : value_(std::move(val)) {}

 ~Tuple() = default;
  Tuple() = default;
  T value_;
};

template<typename ...Args>
auto make_tuple(Args&&... args) -> Tuple<RemoveReference<Args>...> {
  return Tuple<RemoveReference<Args>...>{ std::forward<Args>(args)... };
}

template<size_t n, typename ...Args>
auto tuple_accessor(const Tuple<Args...>& tup) -> const NthType<n, Args...>& {
  return tup. template at<n>();
}

template<size_t n, typename ...Args>
auto tuple_accessor(Tuple<Args...>&& tup) -> NthType<n, Args...>&& {
  return tup. template at<n>();
}

END_NAMESPACE(n19);
#endif //N19_TUPLE_HPP
