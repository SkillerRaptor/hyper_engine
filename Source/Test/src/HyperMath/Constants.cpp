/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperMath/Constants.hpp>

#include <catch2/catch.hpp>

TEST_CASE("g_PI can be used with constexpr", "[HyperMath][Constants][g_PI]")
{
	STATIC_REQUIRE(HyperMath::g_PI * 2 == HyperMath::g_PI2);
}

TEST_CASE("g_PI2 can be used with constexpr", "[HyperMath][Constants][g_PI2]")
{
	STATIC_REQUIRE(HyperMath::g_PI2 / 2 == HyperMath::g_PI);
}
