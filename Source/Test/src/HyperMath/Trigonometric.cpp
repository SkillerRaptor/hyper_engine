/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperMath/Trigonometric.hpp>

#include <catch2/catch.hpp>

TEST_CASE("radians of degrees can be calculated with constexpr", "[HyperMath][Trigonometric][radians]")
{
	STATIC_REQUIRE(HyperMath::radians(90.0f) == 1.57079633f);
	STATIC_REQUIRE(HyperMath::radians(180.0f) == 3.14159265f);
	STATIC_REQUIRE(HyperMath::radians(270.0f) == 4.71238898f);
	STATIC_REQUIRE(HyperMath::radians(360.0f) == 6.28318531f);
}

TEST_CASE("degrees of radians can be calculated with constexpr", "[HyperMath][Trigonometric][degrees]")
{
	STATIC_REQUIRE(HyperMath::degrees(1.57079633f) == 90.0f);
	STATIC_REQUIRE(HyperMath::degrees(3.14159265f) == 180.0f);
	STATIC_REQUIRE(HyperMath::degrees(4.71238898f) == 270.0f);
	STATIC_REQUIRE(HyperMath::degrees(6.28318531f) == 360.0f);
}
