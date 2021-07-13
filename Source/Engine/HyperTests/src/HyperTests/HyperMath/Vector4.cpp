/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperMath/Vector4.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace HyperMath;

TEST_CASE("CVector4 can be created with default value", "[HyperCore::CVector4]")
{
	CVector4<float> vector;
	
	SECTION("Position")
	{
		REQUIRE(vector.x == 0.0f);
		REQUIRE(vector.y == 0.0f);
		REQUIRE(vector.z == 0.0f);
		REQUIRE(vector.w == 0.0f);
	}
	
	SECTION("Color")
	{
		REQUIRE(vector.r == 0.0f);
		REQUIRE(vector.g == 0.0f);
		REQUIRE(vector.b == 0.0f);
		REQUIRE(vector.a == 0.0f);
	}
	
	SECTION("Texture Coordinates")
	{
		REQUIRE(vector.s == 0.0f);
		REQUIRE(vector.t == 0.0f);
		REQUIRE(vector.p == 0.0f);
		REQUIRE(vector.q == 0.0f);
	}
}

TEST_CASE("CVector4 can be assigned", "[HyperCore::CVector4]")
{
	CVector4<float> vector;

	SECTION("Position")
	{
		vector.x = 1.0f;
		vector.y = 2.0f;
		vector.z = 3.0f;
		vector.w = 4.0f;
		
		REQUIRE(vector.x == 1.0f);
		REQUIRE(vector.y == 2.0f);
		REQUIRE(vector.z == 3.0f);
		REQUIRE(vector.w == 4.0f);
	}
	
	SECTION("Color")
	{
		vector.r = 1.2f;
		vector.g = 2.2f;
		vector.b = 3.2f;
		vector.a = 4.2f;
		
		REQUIRE(vector.r == 1.2f);
		REQUIRE(vector.g == 2.2f);
		REQUIRE(vector.b == 3.2f);
		REQUIRE(vector.a == 4.2f);
	}
	
	SECTION("Texture Coordinates")
	{
		vector.s = 1.4f;
		vector.t = 2.4f;
		vector.p = 3.4f;
		vector.q = 4.4f;
		
		REQUIRE(vector.s == 1.4f);
		REQUIRE(vector.t == 2.4f);
		REQUIRE(vector.p == 3.4f);
		REQUIRE(vector.q == 4.4f);
	}
}
