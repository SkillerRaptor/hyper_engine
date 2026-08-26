/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_script/token.hpp"

#include <hyper_core/assertion.hpp>

namespace he::script {

std::string Token::to_string() const
{
    const std::string_view kind = [&]() {
        switch (m_kind) {
        case TokenKind::LeftBrace:
            return "LeftBrace";
        case TokenKind::RightBrace:
            return "RightBrace";
        case TokenKind::LeftParenthesis:
            return "LeftParenthesis";
        case TokenKind::RightParenthesis:
            return "RightParenthesis";
        case TokenKind::Semicolon:
            return "Semicolon";
        case TokenKind::Plus:
            return "Plus";
        case TokenKind::Minus:
            return "Minus";
        case TokenKind::Star:
            return "Star";
        case TokenKind::Slash:
            return "Slash";
        case TokenKind::Equal:
            return "Equal";
        case TokenKind::EqualEqual:
            return "EqualEqual";
        case TokenKind::NotEqual:
            return "NotEqual";
        case TokenKind::Less:
            return "Less";
        case TokenKind::LessEqual:
            return "LessEqual";
        case TokenKind::Greater:
            return "Greater";
        case TokenKind::GreaterEqual:
            return "GreaterEqual";
        case TokenKind::Else:
            return "Else";
        case TokenKind::Fn:
            return "Fn";
        case TokenKind::If:
            return "If";
        case TokenKind::Let:
            return "Let";
        case TokenKind::While:
            return "While";
        case TokenKind::IntegerLiteral:
            return "IntegerLiteral";
        case TokenKind::Identifier:
            return "Identifier";
        case TokenKind::Eof:
            return "Eof";
        }
    }();

    return fmt::format("Token {{ kind: {}, lexeme: \"{}\", line: {}, column: {} }}", kind, m_lexeme, m_line, m_column);
}

u32 Token::integer_value() const
{
    HE_ASSERT(m_kind == TokenKind::IntegerLiteral);

    u32 value = 0;
    const auto [end_ptr, error_code] = std::from_chars(m_lexeme.data(), m_lexeme.data() + m_lexeme.size(), value);

    HE_ASSERT(error_code == std::errc());
    HE_ASSERT(end_ptr == m_lexeme.data() + m_lexeme.size());

    return value;
}

std::string_view Token::string_value() const
{
    HE_ASSERT(m_kind == TokenKind::Identifier);

    return m_lexeme;
}

} // namespace he::script
