/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include <catch2/catch_test_macros.hpp>

#include <hyper_core/macro_utils.hpp>

using namespace he;

static constexpr usize foo(const usize a, const usize b) { return a + b; }

TEST_CASE("argument_count counts zero arguments", "[argument_count]") { STATIC_REQUIRE(detail::argument_count() == 0); }

TEST_CASE("argument_count counts one argument", "[argument_count]") { STATIC_REQUIRE(detail::argument_count(42) == 1); }

TEST_CASE("argument_count counts multiple arguments", "[argument_count]")
{
    STATIC_REQUIRE(detail::argument_count(1, 2, 3, 4) == 4);
}

TEST_CASE("argument_count ignores types and value categories", "[argument_count]")
{
    constexpr usize i = 0;
    constexpr f32 d = 0.0f;

    STATIC_REQUIRE(detail::argument_count(i, d, "text", 3.14f) == 4);
}

TEST_CASE("HE_MACRO_SIZE with zero arguments", "[HE_MACRO_SIZE]") { STATIC_REQUIRE(HE_MACRO_SIZE() == 0); }

TEST_CASE("HE_MACRO_SIZE with one argument", "[HE_MACRO_SIZE]") { STATIC_REQUIRE(HE_MACRO_SIZE(42) == 1); }

TEST_CASE("HE_MACRO_SIZE with multiple arguments", "[HE_MACRO_SIZE]") { STATIC_REQUIRE(HE_MACRO_SIZE(1, 2, 3) == 3); }

TEST_CASE("HE_MACRO_SIZE with expressions", "[HE_MACRO_SIZE]")
{
    STATIC_REQUIRE(HE_MACRO_SIZE(1 + 2, (3 * 4), sizeof(int)) == 3);
}

TEST_CASE("HE_MACRO_SIZE with function calls", "[HE_MACRO_SIZE]")
{
    STATIC_REQUIRE(HE_MACRO_SIZE(foo(1, 2), foo(3, 4)) == 2);
}

TEST_CASE("HE_MACRO_SIZE matches argument_count", "[HE_MACRO_SIZE]")
{
    STATIC_REQUIRE(HE_MACRO_SIZE(1, 2, 3, 4, 5) == detail::argument_count(1, 2, 3, 4, 5));
}
