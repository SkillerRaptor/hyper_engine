/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string_view>

#include <fmt/format.h>

#include <hyper_core/assertion.hpp>
#include <hyper_core/types.hpp>

namespace he {

enum class TokenKind : u8 {
    Invalid,

    Plus,
    Minus,
    Star,
    Slash,

    IntegerLiteral,
};

class Token {
public:
    Token(const TokenKind kind, const std::string_view value, const u32 line, const u32 column, const size_t offset)
        : m_kind(kind)
        , m_value(value)
        , m_line(line)
        , m_column(column)
        , m_offset(offset)
    {
    }

    TokenKind kind() const { return m_kind; }
    std::string_view value() const { return m_value; }
    u32 line() const { return m_line; }
    u32 column() const { return m_column; }
    size_t offset() const { return m_offset; }

private:
    TokenKind m_kind = TokenKind::Invalid;
    std::string_view m_value;
    u32 m_line = 0;
    u32 m_column = 0;
    size_t m_offset = 0;
};

} // namespace he

template <>
struct fmt::formatter<he::TokenKind> : fmt::formatter<std::string_view> {
    auto format(const he::TokenKind &kind, const fmt::format_context &context) const
    {
        const std::string_view kind_string = [kind]() {
            switch (kind) {
            case he::TokenKind::Invalid:
                return "Invalid";
            case he::TokenKind::Plus:
                return "Plus";
            case he::TokenKind::Minus:
                return "Minus";
            case he::TokenKind::Star:
                return "Star";
            case he::TokenKind::Slash:
                return "Slash";
            case he::TokenKind::IntegerLiteral:
                return "IntegerLiteral";
            default:
                HE_UNREACHABLE();
            }
        }();

        return fmt::format_to(context.out(), "{}", kind_string);
    }
};

template <>
struct fmt::formatter<he::Token> : fmt::formatter<std::string_view> {
    auto format(const he::Token &token, const fmt::format_context &context) const
    {
        return fmt::format_to(
            context.out(),
            "{} (\"{}\") [{}:{}]",
            token.kind(),
            token.value(),
            token.line(),
            token.column());
    }
};
