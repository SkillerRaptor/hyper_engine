/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include <string_view>

#include <catch2/catch_test_macros.hpp>

#include <hyper_core/string_utils.hpp>

using namespace he;
using namespace std::literals::string_view_literals;

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

TEST_CASE("extract_function_name basic function call", "[extract_function_name]")
{
    constexpr std::string_view expr = "foo()";
    constexpr std::string_view name = string_utils::extract_function_name(expr);

    STATIC_REQUIRE(name == "foo"sv);
}

TEST_CASE("extract_function_name with arguments", "[extract_function_name]")
{
    constexpr std::string_view expr = "foo(int, float)";
    constexpr std::string_view name = string_utils::extract_function_name(expr);

    STATIC_REQUIRE(name == "foo"sv);
}

TEST_CASE("extract_function_name skips leading spaces", "[extract_function_name]")
{
    constexpr std::string_view expr = "    foo()";
    constexpr std::string_view name = string_utils::extract_function_name(expr);

    STATIC_REQUIRE(name == "foo"sv);
}

TEST_CASE("extract_function_name skips tabs", "[extract_function_name]")
{
    constexpr std::string_view expr = "\t\tfoo()";
    constexpr std::string_view name = string_utils::extract_function_name(expr);

    STATIC_REQUIRE(name == "foo"sv);
}

TEST_CASE("extract_function_name stops at space", "[extract_function_name]")
{
    constexpr std::string_view expr = "foo bar()";
    constexpr std::string_view name = string_utils::extract_function_name(expr);

    STATIC_REQUIRE(name == "foo"sv);
}

TEST_CASE("extract_function_name stops at parenthesis", "[extract_function_name]")
{
    constexpr std::string_view expr = "foo(bar)";
    constexpr std::string_view name = string_utils::extract_function_name(expr);

    STATIC_REQUIRE(name == "foo"sv);
}

TEST_CASE("extract_function_name keeps namespaces", "[extract_function_name]")
{
    constexpr std::string_view expr = "ns::cls::func()";
    constexpr std::string_view name = string_utils::extract_function_name(expr);

    STATIC_REQUIRE(name == "ns::cls::func"sv);
}

TEST_CASE("extract_function_name empty input", "[extract_function_name]")
{
    constexpr std::string_view expr = "";
    constexpr std::string_view name = string_utils::extract_function_name(expr);

    STATIC_REQUIRE(name.empty());
}

TEST_CASE("extract_function_name only whitespace", "[extract_function_name]")
{
    constexpr std::string_view expr = "   \t  ";
    constexpr std::string_view name = string_utils::extract_function_name(expr);

    STATIC_REQUIRE(name.empty());
}

TEST_CASE("extract_function_name no parentheses", "[extract_function_name]")
{
    constexpr std::string_view expr = "foo";
    constexpr std::string_view name = string_utils::extract_function_name(expr);

    STATIC_REQUIRE(name == "foo"sv);
}

TEST_CASE("strip_namespace without namespace", "[strip_namespace]")
{
    constexpr std::string_view name = "func";
    constexpr std::string_view stripped = string_utils::strip_namespace(name);

    STATIC_REQUIRE(stripped == "func"sv);
}

TEST_CASE("strip_namespace single namespace", "[strip_namespace]")
{
    constexpr std::string_view name = "ns::func";
    constexpr std::string_view stripped = string_utils::strip_namespace(name);

    STATIC_REQUIRE(stripped == "func"sv);
}

TEST_CASE("strip_namespace multiple namespaces", "[strip_namespace]")
{
    constexpr std::string_view name = "a::b::c::func";
    constexpr std::string_view stripped = string_utils::strip_namespace(name);

    STATIC_REQUIRE(stripped == "func"sv);
}

TEST_CASE("strip_namespace trailing scope operator", "[strip_namespace]")
{
    constexpr std::string_view name = "ns::";
    constexpr std::string_view stripped = string_utils::strip_namespace(name);

    STATIC_REQUIRE(stripped.empty());
}

TEST_CASE("strip_namespace leading scope operator", "[strip_namespace]")
{
    constexpr std::string_view name = "::func";
    constexpr std::string_view stripped = string_utils::strip_namespace(name);

    STATIC_REQUIRE(stripped == "func"sv);
}

TEST_CASE("strip_namespace only scope operators", "[strip_namespace]")
{
    constexpr std::string_view name = "::";
    constexpr std::string_view stripped = string_utils::strip_namespace(name);

    STATIC_REQUIRE(stripped.empty());
}

TEST_CASE("extract + strip namespace combined", "[string_utils]")
{
    constexpr std::string_view expr = "  a::b::func(int)";
    constexpr std::string_view extracted = string_utils::extract_function_name(expr);
    constexpr std::string_view stripped = string_utils::strip_namespace(extracted);

    STATIC_REQUIRE(extracted == "a::b::func"sv);
    STATIC_REQUIRE(stripped == "func"sv);
}
