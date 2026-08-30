/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_script/lexer.hpp"

#include <cctype>

#include <hyper_core/assertion.hpp>

namespace he::script {

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

    tokens.push_back(Token(
        TokenKind::Eof,
        "",
        Span {
            .source_id = m_source_id,
            .start_offset = m_current_index,
            .end_offset = m_current_index,
        }));

    return tokens;
}

std::optional<Token> Lexer::next_token()
{
    skip_whitespaces();

    const size_t start_index = m_current_index;

    if (has_reached_end()) {
        return std::nullopt;
    }

    const char current_character = advance();

    if (std::isalpha(static_cast<unsigned char>(current_character)) || current_character == '_') {
        Token token = lex_identifier(start_index);

        if (token.lexeme == "else") {
            token.kind = TokenKind::Else;
        } else if (token.lexeme == "fn") {
            token.kind = TokenKind::Fn;
        } else if (token.lexeme == "if") {
            token.kind = TokenKind::If;
        } else if (token.lexeme == "let") {
            token.kind = TokenKind::Let;
        } else if (token.lexeme == "while") {
            token.kind = TokenKind::While;
        }

        return token;
    }

    if (std::isdigit(static_cast<unsigned char>(current_character))) {
        return lex_number(start_index);
    }

    auto make_token = [&](const TokenKind kind) {
        const size_t length = m_current_index - start_index;
        return Token(
            kind,
            m_source.substr(start_index, length),
            Span {
                .source_id = m_source_id,
                .start_offset = start_index,
                .end_offset = m_current_index,
            });
    };

    switch (current_character) {
    case '{':
        return make_token(TokenKind::LeftBrace);
    case '}':
        return make_token(TokenKind::RightBrace);
    case '(':
        return make_token(TokenKind::LeftParenthesis);
    case ')':
        return make_token(TokenKind::RightParenthesis);
    case ';':
        return make_token(TokenKind::Semicolon);
    case '+':
        return make_token(TokenKind::Plus);
    case '-':
        return make_token(TokenKind::Minus);
    case '*':
        return make_token(TokenKind::Star);
    case '/':
        return make_token(TokenKind::Slash);
    case '=':
        if (peek() == '=') {
            advance();
            return make_token(TokenKind::EqualEqual);
        }
        return make_token(TokenKind::Equal);
    case '!':
        if (peek() == '=') {
            advance();
            return make_token(TokenKind::NotEqual);
        }

        HE_PANIC();
    case '<':
        if (peek() == '=') {
            advance();
            return make_token(TokenKind::LessEqual);
        }
        return make_token(TokenKind::Less);
    case '>':
        if (peek() == '=') {
            advance();
            return make_token(TokenKind::GreaterEqual);
        }
        return make_token(TokenKind::Greater);
    default:
        m_diagnostic_engine.emit(
            Diagnostic::new_error("unexpected character")
                .with_label(
                    {
                        .source_id = m_source_id,
                        .start_offset = start_index,
                        .end_offset = m_current_index,
                    },
                    std::nullopt,
                    LabelStyle::Primary)
                .with_help("remove the character"));
        return make_token(TokenKind::Error);
    }
}

Token Lexer::lex_number(const size_t start_index)
{
    while (std::isdigit(static_cast<unsigned char>(peek()))) {
        advance();
    }

    const std::string_view value = m_source.substr(start_index, m_current_index - start_index);
    return Token(
        TokenKind::IntegerLiteral,
        value,
        Span {
            .source_id = m_source_id,
            .start_offset = start_index,
            .end_offset = m_current_index,
        });
}

Token Lexer::lex_identifier(const size_t start_index)
{
    // NOTE: We can check if the character is a digit or alphabetic and assume that the first character will always
    // be not a digit, because under normal circumstances this function should be called with a valid first character.
    while (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_') {
        advance();
    }

    const std::string_view value = m_source.substr(start_index, m_current_index - start_index);
    return Token(
        TokenKind::Identifier,
        value,
        Span {
            .source_id = m_source_id,
            .start_offset = start_index,
            .end_offset = m_current_index,
        });
}

char Lexer::advance()
{
    if (has_reached_end()) {
        return '\0';
    }

    const char character = m_source[m_current_index];
    m_current_index += 1;

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

} // namespace he::script
