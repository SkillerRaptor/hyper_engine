/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperCore/Bits.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace HyperCore;

TEST_CASE("Numbers can be bit shifted", "[HyperCore::Bit]")
{
	REQUIRE(Bit(0) == 1);
	REQUIRE(Bit(1) == 2);
	REQUIRE(Bit(2) == 4);
	REQUIRE(Bit(3) == 8);
	REQUIRE(Bit(4) == 16);
	REQUIRE(Bit(5) == 32);
	REQUIRE(Bit(6) == 64);
	REQUIRE(Bit(7) == 128);
	REQUIRE(Bit(8) == 256);
	REQUIRE(Bit(9) == 512);
	REQUIRE(Bit(10) == 1024);
}
