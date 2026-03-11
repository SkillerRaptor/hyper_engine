/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include <catch2/catch_test_macros.hpp>

#include <hyper_core/bit_flags.hpp>
#include <hyper_core/types.hpp>

using namespace he;

enum class TestFlags : u32 {
    A = 1 << 0,
    B = 1 << 1,
    C = 1 << 2,
    AB = A | B,
    All = A | B | C
};

TEST_CASE("BitFlags default construction yields empty flags", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> flags;

    STATIC_REQUIRE_FALSE(flags.has_any(TestFlags::A));
    STATIC_REQUIRE_FALSE(flags.has_any(TestFlags::B));
    STATIC_REQUIRE_FALSE(flags.has_any(TestFlags::C));
    STATIC_REQUIRE_FALSE(flags.has_any(TestFlags::All));
}

TEST_CASE("BitFlags single-flag construction", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> flags = TestFlags::A;

    STATIC_REQUIRE(flags.has(TestFlags::A));
    STATIC_REQUIRE_FALSE(flags.has(TestFlags::B));
    STATIC_REQUIRE_FALSE(flags.has(TestFlags::C));
}

TEST_CASE("BitFlags initializer_list construction", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> flags { TestFlags::A, TestFlags::C };

    STATIC_REQUIRE(flags.has(TestFlags::A));
    STATIC_REQUIRE_FALSE(flags.has(TestFlags::B));
    STATIC_REQUIRE(flags.has(TestFlags::C));
}

TEST_CASE("BitFlags initializer_list with all flags", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> flags { TestFlags::A, TestFlags::B, TestFlags::C };

    STATIC_REQUIRE(flags.has_all(TestFlags::All));
}

TEST_CASE("BitFlags initializer_list with single flag matches single-flag construction", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> a { TestFlags::A };
    constexpr BitFlags<TestFlags> b = TestFlags::A;

    STATIC_REQUIRE(a == b);
}

TEST_CASE("BitFlags set() adds a flag", "[BitFlags]")
{
    BitFlags<TestFlags> flags;

    flags.set(TestFlags::A);
    REQUIRE(flags.has(TestFlags::A));
}

TEST_CASE("BitFlags set() is idempotent", "[BitFlags]")
{
    BitFlags<TestFlags> flags;

    flags.set(TestFlags::A);
    flags.set(TestFlags::A);

    REQUIRE(flags.has(TestFlags::A));
    REQUIRE_FALSE(flags.has(TestFlags::B));
    REQUIRE_FALSE(flags.has(TestFlags::C));
}

TEST_CASE("BitFlags set() accumulates multiple flags", "[BitFlags]")
{
    BitFlags<TestFlags> flags;

    flags.set(TestFlags::A);
    flags.set(TestFlags::B);

    REQUIRE(flags.has_all(TestFlags::AB));
}

TEST_CASE("BitFlags remove() clears a specific flag", "[BitFlags]")
{
    BitFlags<TestFlags> flags { TestFlags::A, TestFlags::B };

    flags.remove(TestFlags::A);

    REQUIRE_FALSE(flags.has(TestFlags::A));
    REQUIRE(flags.has(TestFlags::B));
}

TEST_CASE("BitFlags remove() on absent flag is a no-op", "[BitFlags]")
{
    BitFlags<TestFlags> flags = TestFlags::B;

    flags.remove(TestFlags::A);

    REQUIRE_FALSE(flags.has(TestFlags::A));
    REQUIRE(flags.has(TestFlags::B));
}

TEST_CASE("BitFlags clear() removes all flags", "[BitFlags]")
{
    BitFlags<TestFlags> flags { TestFlags::All };

    flags.clear();

    REQUIRE_FALSE(flags.has_any(TestFlags::All));
}

TEST_CASE("BitFlags clear() on empty flags is a no-op", "[BitFlags]")
{
    BitFlags<TestFlags> flags;

    flags.clear();

    REQUIRE_FALSE(flags.has_any(TestFlags::All));
}

TEST_CASE("BitFlags has() is equivalent to has_all()", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> flags = TestFlags::A;

    STATIC_REQUIRE(flags.has(TestFlags::A) == flags.has_all(TestFlags::A));
    STATIC_REQUIRE(flags.has(TestFlags::B) == flags.has_all(TestFlags::B));
}

