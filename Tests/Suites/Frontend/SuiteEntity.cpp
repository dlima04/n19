/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Bulwark/Bulwark.hpp>
#include <Frontend/Entities/EntityTable.hpp>
#include <Core/ClassTraits.hpp>
#include <string>
#include <utility>
using namespace rl;

namespace rl {
  class EntFooBar final : public Entity {
    N19_MAKE_DEFAULT_CONSTRUCTIBLE(EntFooBar);
    N19_MAKE_DEFAULT_ASSIGNABLE(EntFooBar);
  public:
    int x = 10;
    int y = 20;
    std::string z;

    auto print(uint32_t depth,
        OStream &stream,
        EntityTable &table ) const -> void override
    {
      (void)table;
      stream << depth;
    }

    EntFooBar() = default;
    EntFooBar(int val1, int val2, std::string&& val3)
      : x(val1)
      , y(val2)
      , z (std::move(val3)) {}
    ~EntFooBar() override = default;
  };
}

TEST_CASE(Entity, SymLinks) {
  SECTION(BasicResolution, {
    EntityTable table(_nstr("MyTable"));
    auto ptr1 = table.insert<SymLink>(RL_ROOT_ENTITY_ID, 1,  1, 1, "entity1");
    REQUIRE(ptr1);

    auto ptr2 = table.insert<Struct>(RL_ROOT_ENTITY_ID, 10,  10, 1, "entity2");
    REQUIRE(ptr2);

    ptr1->link_ = ptr2->id_;
    auto resolved = table.resolve_link(ptr1);

    REQUIRE(resolved);
    REQUIRE(resolved->id_ == ptr2->id_);
  });

  SECTION(MultiLevelResolution, {
    EntityTable table(_nstr("MyTable"));
    auto ptr1 = table.insert<SymLink>(RL_ROOT_ENTITY_ID, 1,  1, 1, "entity1");
    REQUIRE(ptr1);

    auto ptr2 = table.insert<AliasType>(RL_ROOT_ENTITY_ID, 10,  10, 1, "entity2");
    REQUIRE(ptr2);

    auto ptr3 = table.insert<Struct>(RL_ROOT_ENTITY_ID, 20,  30, 1, "entity3");
    REQUIRE(ptr3);

    ptr1->link_ = ptr2->id_;
    ptr2->link_ = ptr3->id_;

    /// Ensure we resolved the symlink correctly
    auto resolved = table.resolve_link(ptr1);
    REQUIRE(resolved);
    REQUIRE(resolved->id_ == ptr3->id_);
  });
}

TEST_CASE(Entity, Construction) {
  EntityTable table(_nstr("MyTable"));
  std::string tempstr = "foobar";
  auto ptr4 = table.insert<EntFooBar>(
    RL_ROOT_ENTITY_ID,
    30, 10, 1,
    "entity4",
    420, 69, std::move(tempstr));

  REQUIRE(ptr4->x == 420);
  REQUIRE(ptr4->y == 69);
  REQUIRE(ptr4->z == "foobar");
}
