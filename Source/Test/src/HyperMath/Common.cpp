/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperMath/Common.hpp>

#include <catch2/catch.hpp>

TEST_CASE("'min' of two numbers can be calculated with constexpr", "[HyperMath][min]")
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

TEST_CASE("'max' of two numbers can be calculated with constexpr", "[HyperMath][max]")
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

TEST_CASE("'abs' of one number can be calculated with constexpr", "[HyperMath][abs]")
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
	
	STATIC_REQUIRE(HyperMath::abs(1.1f) == 1.1f);
	STATIC_REQUIRE(HyperMath::abs(2.2f) == 2.2f);
	STATIC_REQUIRE(HyperMath::abs(3.3f) == 3.3f);
	
	STATIC_REQUIRE(HyperMath::abs(-4.4f) == 4.4f);
	STATIC_REQUIRE(HyperMath::abs(-5.5f) == 5.5f);
	STATIC_REQUIRE(HyperMath::abs(-6.6f) == 6.6f);
}

TEST_CASE("'ceil' of one number can be calculated with constexpr", "[HyperMath][ceil]")
{
	STATIC_REQUIRE(HyperMath::ceil(1) == 1);
	STATIC_REQUIRE(HyperMath::ceil(2) == 2);
	STATIC_REQUIRE(HyperMath::ceil(3) == 3);
	
	STATIC_REQUIRE(HyperMath::ceil(-4) == -4);
	STATIC_REQUIRE(HyperMath::ceil(-5) == -5);
	STATIC_REQUIRE(HyperMath::ceil(-6) == -6);
	
	STATIC_REQUIRE(HyperMath::ceil(1.0f) == 1.0f);
	STATIC_REQUIRE(HyperMath::ceil(2.0f) == 2.0f);
	STATIC_REQUIRE(HyperMath::ceil(3.0f) == 3.0f);
	
	STATIC_REQUIRE(HyperMath::ceil(-4.0f) == -4.0f);
	STATIC_REQUIRE(HyperMath::ceil(-5.0f) == -5.0f);
	STATIC_REQUIRE(HyperMath::ceil(-6.0f) == -6.0f);
	
	STATIC_REQUIRE(HyperMath::ceil(1.1f) == 2.0f);
	STATIC_REQUIRE(HyperMath::ceil(2.2f) == 3.0f);
	STATIC_REQUIRE(HyperMath::ceil(3.3f) == 4.0f);
	
	STATIC_REQUIRE(HyperMath::ceil(-4.4f) == -4.0f);
	STATIC_REQUIRE(HyperMath::ceil(-5.5f) == -5.0f);
	STATIC_REQUIRE(HyperMath::ceil(-6.6f) == -6.0f);
}

TEST_CASE("'floor' of one number can be calculated with constexpr", "[HyperMath][floor]")
{
	STATIC_REQUIRE(HyperMath::floor(1) == 1);
	STATIC_REQUIRE(HyperMath::floor(2) == 2);
	STATIC_REQUIRE(HyperMath::floor(3) == 3);
	
	STATIC_REQUIRE(HyperMath::floor(-4) == -4);
	STATIC_REQUIRE(HyperMath::floor(-5) == -5);
	STATIC_REQUIRE(HyperMath::floor(-6) == -6);
	
	STATIC_REQUIRE(HyperMath::floor(1.0f) == 1.0f);
	STATIC_REQUIRE(HyperMath::floor(2.0f) == 2.0f);
	STATIC_REQUIRE(HyperMath::floor(3.0f) == 3.0f);
	
	STATIC_REQUIRE(HyperMath::floor(-4.0f) == -4.0f);
	STATIC_REQUIRE(HyperMath::floor(-5.0f) == -5.0f);
	STATIC_REQUIRE(HyperMath::floor(-6.0f) == -6.0f);
	
	STATIC_REQUIRE(HyperMath::floor(1.1f) == 1.0f);
	STATIC_REQUIRE(HyperMath::floor(2.2f) == 2.0f);
	STATIC_REQUIRE(HyperMath::floor(3.3f) == 3.0f);
	
	STATIC_REQUIRE(HyperMath::floor(-4.4f) == -5.0f);
	STATIC_REQUIRE(HyperMath::floor(-5.5f) == -6.0f);
	STATIC_REQUIRE(HyperMath::floor(-6.6f) == -7.0f);
}

TEST_CASE("'midpoint' of two numbers can be calculated with constexpr", "[HyperMath][midpoint]")
{
	STATIC_REQUIRE(HyperMath::midpoint(1, 0) == 1);
	STATIC_REQUIRE(HyperMath::midpoint(2, 1) == 2);
	STATIC_REQUIRE(HyperMath::midpoint(3, 2) == 3);
	
	STATIC_REQUIRE(HyperMath::midpoint(-4, 1) == -2);
	STATIC_REQUIRE(HyperMath::midpoint(-5, 2) == -2);
	STATIC_REQUIRE(HyperMath::midpoint(-6, 3) == -2);
	
	STATIC_REQUIRE(HyperMath::midpoint(1.0f, 0.0f) == 0.5f);
	STATIC_REQUIRE(HyperMath::midpoint(2.0f, 1.0f) == 1.5f);
	STATIC_REQUIRE(HyperMath::midpoint(3.0f, 2.0f) == 2.5f);
	
	STATIC_REQUIRE(HyperMath::midpoint(-4.0f, 1.0f) == -1.5f);
	STATIC_REQUIRE(HyperMath::midpoint(-5.0f, 2.0f) == -1.5f);
	STATIC_REQUIRE(HyperMath::midpoint(-6.0f, 3.0f) == -1.5f);
}

TEST_CASE("'factorial' of one number can be calculated with constexpr", "[HyperMath][factorial]")
{
	STATIC_REQUIRE(HyperMath::factorial(1) == 1);
	STATIC_REQUIRE(HyperMath::factorial(2) == 2);
	STATIC_REQUIRE(HyperMath::factorial(3) == 6);
	
	STATIC_REQUIRE(HyperMath::factorial(-4) == 1);
	STATIC_REQUIRE(HyperMath::factorial(-5) == 1);
	STATIC_REQUIRE(HyperMath::factorial(-6) == 1);
}

TEST_CASE("'pow' of two numbers can be calculated with constexpr", "[HyperMath][pow]")
{
	STATIC_REQUIRE(HyperMath::pow(1, 0) == 1);
	STATIC_REQUIRE(HyperMath::pow(2, 1) == 2);
	STATIC_REQUIRE(HyperMath::pow(3, 2) == 9);
	
	STATIC_REQUIRE(HyperMath::pow(-4, 1) == -4);
	STATIC_REQUIRE(HyperMath::pow(-5, 2) == 25);
	STATIC_REQUIRE(HyperMath::pow(-6, 3) == -216);
}
