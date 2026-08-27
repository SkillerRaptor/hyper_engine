/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_script/parser.hpp"

#include <hyper_core/assertion.hpp>

namespace he::script {

std::unique_ptr<AstNode> Parser::parse() { return parse_translation_unit_declaration(); }

std::unique_ptr<Declaration> Parser::parse_declaration()
{
    switch (current_token()->kind) {
    case TokenKind::Fn:
        return parse_function_declaration();
    default:
        break;
    }

    HE_PANIC("Unexpected token while parsing declaration");

    return nullptr;
}

std::unique_ptr<Declaration> Parser::parse_function_declaration()
{
    consume(TokenKind::Fn);

    const std::string_view identifier = parse_identifier();

    consume(TokenKind::LeftParenthesis);
    // TODO: Parse arguments
    consume(TokenKind::RightParenthesis);

    std::unique_ptr<Statement> body = parse_compound_statement();

    return std::make_unique<FunctionDeclaration>(identifier, std::move(body));
}

std::unique_ptr<Declaration> Parser::parse_translation_unit_declaration()
{
    std::vector<std::unique_ptr<Declaration>> declarations;

    while (current_token()->kind != TokenKind::Eof) {
        std::unique_ptr<Declaration> declaration = parse_declaration();
        declarations.push_back(std::move(declaration));
    }

    return std::make_unique<TranslationUnitDeclaration>(std::move(declarations));
}

std::unique_ptr<Declaration> Parser::parse_variable_declaration()
{
    consume(TokenKind::Let);

    const std::string_view identifier = parse_identifier();

    consume(TokenKind::Equal);

    std::unique_ptr<Expression> initializer = parse_binary_expression(0);

    consume(TokenKind::Semicolon);

    return std::make_unique<VariableDeclaration>(identifier, std::move(initializer));
}

std::unique_ptr<Expression> Parser::parse_primary_expression()
{
    switch (current_token()->kind) {
    case TokenKind::IntegerLiteral:
        return parse_literal_expression(parse_integer_literal());
    case TokenKind::Identifier:
        if (peek_token()->kind == TokenKind::LeftParenthesis) {
            return parse_call_expression();
        }

        return parse_variable_expression();
    default:
        break;
    }

    HE_PANIC("Unexpected token while parsing primary expression");

    return nullptr;
}

std::unique_ptr<Expression> Parser::parse_binary_expression(const u8 precedence)
{
    std::unique_ptr<Expression> left = parse_primary_expression();

    while (get_operator_precedence(current_token()->kind) > precedence) {
        const TokenKind operation_kind = consume().kind;
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
            case TokenKind::EqualEqual:
                return BinaryOperation::Equal;
            case TokenKind::NotEqual:
                return BinaryOperation::NotEqual;
            case TokenKind::Less:
                return BinaryOperation::LessThan;
            case TokenKind::LessEqual:
                return BinaryOperation::LessThanOrEqual;
            case TokenKind::Greater:
                return BinaryOperation::GreaterThan;
            case TokenKind::GreaterEqual:
                return BinaryOperation::GreaterThanOrEqual;
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
    const std::string_view identifier = parse_identifier();

    consume(TokenKind::LeftParenthesis);

    std::vector<std::unique_ptr<Expression>> arguments;
    if (!match(TokenKind::RightParenthesis)) {
        arguments.push_back(parse_binary_expression(0));
        // TODO: Continue parsing arguments
    }

    consume(TokenKind::RightParenthesis);

    return std::make_unique<CallExpression>(identifier, std::move(arguments));
}

std::unique_ptr<Expression> Parser::parse_literal_expression(std::unique_ptr<Literal> literal)
{
    return std::make_unique<LiteralExpression>(std::move(literal));
}

std::unique_ptr<Expression> Parser::parse_variable_expression()
{
    const std::string_view identifier = parse_identifier();
    return std::make_unique<VariableExpression>(identifier);
}

std::unique_ptr<Literal> Parser::parse_integer_literal()
{
    const Token token = consume(TokenKind::IntegerLiteral);

    u32 value = 0;
    const std::from_chars_result result
        = std::from_chars(token.lexeme.data(), token.lexeme.data() + token.lexeme.size(), value);

    HE_ASSERT(result.ptr == token.lexeme.data() + token.lexeme.size());
    HE_ASSERT(result.ec == std::errc());

    return std::make_unique<IntegerLiteral>(value);
}

std::unique_ptr<Statement> Parser::parse_statement()
{
    switch (current_token()->kind) {
    case TokenKind::If: {
        return parse_if_statement();
    }
    case TokenKind::Let: {
        std::unique_ptr<Declaration> declaration = parse_variable_declaration();
        return parse_declaration_statement(std::move(declaration));
    }
    case TokenKind::While: {
        return parse_while_statement();
    }
    case TokenKind::Identifier: {
        switch (peek_token()->kind) {
        case TokenKind::LeftParenthesis: {
            std::unique_ptr<Expression> expression = parse_call_expression();
            consume(TokenKind::Semicolon);
            return parse_expression_statement(std::move(expression));
        }
        case TokenKind::Equal: {
            return parse_assign_statement();
        }
        default:
            HE_PANIC();
        }
    }
    default:
        break;
    }

    HE_PANIC("Unexpected token while parsing statement");

    return nullptr;
}

std::unique_ptr<Statement> Parser::parse_assign_statement()
{
    const std::string_view identifier = parse_identifier();

    consume(TokenKind::Equal);

    std::unique_ptr<Expression> value = parse_binary_expression(0);

    consume(TokenKind::Semicolon);

    return std::make_unique<AssignStatement>(identifier, std::move(value));
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

std::unique_ptr<Statement> Parser::parse_declaration_statement(std::unique_ptr<Declaration> declaration)
{
    return std::make_unique<DeclarationStatement>(std::move(declaration));
}

std::unique_ptr<Statement> Parser::parse_expression_statement(std::unique_ptr<Expression> expression)
{
    return std::make_unique<ExpressionStatement>(std::move(expression));
}

std::unique_ptr<Statement> Parser::parse_if_statement()
{
    consume(TokenKind::If);
    consume(TokenKind::LeftParenthesis);

    std::unique_ptr<Expression> condition = parse_binary_expression(0);

    consume(TokenKind::RightParenthesis);

    std::unique_ptr<Statement> then_body = parse_compound_statement();

    std::unique_ptr<Statement> else_body = nullptr;
    if (match(TokenKind::Else)) {
        consume(TokenKind::Else);

        else_body = parse_compound_statement();
    }

    return std::make_unique<IfStatement>(std::move(condition), std::move(then_body), std::move(else_body));
}

std::unique_ptr<Statement> Parser::parse_while_statement()
{
    consume(TokenKind::While);
    consume(TokenKind::LeftParenthesis);

    std::unique_ptr<Expression> condition = parse_binary_expression(0);

    consume(TokenKind::RightParenthesis);

    std::unique_ptr<Statement> body = parse_compound_statement();

    return std::make_unique<WhileStatement>(std::move(condition), std::move(body));
}

std::string_view Parser::parse_identifier()
{
    const Token identifier = consume(TokenKind::Identifier);
    return identifier.lexeme;
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
    case TokenKind::EqualEqual:
    case TokenKind::NotEqual:
        return 30;
    case TokenKind::Less:
    case TokenKind::LessEqual:
    case TokenKind::Greater:
    case TokenKind::GreaterEqual:
        return 40;
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

    return token->kind == kind;
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