TEST_CASE("BitFlags has_all() requires every bit to be set", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> flags { TestFlags::A, TestFlags::B };

    STATIC_REQUIRE(flags.has_all(TestFlags::AB));
    STATIC_REQUIRE_FALSE(flags.has_all(TestFlags::All));
}

TEST_CASE("BitFlags has_all() on empty flags returns false", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> flags;

    STATIC_REQUIRE_FALSE(flags.has_all(TestFlags::A));
}

TEST_CASE("BitFlags has_any() returns true if at least one bit matches", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> flags = TestFlags::A;

    STATIC_REQUIRE(flags.has_any(TestFlags::A));
    STATIC_REQUIRE(flags.has_any(TestFlags::AB)); // A is part of AB
    STATIC_REQUIRE_FALSE(flags.has_any(TestFlags::C));
}

TEST_CASE("BitFlags has_any() on empty flags returns false", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> flags;

    STATIC_REQUIRE_FALSE(flags.has_any(TestFlags::All));
}

TEST_CASE("BitFlags operator| (BitFlags, BitFlags)", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> a = TestFlags::A;
    constexpr BitFlags<TestFlags> b = TestFlags::B;
    constexpr BitFlags<TestFlags> result = a | b;

    STATIC_REQUIRE(result.has_all(TestFlags::AB));
    STATIC_REQUIRE_FALSE(result.has(TestFlags::C));
}

TEST_CASE("BitFlags operator| (BitFlags, enum)", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> flags = TestFlags::A;
    constexpr BitFlags<TestFlags> result = flags | TestFlags::B;

    STATIC_REQUIRE(result.has_all(TestFlags::AB));
    STATIC_REQUIRE_FALSE(result.has(TestFlags::C));
}

TEST_CASE("BitFlags operator|= (enum)", "[BitFlags]")
{
    BitFlags<TestFlags> flags = TestFlags::A;

    flags |= TestFlags::B;

    REQUIRE(flags.has_all(TestFlags::AB));
}

TEST_CASE("BitFlags operator|= (BitFlags)", "[BitFlags]")
{
    BitFlags<TestFlags> flags = TestFlags::A;
    const BitFlags<TestFlags> other = TestFlags::B;

    flags |= other;

    REQUIRE(flags.has_all(TestFlags::AB));
}

TEST_CASE("BitFlags operator& (BitFlags, enum) masks bits", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> flags = TestFlags::All;
    constexpr BitFlags<TestFlags> result = flags & TestFlags::AB;

    STATIC_REQUIRE(result.has_all(TestFlags::AB));
    STATIC_REQUIRE_FALSE(result.has(TestFlags::C));
}

TEST_CASE("BitFlags operator& (BitFlags, BitFlags) masks bits", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> all = TestFlags::All;
    constexpr BitFlags<TestFlags> mask { TestFlags::A, TestFlags::B };
    constexpr BitFlags<TestFlags> result = all & mask;

    STATIC_REQUIRE(result.has_all(TestFlags::AB));
    STATIC_REQUIRE_FALSE(result.has(TestFlags::C));
}

TEST_CASE("BitFlags operator& with no common bits yields empty", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> a = TestFlags::A;
    constexpr BitFlags<TestFlags> c = TestFlags::C;
    constexpr BitFlags<TestFlags> result = a & c;

    STATIC_REQUIRE_FALSE(result.has_any(TestFlags::All));
}

TEST_CASE("BitFlags operator&= (enum)", "[BitFlags]")
{
    BitFlags<TestFlags> flags = TestFlags::All;

    flags &= TestFlags::AB;

    REQUIRE(flags.has_all(TestFlags::AB));
    REQUIRE_FALSE(flags.has(TestFlags::C));
}

TEST_CASE("BitFlags operator&= (BitFlags)", "[BitFlags]")
{
    BitFlags<TestFlags> flags = TestFlags::All;
    const BitFlags<TestFlags> mask = TestFlags::AB;

    flags &= mask;

    REQUIRE(flags.has_all(TestFlags::AB));
    REQUIRE_FALSE(flags.has(TestFlags::C));
}

TEST_CASE("BitFlags operator^ (enum) toggles a set bit off", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> flags = TestFlags::A;
    constexpr BitFlags<TestFlags> result = flags ^ TestFlags::A;

    STATIC_REQUIRE_FALSE(result.has(TestFlags::A));
}

