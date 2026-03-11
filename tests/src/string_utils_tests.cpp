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

TEST_CASE("to_string and to_wstring convert empty strings", "[string_utils]")
{
    REQUIRE(string_utils::to_string(L"").empty());
    REQUIRE(string_utils::to_wstring("").empty());
}

TEST_CASE("to_string and to_wstring convert a single ASCII character", "[string_utils]")
{
    REQUIRE(string_utils::to_string(L"A") == "A");
    REQUIRE(string_utils::to_wstring("A") == L"A");
}

TEST_CASE("to_string and to_wstring convert ASCII strings", "[string_utils]")
{
    REQUIRE(string_utils::to_string(L"Hello, world!") == "Hello, world!");
    REQUIRE(string_utils::to_wstring("Hello, world!") == L"Hello, world!");
}

TEST_CASE("to_string round-trips Unicode BMP characters", "[string_utils]")
{
    const std::wstring wide = L"Grüße, 世界";
    REQUIRE(string_utils::to_wstring(string_utils::to_string(wide)) == wide);
}

TEST_CASE("to_string round-trips non-BMP Unicode characters (emoji)", "[string_utils]")
{
    const std::wstring wide = L"Hello 🌍🚀";
    REQUIRE(string_utils::to_wstring(string_utils::to_string(wide)) == wide);
}

TEST_CASE("to_wstring round-trips a UTF-8 narrow string", "[string_utils]")
{
    const std::string narrow = "Test äöü 世界";
    REQUIRE(string_utils::to_string(string_utils::to_wstring(narrow)) == narrow);
}

TEST_CASE("to_string and to_wstring preserve embedded null characters", "[string_utils]")
{
    const std::string narrow(std::string("abc\0def", 7));
    const std::wstring wide = string_utils::to_wstring(narrow);
    const std::string round_trip = string_utils::to_string(wide);

    REQUIRE(round_trip.size() == narrow.size());
    REQUIRE(round_trip == narrow);
}

TEST_CASE("extract_function_name returns empty for empty input", "[extract_function_name]")
{
    STATIC_REQUIRE(string_utils::extract_function_name(""sv).empty());
}

TEST_CASE("extract_function_name returns empty for whitespace-only input", "[extract_function_name]")
{
    STATIC_REQUIRE(string_utils::extract_function_name("   \t  "sv).empty());
}

TEST_CASE("extract_function_name extracts a plain name without parentheses", "[extract_function_name]")
{
    STATIC_REQUIRE(string_utils::extract_function_name("foo"sv) == "foo"sv);
}

TEST_CASE("extract_function_name stops at opening parenthesis", "[extract_function_name]")
{
    STATIC_REQUIRE(string_utils::extract_function_name("foo()"sv) == "foo"sv);
    STATIC_REQUIRE(string_utils::extract_function_name("foo(int, float)"sv) == "foo"sv);
}

TEST_CASE("extract_function_name stops at the first space", "[extract_function_name]")
{
    STATIC_REQUIRE(string_utils::extract_function_name("foo bar()"sv) == "foo"sv);
}

TEST_CASE("extract_function_name skips leading spaces", "[extract_function_name]")
{
    STATIC_REQUIRE(string_utils::extract_function_name("    foo()"sv) == "foo"sv);
}

TEST_CASE("extract_function_name skips leading tabs", "[extract_function_name]")
{
    STATIC_REQUIRE(string_utils::extract_function_name("\t\tfoo()"sv) == "foo"sv);
}

TEST_CASE("extract_function_name skips mixed leading whitespace", "[extract_function_name]")
{
    STATIC_REQUIRE(string_utils::extract_function_name(" \t foo()"sv) == "foo"sv);
}

TEST_CASE("extract_function_name preserves namespace-qualified names", "[extract_function_name]")
{
    STATIC_REQUIRE(string_utils::extract_function_name("ns::cls::func()"sv) == "ns::cls::func"sv);
}

TEST_CASE("strip_namespace returns the name unchanged when there is no namespace", "[strip_namespace]")
{
    STATIC_REQUIRE(string_utils::strip_namespace("func"sv) == "func"sv);
}

TEST_CASE("strip_namespace strips a single namespace prefix", "[strip_namespace]")
{
    STATIC_REQUIRE(string_utils::strip_namespace("ns::func"sv) == "func"sv);
}

TEST_CASE("strip_namespace strips multiple nested namespace prefixes", "[strip_namespace]")
{
    STATIC_REQUIRE(string_utils::strip_namespace("a::b::c::func"sv) == "func"sv);
}

TEST_CASE("strip_namespace returns empty for a trailing scope operator", "[strip_namespace]")
{
    STATIC_REQUIRE(string_utils::strip_namespace("ns::"sv).empty());
}

TEST_CASE("strip_namespace strips a leading scope operator", "[strip_namespace]")
{
    STATIC_REQUIRE(string_utils::strip_namespace("::func"sv) == "func"sv);
}

TEST_CASE("strip_namespace returns empty for only scope operators", "[strip_namespace]")
{
    STATIC_REQUIRE(string_utils::strip_namespace("::"sv).empty());
}

TEST_CASE("strip_namespace returns empty for empty input", "[strip_namespace]")
{
    STATIC_REQUIRE(string_utils::strip_namespace(""sv).empty());
}

TEST_CASE(
    "extract_function_name then strip_namespace yields the bare function name",
    "[extract_function_name][strip_namespace]")
{
    constexpr std::string_view expr = "  a::b::func(int)";
    constexpr std::string_view extracted = string_utils::extract_function_name(expr);
    constexpr std::string_view stripped = string_utils::strip_namespace(extracted);

    STATIC_REQUIRE(extracted == "a::b::func"sv);
    STATIC_REQUIRE(stripped == "func"sv);
}

TEST_CASE(
    "extract_function_name then strip_namespace on an unqualified name is a no-op",
    "[extract_function_name][strip_namespace]")
{
    constexpr std::string_view expr = "func(int)";
    constexpr std::string_view extracted = string_utils::extract_function_name(expr);
    constexpr std::string_view stripped = string_utils::strip_namespace(extracted);

    STATIC_REQUIRE(extracted == "func"sv);
    STATIC_REQUIRE(stripped == "func"sv);
}
