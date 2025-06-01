/*
* Copyright (c) 2025 Diago Lima
* SPDX-License-Identifier: BSD-3-Clause
*/

#include <Bulwark/Bulwark.hpp>
#include <Frontend/EntityTable.hpp>
using namespace n19;

TEST_CASE(Entity, SymLinks) {
  SECTION(BasicResolution, {
    EntityTable table(_nstr("MyTable"));
    auto ptr1 = table.insert<SymLink>(N19_ROOT_ENTITY_ID, 1,  1, _nstr("file"), "entity1");
    REQUIRE(ptr1);

    auto ptr2 = table.insert<Struct>(N19_ROOT_ENTITY_ID, 10,  10, _nstr("file"), "entity2");
    REQUIRE(ptr2);

    ptr1->link_ = ptr2->id_;
    auto resolved = table.resolve_link(ptr1);

    REQUIRE(resolved);
    REQUIRE(resolved->id_ == ptr2->id_);
  });

  SECTION(MultiLevelResolution, {
    EntityTable table(_nstr("MyTable"));
    auto ptr1 = table.insert<SymLink>(N19_ROOT_ENTITY_ID, 1,  1, _nstr("file"), "entity1");
    REQUIRE(ptr1);

    auto ptr2 = table.insert<AliasType>(N19_ROOT_ENTITY_ID, 10,  10, _nstr("file"), "entity2");
    REQUIRE(ptr2);

    auto ptr3 = table.insert<Struct>(N19_ROOT_ENTITY_ID, 20,  30, _nstr("file"), "entity3");
    REQUIRE(ptr3);

    ptr1->link_ = ptr2->id_;
    ptr2->link_ = ptr3->id_;

    /// Ensure we resolved the symlink correctly
    auto resolved = table.resolve_link(ptr1);
    REQUIRE(resolved);
    REQUIRE(resolved->id_ == ptr3->id_);
  });
}
