#ifndef CONCEPTS_H
#define CONCEPTS_H
#include <type_traits>
#include <concepts>

namespace n19 {
  class AstNode;
  class Entity;

  // n19::AreAll
  // Constrains each type in the pack Us
  // to being of type T. The constraint is not
  // satisfied unless every type in Us... is of
  // type T.
  template <class T, class ...Us>
  concept AreAll =
    (std::is_same_v<T, Us> && ...);

  // n19::IsAnyOf
  // Constrains T so that it must be any type
  // in the pack Us.
  template<class T, class ...Us>
  concept IsAnyOf =
    ((std::is_same_v<T, Us> || ...));

  // n19::IsSame
  // Constrains the type T to being the same as
  // the type U (removes reference qualifications).
  template <class T, class U>
  concept IsSame =
    std::is_same_v<
      std::remove_reference_t<T>,
      std::remove_reference_t<U>>;

  // n19::IsIntOrFloatingPoint
  // The name is self explanatory
  template <class ...Ts>
  concept IsIntOrFloatingPoint =
    (std::is_integral_v<Ts> && ...);

  // n19::IsAstNode
  // Constrains any amount of types Ts
  // to being a derived class of n19::AstNode,
  // or a n19::AstNode itself.
  template <class ...Ts>
  concept IsAstNode =
    (std::derived_from<Ts, AstNode> && ...) ||
    (AreAll<AstNode, Ts...>);

  // n19::IsEntity
  // Constrains any amount of types Ts
  // to being a derived class of n19::Entity,
  // or a n19::Entity itself.
  template <class ...Ts>
  concept IsEntity =
    (std::derived_from<Ts, Entity> && ...) ||
    (AreAll<Entity, Ts...>);
}

#endif //CONCEPTS_H
