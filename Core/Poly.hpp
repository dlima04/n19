/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#pragma once

#include <Core/Common.hpp>
#include <Core/ClassTraits.hpp>
#include <variant>
#include <type_traits>
#include <utility>
BEGIN_NAMESPACE(n19);

/* n19::Poly<T>
 *
 * A simple inline polymorphic buffer. Can replace
 * smart pointers (for polymorphism needs) in some cases.
 */
template<typename Base, typename ...Derived>
class Poly {
public:
  static_assert(std::is_abstract_v<Base>);
  static_assert((std::is_base_of_v<Base, Derived> && ...));

  auto operator->() const -> const Base* {
    return std::visit([](const auto& val_) -> const Base* {
      return static_cast<const Base*>(&val_);
    }, buff_);
  }

  auto operator->() -> Base* {
    return std::visit([](auto& val_) -> Base* {
      return static_cast<Base*>(&val_);
    }, buff_);
  }

  auto operator*() const -> const Base& { return *this->operator->(); }
  auto operator*() -> Base&  { return *this->operator->(); }

  template<typename T>
  auto get_as() const -> const T& { return std::get<T>(buff_); }

  template<typename T>
  auto get_as() -> T& { return std::get<T>(buff_); }

  template<typename T> requires(!IsSame<DecayT<T>, Poly>)
  Poly& operator=(T&& val_) {
    buff_ = std::forward(val_);
    return *this;
  }

  Poly& operator=(const Poly& other) {
    buff_ = other.buff_;
    return *this;
  }

  Poly& operator=(Poly&& other) {
    buff_ = std::move(other.buff_);
    return *this;
  }

  template<typename T> requires(!IsSame<DecayT<T>, Poly>)
  Poly(T&& val) : buff_(std::forward<T>(val)) {}

  Poly(const Poly& other) : buff_(other.buff_) {}
  Poly(Poly&& other) : buff_(std::move(other.buff_)) {}

  ~Poly() = default;
private:
  std::variant<Derived...> buff_;
};

END_NAMESPACE(n19);
