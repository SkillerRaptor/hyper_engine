/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include <catch2/catch_test_macros.hpp>

#include <hyper_core/string_utils.hpp>

using namespace he;

TEST_CASE("string_utils converts empty strings", "[string_utils]")
{
    const std::wstring empty_wide;
    const std::string empty_narrow;

    const std::string narrow = string_utils::to_string(empty_wide);
    const std::wstring wide = string_utils::to_wstring(empty_narrow);

    REQUIRE(narrow.empty());
    REQUIRE(wide.empty());
}

TEST_CASE("string_utils converts ASCII strings", "[string_utils]")
{
    const std::wstring wide = L"Hello, world!";
    const std::string narrow = "Hello, world!";

    const std::string converted_narrow = string_utils::to_string(wide);
    const std::wstring converted_wide = string_utils::to_wstring(narrow);

    REQUIRE(converted_narrow == narrow);
    REQUIRE(converted_wide == wide);
}

TEST_CASE("string_utils converts Unicode BMP characters", "[string_utils]")
{
    const std::wstring wide = L"Grüße, 世界";
    const std::string narrow = string_utils::to_string(wide);
    const std::wstring round_trip = string_utils::to_wstring(narrow);

    REQUIRE(round_trip == wide);
}

TEST_CASE("string_utils converts non-BMP Unicode characters", "[string_utils]")
{
    const std::wstring wide = L"Hello 🌍🚀";
    const std::string narrow = string_utils::to_string(wide);
    const std::wstring round_trip = string_utils::to_wstring(narrow);

    REQUIRE(round_trip == wide);
}

TEST_CASE("string_utils narrow round-trip stability", "[string_utils]")
{
    const std::string original = "Test äöü 世界";

    const std::wstring wide = string_utils::to_wstring(original);
    const std::string result = string_utils::to_string(wide);

    REQUIRE(result == original);
}

TEST_CASE("string_utils handles embedded null characters", "[string_utils]")
{
    const std::string narrow = std::string("abc\0def", 7);
    const std::wstring wide = string_utils::to_wstring(narrow);
    const std::string round_trip = string_utils::to_string(wide);

    REQUIRE(round_trip.size() == narrow.size());
    REQUIRE(round_trip == narrow);
}
