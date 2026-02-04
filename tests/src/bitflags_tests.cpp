/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include <catch2/catch_test_macros.hpp>

#include <hyper_core/bit_flags.hpp>
#include <hyper_core/types.hpp>

using namespace he;

enum class TestFlags : u32
{
    A = 1 << 0,
    B = 1 << 1,
    C = 1 << 2,
    AB = A | B,
    All = A | B | C
};

TEST_CASE("BitFlags default construction", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> flags;

    STATIC_REQUIRE_FALSE(flags.has_any(TestFlags::A));
    STATIC_REQUIRE_FALSE(flags.has_any(TestFlags::B));
    STATIC_REQUIRE_FALSE(flags.has_any(TestFlags::C));
}

TEST_CASE("BitFlags single-flag construction", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> flags = TestFlags::A;

    STATIC_REQUIRE(flags.has(TestFlags::A));
    STATIC_REQUIRE_FALSE(flags.has(TestFlags::B));
}

TEST_CASE("BitFlags initializer_list construction", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> flags { TestFlags::A, TestFlags::C };

    STATIC_REQUIRE(flags.has(TestFlags::A));
    STATIC_REQUIRE_FALSE(flags.has(TestFlags::B));
    STATIC_REQUIRE(flags.has(TestFlags::C));
}

TEST_CASE("BitFlags set()", "[BitFlags]")
{
    BitFlags<TestFlags> flags;

    flags.set(TestFlags::A);
    REQUIRE(flags.has(TestFlags::A));

    flags.set(TestFlags::B);
    REQUIRE(flags.has_all(TestFlags::AB));
}

TEST_CASE("BitFlags remove()", "[BitFlags]")
{
    BitFlags<TestFlags> flags { TestFlags::A, TestFlags::B };

    flags.remove(TestFlags::A);

    REQUIRE_FALSE(flags.has(TestFlags::A));
    REQUIRE(flags.has(TestFlags::B));
}

TEST_CASE("BitFlags clear()", "[BitFlags]")
{
    BitFlags<TestFlags> flags { TestFlags::All };

    flags.clear();

    REQUIRE_FALSE(flags.has_any(TestFlags::All));
}

TEST_CASE("BitFlags has_all()", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> flags { TestFlags::A, TestFlags::B };

    STATIC_REQUIRE(flags.has_all(TestFlags::AB));
    STATIC_REQUIRE_FALSE(flags.has_all(TestFlags::All));
}

TEST_CASE("BitFlags has_any()", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> flags = TestFlags::A;

    STATIC_REQUIRE(flags.has_any(TestFlags::A));
    STATIC_REQUIRE(flags.has_any(TestFlags::AB));
    STATIC_REQUIRE_FALSE(flags.has_any(TestFlags::C));
}

TEST_CASE("BitFlags operator| (BitFlags)", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> a = TestFlags::A;
    constexpr BitFlags<TestFlags> b = TestFlags::B;
    constexpr BitFlags<TestFlags> result = a | b;

    STATIC_REQUIRE(result.has_all(TestFlags::AB));
}

TEST_CASE("BitFlags operator| (enum)", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> flags = TestFlags::A;
    constexpr BitFlags<TestFlags> result = flags | TestFlags::B;

    STATIC_REQUIRE(result.has_all(TestFlags::AB));
}

TEST_CASE("BitFlags operator|=", "[BitFlags]")
{
    BitFlags<TestFlags> flags = TestFlags::A;

    flags |= TestFlags::B;

    REQUIRE(flags.has_all(TestFlags::AB));
}

TEST_CASE("BitFlags operator& (enum)", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> flags = TestFlags::All;
    constexpr BitFlags<TestFlags> result = flags & TestFlags::AB;

    STATIC_REQUIRE(result.has_all(TestFlags::AB));
    STATIC_REQUIRE_FALSE(result.has(TestFlags::C));
}

TEST_CASE("BitFlags operator&=", "[BitFlags]")
{
    BitFlags<TestFlags> flags = TestFlags::All;

    flags &= TestFlags::AB;

    REQUIRE(flags.has_all(TestFlags::AB));
    REQUIRE_FALSE(flags.has(TestFlags::C));
}

TEST_CASE("BitFlags operator^ (enum)", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> flags = TestFlags::A;
    constexpr BitFlags<TestFlags> result = flags ^ TestFlags::A;

    STATIC_REQUIRE_FALSE(result.has(TestFlags::A));
}

TEST_CASE("BitFlags operator^=", "[BitFlags]")
{
    BitFlags<TestFlags> flags = TestFlags::A;

    flags ^= TestFlags::B;

    REQUIRE(flags.has_all(TestFlags::AB));
}

TEST_CASE("BitFlags operator~", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> flags = TestFlags::A;
    constexpr BitFlags<TestFlags> inverted = ~flags;

    STATIC_REQUIRE_FALSE(inverted.has(TestFlags::A));
    STATIC_REQUIRE(inverted.has_any(TestFlags::B));
}

TEST_CASE("BitFlags equality operators", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> a { TestFlags::A, TestFlags::B };
    constexpr BitFlags<TestFlags> b = TestFlags::AB;
    constexpr BitFlags<TestFlags> c = TestFlags::C;

    STATIC_REQUIRE(a == b);
    STATIC_REQUIRE(a != c);
}
