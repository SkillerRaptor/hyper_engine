/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include <string_view>

#include <catch2/catch_test_macros.hpp>
#include <fmt/format.h>

#include <hyper_script/debug.hpp>
#include <hyper_script/lexer.hpp>

using namespace he::script;

struct ExpectedToken {
    TokenKind kind;
    std::string_view lexeme;
    uint32_t start_offset;
    uint32_t end_offset;

    ExpectedToken(const TokenKind kind, const std::string_view lexeme, const uint32_t start, const uint32_t end)
        : kind(kind)
        , lexeme(lexeme)
        , start_offset(start)
        , end_offset(end)
    {
    }
};

void test_lexer(std::string source, std::initializer_list<ExpectedToken> expected_tokens)
{
    SourceManager source_manager;
    DiagnosticEngine diagnostic_engine;

    INFO(fmt::format("Input: \"{}\"", source));

    const SourceId source_id = source_manager.add_file("./lexer_tests.hyper", std::move(source));

    Lexer lexer(source_manager, diagnostic_engine, source_id);
    const std::vector<Token> tokens = lexer.lex();

    {
        INFO(fmt::format("Expected {} tokens, but got {}\n", expected_tokens.size(), tokens.size()));
        REQUIRE(tokens.size() == expected_tokens.size());
    }

    size_t index = 0;
    for (const ExpectedToken &expected : expected_tokens) {
        const Token &actual = tokens[index];

        INFO(fmt::format("Assertion failed with token at index #{}\n", index));

        {
            INFO(
                fmt::format(
                    "Expected kind '{}', but got '{}'\n",
                    token_kind_to_string(expected.kind),
                    token_kind_to_string(actual.kind)));
            CHECK(actual.kind == expected.kind);
        }

        {
            INFO(fmt::format("Expected lexeme '{}', but got '{}'\n", expected.lexeme, actual.lexeme));
            CHECK(actual.lexeme == expected.lexeme);
        }

        {
            INFO(fmt::format("Expected source id {}, but got {}\n", source_id, actual.span.source_id));
            CHECK(actual.span.source_id == source_id);
        }

        {
            INFO(
                fmt::format(
                    "Expected offsets {}..{}, but got {}..{}\n",
                    expected.start_offset,
                    expected.end_offset,
                    actual.span.start_offset,
                    actual.span.end_offset));
            CHECK(actual.span.start_offset == expected.start_offset);
            CHECK(actual.span.end_offset == expected.end_offset);
        }

        index += 1;
    }
}

TEST_CASE("Lexing empty source", "[script][lexer]")
{
    test_lexer(
        "",
        {
            { TokenKind::Eof, "", 0, 0 },
        });
}

TEST_CASE("Lexing delimiters", "[script][lexer]")
{
    test_lexer(
        "{ } ( ) ;",
        {
            { TokenKind::LeftBrace, "{", 0, 1 },
            { TokenKind::RightBrace, "}", 2, 3 },
            { TokenKind::LeftParenthesis, "(", 4, 5 },
            { TokenKind::RightParenthesis, ")", 6, 7 },
            { TokenKind::Semicolon, ";", 8, 9 },
            { TokenKind::Eof, "", 9, 9 },
        });
}

TEST_CASE("Lexing operators", "[script][lexer]")
{
    test_lexer(
        "+ - * / = == != < <= > >=",
        {
            { TokenKind::Plus, "+", 0, 1 },
            { TokenKind::Minus, "-", 2, 3 },
            { TokenKind::Star, "*", 4, 5 },
            { TokenKind::Slash, "/", 6, 7 },
            { TokenKind::Equal, "=", 8, 9 },
            { TokenKind::EqualEqual, "==", 10, 12 },
            { TokenKind::NotEqual, "!=", 13, 15 },
            { TokenKind::Less, "<", 16, 17 },
            { TokenKind::LessEqual, "<=", 18, 20 },
            { TokenKind::Greater, ">", 21, 22 },
            { TokenKind::GreaterEqual, ">=", 23, 25 },
            { TokenKind::Eof, "", 25, 25 },
        });
}

TEST_CASE("Lexing keywords", "[script][lexer]")
{
    test_lexer(
        "else fn if let while",
        {
            { TokenKind::Else, "else", 0, 4 },
            { TokenKind::Fn, "fn", 5, 7 },
            { TokenKind::If, "if", 8, 10 },
            { TokenKind::Let, "let", 11, 14 },
            { TokenKind::While, "while", 15, 20 },
            { TokenKind::Eof, "", 20, 20 },
        });
}

TEST_CASE("Lexing integer literals", "[script][lexer]")
{
    test_lexer(
        "0 1 2 3 4 5 6 7 8 9 10 100 1000 10000",
        {
            { TokenKind::IntegerLiteral, "0", 0, 1 },
            { TokenKind::IntegerLiteral, "1", 2, 3 },
            { TokenKind::IntegerLiteral, "2", 4, 5 },
            { TokenKind::IntegerLiteral, "3", 6, 7 },
            { TokenKind::IntegerLiteral, "4", 8, 9 },
            { TokenKind::IntegerLiteral, "5", 10, 11 },
            { TokenKind::IntegerLiteral, "6", 12, 13 },
            { TokenKind::IntegerLiteral, "7", 14, 15 },
            { TokenKind::IntegerLiteral, "8", 16, 17 },
            { TokenKind::IntegerLiteral, "9", 18, 19 },
            { TokenKind::IntegerLiteral, "10", 20, 22 },
            { TokenKind::IntegerLiteral, "100", 23, 26 },
            { TokenKind::IntegerLiteral, "1000", 27, 31 },
            { TokenKind::IntegerLiteral, "10000", 32, 37 },
            { TokenKind::Eof, "", 37, 37 },
        });
}

TEST_CASE("Lexing identifiers", "[script][lexer]")
{
    test_lexer(
        "foo bar",
        {
            { TokenKind::Identifier, "foo", 0, 3 },
            { TokenKind::Identifier, "bar", 4, 7 },
            { TokenKind::Eof, "", 7, 7 },
        });
}
