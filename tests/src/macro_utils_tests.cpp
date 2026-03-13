/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include <catch2/catch_test_macros.hpp>

#include <hyper_core/macro_utils.hpp>

using namespace he;

static constexpr usize add(const usize a, const usize b) { return a + b; }

struct Dummy { };

TEST_CASE("argument_count counts zero arguments", "[argument_count]") { STATIC_REQUIRE(detail::argument_count() == 0); }

TEST_CASE("argument_count counts one argument", "[argument_count]") { STATIC_REQUIRE(detail::argument_count(0) == 1); }

TEST_CASE("argument_count counts two arguments", "[argument_count]")
{
    STATIC_REQUIRE(detail::argument_count(0, 0) == 2);
}

TEST_CASE("argument_count counts three arguments", "[argument_count]")
{
    STATIC_REQUIRE(detail::argument_count(0, 0, 0) == 3);
}

TEST_CASE("argument_count counts a large number of arguments", "[argument_count]")
{
    STATIC_REQUIRE(detail::argument_count(0, 0, 0, 0, 0, 0, 0, 0, 0, 0) == 10);
}

TEST_CASE("argument_count is blind to argument type", "[argument_count]")
{
    constexpr Dummy d {};
    STATIC_REQUIRE(detail::argument_count(0, 0.0f, 0.0, 'x', d, "str") == 6);
}

TEST_CASE("argument_count result is a compile-time constant", "[argument_count]")
{
    constexpr usize n = detail::argument_count(1, 2, 3);
    STATIC_REQUIRE(n == 3);
}

TEST_CASE("HE_MACRO_SIZE with zero arguments", "[HE_MACRO_SIZE]") { STATIC_REQUIRE(HE_MACRO_SIZE() == 0); }

TEST_CASE("HE_MACRO_SIZE with one argument", "[HE_MACRO_SIZE]") { STATIC_REQUIRE(HE_MACRO_SIZE(0) == 1); }

TEST_CASE("HE_MACRO_SIZE with two arguments", "[HE_MACRO_SIZE]") { STATIC_REQUIRE(HE_MACRO_SIZE(0, 0) == 2); }

TEST_CASE("HE_MACRO_SIZE with three arguments", "[HE_MACRO_SIZE]") { STATIC_REQUIRE(HE_MACRO_SIZE(0, 0, 0) == 3); }

TEST_CASE("HE_MACRO_SIZE with arithmetic expressions", "[HE_MACRO_SIZE]")
{
    // Each comma-separated token is one argument regardless of operators.
    STATIC_REQUIRE(HE_MACRO_SIZE(1 + 2, 3 * 4, sizeof(int)) == 3);
}

TEST_CASE("HE_MACRO_SIZE with parenthesised sub-expressions containing commas", "[HE_MACRO_SIZE]")
{
    STATIC_REQUIRE(HE_MACRO_SIZE(add(1, 2)) == 1);
    STATIC_REQUIRE(HE_MACRO_SIZE(add(1, 2), add(3, 4)) == 2);
}

TEST_CASE("HE_MACRO_SIZE with nested parenthesised commas", "[HE_MACRO_SIZE]")
{
    STATIC_REQUIRE(HE_MACRO_SIZE((add(1, add(2, 3)))) == 1);
}

TEST_CASE("HE_MACRO_SIZE with mixed literal types", "[HE_MACRO_SIZE]")
{
    STATIC_REQUIRE(HE_MACRO_SIZE(1, 2.0f, 'c', "str") == 4);
}

TEST_CASE("HE_MACRO_SIZE matches argument_count for zero arguments", "[HE_MACRO_SIZE][argument_count]")
{
    STATIC_REQUIRE(HE_MACRO_SIZE() == detail::argument_count());
}

TEST_CASE("HE_MACRO_SIZE matches argument_count for one argument", "[HE_MACRO_SIZE][argument_count]")
{
    STATIC_REQUIRE(HE_MACRO_SIZE(0) == detail::argument_count(0));
}

TEST_CASE("HE_MACRO_SIZE matches argument_count for five arguments", "[HE_MACRO_SIZE][argument_count]")
{
    STATIC_REQUIRE(HE_MACRO_SIZE(0, 0, 0, 0, 0) == detail::argument_count(0, 0, 0, 0, 0));
}
