/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string_view>

#include <hyper_core/types.hpp>

namespace he::script {

enum class TokenKind : u8 {
    Unknown = 0,

    LeftBrace,
    RightBrace,
    LeftParenthesis,
    RightParenthesis,

    Semicolon,

    Plus,
    Minus,
    Star,
    Slash,

    Equal,
    EqualEqual,
    NotEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,

    Else,
    Fn,
    If,
    Let,
    While,

    IntegerLiteral,
    Identifier,

    Eof,
};

struct Token {
    TokenKind kind = TokenKind::Unknown;
    std::string_view lexeme;
    u32 line = 0;
    u32 column = 0;

    Token(const TokenKind kind, const std::string_view lexeme, const u32 line, const u32 column)
        : kind(kind)
        , lexeme(lexeme)
        , line(line)
        , column(column)
    {
    }
};

} // namespace he::script
