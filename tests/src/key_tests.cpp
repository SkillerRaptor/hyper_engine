/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include <type_traits>

#include <catch2/catch_test_macros.hpp>

#include <hyper_core/key.hpp>

using namespace he;

struct Owner
{
    static constexpr Key<Owner> make_key() { return Key<Owner> { }; }
};

struct Other
{
};

struct Protected
{
    explicit Protected(Key<Owner>) { }
};

TEST_CASE("Key cannot be default-constructed by non-owner", "[Key]")
{
    STATIC_REQUIRE_FALSE(std::is_default_constructible_v<Key<Owner>>);
}

TEST_CASE("Key can be constructed by its owning type", "[Key]")
{
    constexpr Key<Owner> key = Owner::make_key();
    (void) key;

    SUCCEED();
}

TEST_CASE("Key types are distinct per owner type", "[Key]") { STATIC_REQUIRE(!std::is_same_v<Key<Owner>, Key<Other>>); }

TEST_CASE("Key is not implicitly constructible", "[Key]")
{
    STATIC_REQUIRE_FALSE(std::is_convertible_v<int, Key<Owner>>);
}

TEST_CASE("Key copy and move semantics", "[Key]")
{
    STATIC_REQUIRE(std::is_copy_constructible_v<Key<Owner>>);
    STATIC_REQUIRE(std::is_move_constructible_v<Key<Owner>>);
}

TEST_CASE("Key is empty and trivial", "[Key]")
{
    STATIC_REQUIRE(std::is_empty_v<Key<Owner>>);
    STATIC_REQUIRE(std::is_trivially_destructible_v<Key<Owner>>);
}

TEST_CASE("Key enables protected API construction", "[Key]")
{
    constexpr Key<Owner> key = Owner::make_key();
    const Protected object { key };
    (void) object;

    SUCCEED();
}
