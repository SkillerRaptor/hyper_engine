/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string_view>

#include <hyper_core/types.hpp>

#include "hyper_script/diagnostics.hpp"

namespace he::script {

enum class TokenKind : u8 {
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
    Error,
};

struct Token {
    TokenKind kind;
    std::string_view lexeme;
    Span span;

    Token(const TokenKind kind, const std::string_view lexeme, const Span span)
        : kind(kind)
        , lexeme(lexeme)
        , span(span)
    {
    }
};

} // namespace he::script