TEST_CASE("BitFlags operator^ (enum) toggles an unset bit on", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> flags = TestFlags::A;
    constexpr BitFlags<TestFlags> result = flags ^ TestFlags::B;

    STATIC_REQUIRE(result.has(TestFlags::A));
    STATIC_REQUIRE(result.has(TestFlags::B));
}

TEST_CASE("BitFlags operator^ (BitFlags) toggles multiple bits", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> flags = TestFlags::AB;
    constexpr BitFlags<TestFlags> toggle = TestFlags::All; // A|B|C
    constexpr BitFlags<TestFlags> result = flags ^ toggle; // clears A,B; sets C

    STATIC_REQUIRE_FALSE(result.has(TestFlags::A));
    STATIC_REQUIRE_FALSE(result.has(TestFlags::B));
    STATIC_REQUIRE(result.has(TestFlags::C));
}

TEST_CASE("BitFlags operator^= (enum) toggles a set bit off", "[BitFlags]")
{
    BitFlags<TestFlags> flags = TestFlags::AB;

    flags ^= TestFlags::A;

    REQUIRE_FALSE(flags.has(TestFlags::A));
    REQUIRE(flags.has(TestFlags::B));
}

TEST_CASE("BitFlags operator^= (BitFlags)", "[BitFlags]")
{
    BitFlags<TestFlags> flags = TestFlags::AB;
    const BitFlags<TestFlags> toggle = TestFlags::All;

    flags ^= toggle;

    REQUIRE_FALSE(flags.has(TestFlags::A));
    REQUIRE_FALSE(flags.has(TestFlags::B));
    REQUIRE(flags.has(TestFlags::C));
}

TEST_CASE("BitFlags operator~ clears originally-set bits", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> flags = TestFlags::A;
    constexpr BitFlags<TestFlags> inverted = ~flags;

    STATIC_REQUIRE_FALSE(inverted.has(TestFlags::A));
}

TEST_CASE("BitFlags operator~ sets originally-clear bits", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> flags = TestFlags::A;
    constexpr BitFlags<TestFlags> inverted = ~flags;

    STATIC_REQUIRE(inverted.has_any(TestFlags::B));
    STATIC_REQUIRE(inverted.has_any(TestFlags::C));
}

TEST_CASE("BitFlags double complement is identity", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> flags { TestFlags::A, TestFlags::C };
    constexpr BitFlags<TestFlags> roundtrip = ~~flags;

    STATIC_REQUIRE(roundtrip == flags);
}

TEST_CASE("BitFlags complement of all-set flags (within used bits) has no used bits set", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> all = TestFlags::All;
    constexpr BitFlags<TestFlags> inverted = ~all;

    // The three defined bits should all be absent after complement
    STATIC_REQUIRE_FALSE((inverted & TestFlags::All).has_any(TestFlags::All));
}

TEST_CASE("BitFlags operator== for equal flags", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> a { TestFlags::A, TestFlags::B };
    constexpr BitFlags<TestFlags> b = TestFlags::AB;

    STATIC_REQUIRE(a == b);
}

TEST_CASE("BitFlags operator!= for different flags", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> a = TestFlags::A;
    constexpr BitFlags<TestFlags> c = TestFlags::C;

    STATIC_REQUIRE(a != c);
}

TEST_CASE("BitFlags two default-constructed instances are equal", "[BitFlags]")
{
    constexpr BitFlags<TestFlags> a;
    constexpr BitFlags<TestFlags> b;

    STATIC_REQUIRE(a == b);
}

TEST_CASE("BitFlags chained set and remove round-trips correctly", "[BitFlags]")
{
    BitFlags<TestFlags> flags;

    flags.set(TestFlags::A);
    flags.set(TestFlags::B);
    flags.set(TestFlags::C);
    flags.remove(TestFlags::B);

    REQUIRE(flags.has(TestFlags::A));
    REQUIRE_FALSE(flags.has(TestFlags::B));
    REQUIRE(flags.has(TestFlags::C));
}

TEST_CASE("BitFlags set then clear leaves flags empty", "[BitFlags]")
{
    BitFlags<TestFlags> flags;

    flags.set(TestFlags::A);
    flags.set(TestFlags::B);
    flags.clear();

    REQUIRE_FALSE(flags.has_any(TestFlags::All));
}
