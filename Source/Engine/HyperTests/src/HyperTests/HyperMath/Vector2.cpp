/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperMath/Vector2.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace HyperMath;

TEST_CASE("CVector2 can be created with default value", "[HyperCore::CVector2]")
{
	CVector2<float> vector;
	
	SECTION("Position")
	{
		REQUIRE(vector.x == 0.0f);
		REQUIRE(vector.y == 0.0f);
	}
	
	SECTION("Texture Coordinates")
	{
		REQUIRE(vector.s == 0.0f);
		REQUIRE(vector.t == 0.0f);
	}
	
	SECTION("UV Coordinates")
	{
		REQUIRE(vector.u == 0.0f);
		REQUIRE(vector.v == 0.0f);
	}
}

TEST_CASE("CVector2 can be assigned", "[HyperCore::CVector2]")
{
	CVector2<float> vector;

	SECTION("Position")
	{
		vector.x = 1.0f;
		vector.y = 2.0f;
		
		REQUIRE(vector.x == 1.0f);
		REQUIRE(vector.y == 2.0f);
	}
	
	SECTION("Texture Coordinates")
	{
		vector.s = 1.2f;
		vector.t = 2.2f;
		
		REQUIRE(vector.s == 1.2f);
		REQUIRE(vector.t == 2.2f);
	}
	
	SECTION("UV Coordinates")
	{
		vector.u = 1.4f;
		vector.v = 2.4f;
		
		REQUIRE(vector.u == 1.4f);
		REQUIRE(vector.v == 2.4f);
	}
}
