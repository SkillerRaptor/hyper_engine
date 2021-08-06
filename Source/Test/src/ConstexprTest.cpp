/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <catch2/catch.hpp>

constexpr unsigned int factorial(unsigned int number)
{
	return number <= 1 ? number : factorial(number - 1) * number;
}

TEST_CASE("Factorials are computed with constexpr", "[factorial]")
{
	STATIC_REQUIRE(factorial(1) == 1);
	STATIC_REQUIRE(factorial(2) == 2);
	STATIC_REQUIRE(factorial(3) == 6);
	STATIC_REQUIRE(factorial(10) == 3628800);
}
