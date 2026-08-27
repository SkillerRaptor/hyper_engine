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
    TokenKind kind = TokenKind::Eof;
    std::string_view lexeme;
    u32 line = 0;
    u32 column = 0;
};

} // namespace he::script
