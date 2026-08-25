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
        case TokenKind::Plus:
            return "Plus";
        case TokenKind::Minus:
            return "Minus";
        case TokenKind::Star:
            return "Star";
        case TokenKind::Slash:
            return "Slash";
        case TokenKind::IntegerLiteral:
            return "IntegerLiteral";
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

} // namespace he::script
