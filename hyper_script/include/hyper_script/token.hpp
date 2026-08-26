/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>
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

    // Keywords
    Else,
    Fn,
    If,
    While,

    IntegerLiteral,
    Identifier,

    Eof,
};

class Token {
public:
    Token(const TokenKind kind, const std::string_view lexeme, const u32 line, const u32 column, const size_t offset)
        : m_kind(kind)
        , m_lexeme(lexeme)
        , m_line(line)
        , m_column(column)
        , m_offset(offset)
    {
    }

    std::string to_string() const;

    u32 integer_value() const;
    std::string_view string_value() const;

    TokenKind kind() const { return m_kind; }
    std::string_view lexeme() const { return m_lexeme; }
    u32 line() const { return m_line; }
    u32 column() const { return m_column; }
    size_t offset() const { return m_offset; }

private:
    TokenKind m_kind;
    std::string_view m_lexeme;
    u32 m_line;
    u32 m_column;
    size_t m_offset;
};

} // namespace he::script
