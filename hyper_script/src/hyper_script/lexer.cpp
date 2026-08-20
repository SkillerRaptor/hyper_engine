/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_script/lexer.hpp"

#include <hyper_core/assertion.hpp>

namespace he {

Lexer::Lexer(const std::string_view source)
    : m_source(source)
{
}

std::vector<Token> Lexer::lex()
{
    std::vector<Token> tokens = { };

    while (true) {
        const std::optional<Token> token = next_token();
        if (!token.has_value()) {
            break;
        }

        tokens.push_back(token.value());
    }

    return tokens;
}

std::optional<Token> Lexer::next_token()
{
    skip_whitespaces();

    const u32 start_line = m_line;
    const u32 start_column = m_column;
    const size_t start_index = m_current_index;

    if (has_reached_end()) {
        return std::nullopt;
    }

    const char current_character = advance();
    if (std::isdigit(static_cast<unsigned char>(current_character))) {
        return lex_number(start_line, start_column, start_index);
    }

    auto make_token = [&](const TokenKind kind) {
        const size_t length = m_current_index - start_index;
        return Token(kind, m_source.substr(start_index, length), start_line, start_column, start_index);
    };

    switch (current_character) {
    case '+':
        return make_token(TokenKind::Plus);
    case '-':
        return make_token(TokenKind::Minus);
    case '*':
        return make_token(TokenKind::Star);
    case '/':
        return make_token(TokenKind::Slash);
    default:
        HE_PANIC("Unknown character '{}' at line {}, column {}", current_character, start_line, start_column);
    }
}

Token Lexer::lex_number(const u32 start_line, const u32 start_column, const size_t start_index)
{
    while (std::isdigit(static_cast<unsigned char>(peek()))) {
        advance();
    }

    const std::string_view value = m_source.substr(start_index, m_current_index - start_index);
    return Token(TokenKind::IntegerLiteral, value, start_line, start_column, start_index);
}

char Lexer::advance()
{
    if (has_reached_end()) {
        return '\0';
    }

    const char character = m_source[m_current_index];
    m_current_index += 1;

    if (character == '\n') {
        m_line += 1;
        m_column = 1;
    } else {
        m_column += 1;
    }

    return character;
}

char Lexer::peek() const
{
    if (has_reached_end()) {
        return '\0';
    }

    return m_source[m_current_index];
}

void Lexer::skip_whitespaces()
{
    while (!has_reached_end()) {
        const char c = peek();
        if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
            break;
        }

        advance();
    }
}

bool Lexer::has_reached_end() const { return m_current_index >= m_source.length(); }

} // namespace he
