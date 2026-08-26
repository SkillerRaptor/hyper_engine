/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_script/parser.hpp"

#include <hyper_core/assertion.hpp>

namespace he::script {

std::unique_ptr<AstNode> Parser::parse() { return parse_compound_statement(); }

std::unique_ptr<Expression> Parser::parse_primary_expression()
{
    switch (current_token()->kind()) {
    case TokenKind::IntegerLiteral:
        return parse_integer_literal();
    default:
        break;
    }

    HE_ERROR("Unexpected token while parsing primary expression");

    return nullptr;
}

std::unique_ptr<Expression> Parser::parse_binary_expression(const u8 precedence)
{
    std::unique_ptr<Expression> left = parse_primary_expression();

    while (get_operator_precedence(current_token()->kind()) > precedence) {
        const TokenKind operation_kind = consume().kind();
        const BinaryOperation operation = [operation_kind]() {
            switch (operation_kind) {
            case TokenKind::Plus:
                return BinaryOperation::Addition;
            case TokenKind::Minus:
                return BinaryOperation::Subtraction;
            case TokenKind::Star:
                return BinaryOperation::Multiplication;
            case TokenKind::Slash:
                return BinaryOperation::Division;
            default:
                HE_UNREACHABLE();
            }
        }();

        const u8 current_precedence = get_operator_precedence(operation_kind);
        std::unique_ptr<Expression> right = parse_binary_expression(current_precedence);

        left = std::make_unique<BinaryExpression>(operation, std::move(left), std::move(right));
    }

    return left;
}

std::unique_ptr<Expression> Parser::parse_call_expression()
{
    const std::string_view identifier = consume(TokenKind::Identifier).lexeme();

    consume(TokenKind::LeftParenthesis);

    std::vector<std::unique_ptr<Expression>> arguments;
    if (!match(TokenKind::RightParenthesis)) {
        arguments.push_back(parse_binary_expression(0));
    }

    consume(TokenKind::RightParenthesis);

    return std::make_unique<CallExpression>(identifier, std::move(arguments));
}

std::unique_ptr<Literal> Parser::parse_integer_literal()
{
    const Token token = consume(TokenKind::IntegerLiteral);
    return std::make_unique<IntegerLiteral>(token.integer_value());
}

std::unique_ptr<Statement> Parser::parse_statement()
{
    switch (current_token()->kind()) {
    case TokenKind::Identifier: {
        std::unique_ptr<Expression> expression = parse_call_expression();
        consume(TokenKind::Semicolon);
        return parse_expression_statement(std::move(expression));
    }
    default:
        break;
    }

    HE_ERROR("Unexpected token while parsing statement");

    return nullptr;
}

std::unique_ptr<Statement> Parser::parse_compound_statement()
{
    consume(TokenKind::LeftBrace);

    std::vector<std::unique_ptr<Statement>> statements;
    while (!match(TokenKind::RightBrace)) {
        std::unique_ptr<Statement> statement = parse_statement();
        statements.push_back(std::move(statement));
    }

    consume(TokenKind::RightBrace);

    return std::make_unique<CompoundStatement>(std::move(statements));
}

std::unique_ptr<Statement> Parser::parse_expression_statement(std::unique_ptr<Expression> expression)
{
    return std::make_unique<ExpressionStatement>(std::move(expression));
}

u8 Parser::get_operator_precedence(const TokenKind kind)
{
    switch (kind) {
    case TokenKind::Plus:
    case TokenKind::Minus:
        return 10;
    case TokenKind::Star:
    case TokenKind::Slash:
        return 20;
    default:
        return 0;
    }
}

std::optional<Token> Parser::current_token() const
{
    if (has_reached_end()) {
        return std::nullopt;
    }

    return m_tokens[m_current_token_index];
}

std::optional<Token> Parser::peek_token() const
{
    if (m_current_token_index + 1 >= m_tokens.size()) {
        return std::nullopt;
    }

    return m_tokens[m_current_token_index + 1];
}

bool Parser::match(const TokenKind kind) const
{
    const std::optional<Token> token = current_token();
    if (!token.has_value()) {
        return false;
    }

    return token->kind() == kind;
}

Token Parser::consume()
{
    if (m_current_token_index < m_tokens.size()) {
        m_current_token_index += 1;
    }

    return m_tokens[m_current_token_index - 1];
}

Token Parser::consume(const TokenKind kind)
{
    HE_ASSERT(match(kind));
    return consume();
}

bool Parser::has_reached_end() const { return m_current_token_index >= m_tokens.size(); }

} // namespace he::script
