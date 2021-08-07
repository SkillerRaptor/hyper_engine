/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperMath/Common.hpp>

#include <catch2/catch.hpp>


TEST_CASE("Absolute of number can be calculated with constexpr", "[HyperMath][Common][abs]")
{
	STATIC_REQUIRE(HyperMath::abs(1) == 1);
	STATIC_REQUIRE(HyperMath::abs(2) == 2);
	STATIC_REQUIRE(HyperMath::abs(3) == 3);
	
	STATIC_REQUIRE(HyperMath::abs(-4) == 4);
	STATIC_REQUIRE(HyperMath::abs(-5) == 5);
	STATIC_REQUIRE(HyperMath::abs(-6) == 6);
	
	STATIC_REQUIRE(HyperMath::abs(1.0f) == 1.0f);
	STATIC_REQUIRE(HyperMath::abs(2.0f) == 2.0f);
	STATIC_REQUIRE(HyperMath::abs(3.0f) == 3.0f);
	
	STATIC_REQUIRE(HyperMath::abs(-4.0f) == 4.0f);
	STATIC_REQUIRE(HyperMath::abs(-5.0f) == 5.0f);
	STATIC_REQUIRE(HyperMath::abs(-6.0f) == 6.0f);
}

TEST_CASE("Minimum of numbers can be calculated with constexpr", "[HyperMath][Common][min]")
{
	STATIC_REQUIRE(HyperMath::min(1, 0) == 0);
	STATIC_REQUIRE(HyperMath::min(2, 1) == 1);
	STATIC_REQUIRE(HyperMath::min(3, 2) == 2);
	
	STATIC_REQUIRE(HyperMath::min(-4, 1) == -4);
	STATIC_REQUIRE(HyperMath::min(-5, 2) == -5);
	STATIC_REQUIRE(HyperMath::min(-6, 3) == -6);
	
	STATIC_REQUIRE(HyperMath::min(1.0f, 0.0f) == 0.0f);
	STATIC_REQUIRE(HyperMath::min(2.0f, 1.0f) == 1.0f);
	STATIC_REQUIRE(HyperMath::min(3.0f, 2.0f) == 2.0f);
	
	STATIC_REQUIRE(HyperMath::min(-4.0f, 1.0f) == -4.0f);
	STATIC_REQUIRE(HyperMath::min(-5.0f, 2.0f) == -5.0f);
	STATIC_REQUIRE(HyperMath::min(-6.0f, 3.0f) == -6.0f);
}

TEST_CASE("Maximum of numbers can be calculated with constexpr", "[HyperMath][Common][max]")
{
	STATIC_REQUIRE(HyperMath::max(1, 0) == 1);
	STATIC_REQUIRE(HyperMath::max(2, 1) == 2);
	STATIC_REQUIRE(HyperMath::max(3, 2) == 3);
	
	STATIC_REQUIRE(HyperMath::max(-4, 1) == 1);
	STATIC_REQUIRE(HyperMath::max(-5, 2) == 2);
	STATIC_REQUIRE(HyperMath::max(-6, 3) == 3);
	
	STATIC_REQUIRE(HyperMath::max(1.0f, 0.0f) == 1.0f);
	STATIC_REQUIRE(HyperMath::max(2.0f, 1.0f) == 2.0f);
	STATIC_REQUIRE(HyperMath::max(3.0f, 2.0f) == 3.0f);
	
	STATIC_REQUIRE(HyperMath::max(-4.0f, 1.0f) == 1.0f);
	STATIC_REQUIRE(HyperMath::max(-5.0f, 2.0f) == 2.0f);
	STATIC_REQUIRE(HyperMath::max(-6.0f, 3.0f) == 3.0f);
}
