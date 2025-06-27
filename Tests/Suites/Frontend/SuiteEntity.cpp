/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Bulwark/Bulwark.hpp>
#include <Frontend/Entities/EntityTable.hpp>
using namespace rl;

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
