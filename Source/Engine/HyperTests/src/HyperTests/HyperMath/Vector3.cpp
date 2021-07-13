/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperMath/Vector3.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace HyperMath;

TEST_CASE("CVector3 can be created with default value", "[HyperCore::CVector3]")
{
	CVector3<float> vector;
	
	SECTION("Position")
	{
		REQUIRE(vector.x == 0.0f);
		REQUIRE(vector.y == 0.0f);
		REQUIRE(vector.z == 0.0f);
	}
	
	SECTION("Color")
	{
		REQUIRE(vector.r == 0.0f);
		REQUIRE(vector.g == 0.0f);
		REQUIRE(vector.b == 0.0f);
	}
	
	SECTION("Texture Coordinates")
	{
		REQUIRE(vector.s == 0.0f);
		REQUIRE(vector.t == 0.0f);
		REQUIRE(vector.p == 0.0f);
	}
	
	SECTION("UVW Coordinates")
	{
		REQUIRE(vector.u == 0.0f);
		REQUIRE(vector.v == 0.0f);
		REQUIRE(vector.w == 0.0f);
	}
}

TEST_CASE("CVector3 can be assigned", "[HyperCore::CVector3]")
{
	CVector3<float> vector;

	SECTION("Position")
	{
		vector.x = 1.0f;
		vector.y = 2.0f;
		vector.z = 3.0f;
		
		REQUIRE(vector.x == 1.0f);
		REQUIRE(vector.y == 2.0f);
		REQUIRE(vector.z == 3.0f);
	}
	
	SECTION("Color")
	{
		vector.r = 1.2f;
		vector.g = 2.2f;
		vector.b = 3.2f;
		
		REQUIRE(vector.r == 1.2f);
		REQUIRE(vector.g == 2.2f);
		REQUIRE(vector.b == 3.2f);
	}
	
	SECTION("Texture Coordinates")
	{
		vector.s = 1.4f;
		vector.t = 2.4f;
		vector.p = 3.4f;
		
		REQUIRE(vector.s == 1.4f);
		REQUIRE(vector.t == 2.4f);
		REQUIRE(vector.p == 3.4f);
	}
	
	SECTION("UVW Coordinates")
	{
		vector.u = 1.6f;
		vector.v = 2.6f;
		vector.w = 3.6f;
		
		REQUIRE(vector.u == 1.6f);
		REQUIRE(vector.v == 2.6f);
		REQUIRE(vector.w == 3.6f);
	}
}
