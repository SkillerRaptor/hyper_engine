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
        expected(TokenKind::Fn);
        return nullptr;
    }
}

std::unique_ptr<Declaration> Parser::parse_function_declaration()
{
    if (!consume(TokenKind::Fn)) {
        return nullptr;
    }

    const std::optional<std::string_view> identifier = parse_identifier();
    if (!identifier) {
        return nullptr;
    }

    if (!consume(TokenKind::LeftParenthesis)) {
        return nullptr;
    }

    // TODO: Parse arguments

    if (!consume(TokenKind::RightParenthesis)) {
        return nullptr;
    }

    std::unique_ptr<Statement> body = parse_compound_statement();
    if (!body) {
        return nullptr;
    }

    return std::make_unique<FunctionDeclaration>(*identifier, std::move(body));
}

std::unique_ptr<Declaration> Parser::parse_translation_unit_declaration()
{
    std::vector<std::unique_ptr<Declaration>> declarations;

    while (!has_reached_end() && !match(TokenKind::Eof)) {
        std::unique_ptr<Declaration> declaration = parse_declaration();
        if (!declaration) {
            synchronize_declaration();
            continue;
        }

        declarations.push_back(std::move(declaration));
    }

    return std::make_unique<TranslationUnitDeclaration>(std::move(declarations));
}

std::unique_ptr<Declaration> Parser::parse_variable_declaration()
{
    if (!consume(TokenKind::Let)) {
        return nullptr;
    }

    const std::optional<std::string_view> identifier = parse_identifier();
    if (!identifier) {
        return nullptr;
    }

    if (!consume(TokenKind::Equal)) {
        return nullptr;
    }

    std::unique_ptr<Expression> initializer = parse_binary_expression(0);
    if (!initializer) {
        return nullptr;
    }

    if (!consume(TokenKind::Semicolon)) {
        return nullptr;
    }

    return std::make_unique<VariableDeclaration>(*identifier, std::move(initializer));
}

std::unique_ptr<Expression> Parser::parse_primary_expression()
{
    switch (current_token()->kind) {
    case TokenKind::IntegerLiteral: {
        std::unique_ptr<Literal> literal = parse_integer_literal();
        if (!literal) {
            return nullptr;
        }

        return std::make_unique<LiteralExpression>(std::move(literal));
    }
    case TokenKind::Identifier:
        if (peek_token()->kind == TokenKind::LeftParenthesis) {
            return parse_call_expression();
        }

        return parse_variable_expression();
    default:
        expected({ TokenKind::IntegerLiteral, TokenKind::Identifier });
        return nullptr;
    }
}

std::unique_ptr<Expression> Parser::parse_binary_expression(const u8 precedence)
{
    std::unique_ptr<Expression> left = parse_primary_expression();
    if (!left) {
        return nullptr;
    }

    while (get_operator_precedence(current_token()->kind) > precedence) {
        const TokenKind operation_kind = consume().kind;

        const BinaryOperation operation = [&]() {
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
        if (!right) {
            return nullptr;
        }

        left = std::make_unique<BinaryExpression>(operation, std::move(left), std::move(right));
    }

    return left;
}

std::unique_ptr<Expression> Parser::parse_call_expression()
{
    const std::optional<std::string_view> identifier = parse_identifier();
    if (!identifier) {
        return nullptr;
    }

    if (!consume(TokenKind::LeftParenthesis)) {
        return nullptr;
    }

    std::vector<std::unique_ptr<Expression>> arguments;
    if (!match(TokenKind::RightParenthesis)) {
        // TODO: Continue parsing arguments
        std::unique_ptr<Expression> argument = parse_binary_expression(0);
        arguments.push_back(std::move(argument));
    }

    if (!consume(TokenKind::RightParenthesis)) {
        return nullptr;
    }

    return std::make_unique<CallExpression>(*identifier, std::move(arguments));
}

std::unique_ptr<Expression> Parser::parse_variable_expression()
{
    const std::optional<std::string_view> identifier = parse_identifier();
    if (!identifier) {
        return nullptr;
    }

    return std::make_unique<VariableExpression>(*identifier);
}

std::unique_ptr<Literal> Parser::parse_integer_literal()
{
    const std::optional<Token> token = consume(TokenKind::IntegerLiteral);
    if (!token) {
        return nullptr;
    }

    u32 value = 0;
    const std::from_chars_result result
        = std::from_chars(token->lexeme.data(), token->lexeme.data() + token->lexeme.size(), value);

    HE_ASSERT(result.ptr == token->lexeme.data() + token->lexeme.size());
    HE_ASSERT(result.ec == std::errc());

    return std::make_unique<IntegerLiteral>(value);
}

std::unique_ptr<Statement> Parser::parse_statement()
{
    switch (current_token()->kind) {
    case TokenKind::If:
        return parse_if_statement();
    case TokenKind::Let: {
        std::unique_ptr<Declaration> declaration = parse_variable_declaration();
        if (!declaration) {
            return nullptr;
        }

        return std::make_unique<DeclarationStatement>(std::move(declaration));
    }
    case TokenKind::While:
        return parse_while_statement();
    case TokenKind::Identifier: {
        switch (peek_token()->kind) {
        case TokenKind::LeftParenthesis: {
            std::unique_ptr<Expression> expression = parse_call_expression();
            if (!expression) {
                return nullptr;
            }

            if (!consume(TokenKind::Semicolon)) {
                return nullptr;
            }

            return std::make_unique<ExpressionStatement>(std::move(expression));
        }
        case TokenKind::Equal: {
            return parse_assign_statement();
        }
        default:
            expected({ TokenKind::LeftParenthesis, TokenKind::Equal });
            return nullptr;
        }
    }
    default:
        expected({ TokenKind::If, TokenKind::Let, TokenKind::While, TokenKind::Identifier });
        return nullptr;
    }
}

std::unique_ptr<Statement> Parser::parse_assign_statement()
{
    const std::optional<std::string_view> identifier = parse_identifier();
    if (!identifier) {
        return nullptr;
    }

    if (!consume(TokenKind::Equal)) {
        return nullptr;
    }

    std::unique_ptr<Expression> value = parse_binary_expression(0);
    if (!value) {
        return nullptr;
    }

    if (!consume(TokenKind::Semicolon)) {
        return nullptr;
    }

    return std::make_unique<AssignStatement>(*identifier, std::move(value));
}

std::unique_ptr<Statement> Parser::parse_compound_statement()
{
    // TODO: Fix crash if a brace is missing, because it will try to either sync or not skip it, but
    // the problem is that it needs to be depth aware
    if (!consume(TokenKind::LeftBrace)) {
        return nullptr;
    }

    std::vector<std::unique_ptr<Statement>> statements;
    while (!has_reached_end() && !match(TokenKind::RightBrace)) {
        std::unique_ptr<Statement> statement = parse_statement();
        if (!statement) {
            synchronize_statement();
            continue;
        }

        statements.push_back(std::move(statement));
    }

    if (!consume(TokenKind::RightBrace)) {
        return nullptr;
    }

    return std::make_unique<CompoundStatement>(std::move(statements));
}

std::unique_ptr<Statement> Parser::parse_if_statement()
{
    if (!consume(TokenKind::If)) {
        return nullptr;
    }

    bool has_errors = false;

    if (!consume(TokenKind::LeftParenthesis)) {
        has_errors = true;
    }

    std::unique_ptr<Expression> condition = parse_binary_expression(0);
    if (!condition) {
        has_errors = true;
    }

    if (!consume(TokenKind::RightParenthesis)) {
        has_errors = true;
    }

    std::unique_ptr<Statement> then_body = parse_compound_statement();
    if (!then_body) {
        has_errors = true;
    }

    std::unique_ptr<Statement> else_body = nullptr;
    if (match(TokenKind::Else)) {
        consume();

        else_body = parse_compound_statement();
        if (!else_body) {
            has_errors = true;
        }
    }

    if (has_errors) {
        return nullptr;
    }

    return std::make_unique<IfStatement>(std::move(condition), std::move(then_body), std::move(else_body));
}

std::unique_ptr<Statement> Parser::parse_while_statement()
{
    if (!consume(TokenKind::While)) {
        return nullptr;
    }

    bool has_errors = false;

    if (!consume(TokenKind::LeftParenthesis)) {
        has_errors = true;
    }

    std::unique_ptr<Expression> condition = parse_binary_expression(0);
    if (!condition) {
        has_errors = true;
    }

    if (!consume(TokenKind::RightParenthesis)) {
        has_errors = true;
    }

    std::unique_ptr<Statement> body = parse_compound_statement();
    if (!body) {
        has_errors = true;
    }

    if (has_errors) {
        return nullptr;
    }

    return std::make_unique<WhileStatement>(std::move(condition), std::move(body));
}

std::optional<std::string_view> Parser::parse_identifier()
{
    const std::optional<Token> identifier = consume(TokenKind::Identifier);
    if (!identifier) {
        return std::nullopt;
    }

    return identifier->lexeme;
}

void Parser::synchronize_declaration()
{
    while (!has_reached_end()) {
        switch (current_token()->kind) {
        case TokenKind::Fn:
            return;
        default:
            consume();
            break;
        }
    }
}

void Parser::synchronize_statement()
{
    while (!has_reached_end()) {
        switch (current_token()->kind) {
        case TokenKind::Semicolon:
            consume();
            return;
        case TokenKind::RightBrace:
            consume();
            return;
        case TokenKind::If:
        case TokenKind::Let:
        case TokenKind::While:
            return;
        default:
            consume();
            break;
        }
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
    if (!token) {
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

std::optional<Token> Parser::consume(const TokenKind kind)
{
    if (!match(kind)) {
        expected(kind);
        return std::nullopt;
    }

    return consume();
}

bool Parser::has_reached_end() const { return m_current_token_index >= m_tokens.size(); }

void Parser::expected(const TokenKind kind) const
{
    const Token token = *current_token();
    m_diagnostics.emit_error("unexpected token")
        .with_label(token.span, std::nullopt, LabelStyle::Primary)
        .with_note(
            fmt::format("expected '{}', got '{}'", token_kind_to_string(kind), token_kind_to_string(token.kind)));
}

void Parser::expected(const std::initializer_list<TokenKind> token_kinds) const
{
    std::string kinds;

    usize i = 0;
    for (const TokenKind &kind : token_kinds) {
        if (i > 0) {
            if (i == token_kinds.size() - 1) {
                kinds += " or ";
            } else {
                kinds += ", ";
            }
        }

        kinds += fmt::format("'{}'", token_kind_to_string(kind));

        i += 1;
    }

    const Token token = *current_token();
    m_diagnostics.emit_error("unexpected token")
        .with_label(token.span, std::nullopt, LabelStyle::Primary)
        .with_note(fmt::format("expected {}, got '{}'", kinds, token_kind_to_string(token.kind)));
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

std::string_view Parser::token_kind_to_string(const TokenKind kind)
{
    switch (kind) {
    case TokenKind::LeftBrace:
        return "{";
    case TokenKind::RightBrace:
        return "}";
    case TokenKind::LeftParenthesis:
        return "(";
    case TokenKind::RightParenthesis:
        return ")";
    case TokenKind::Semicolon:
        return ";";
    case TokenKind::Plus:
        return "+";
    case TokenKind::Minus:
        return "-";
    case TokenKind::Star:
        return "*";
    case TokenKind::Slash:
        return "/";
    case TokenKind::Equal:
        return "=";
    case TokenKind::EqualEqual:
        return "==";
    case TokenKind::NotEqual:
        return "!=";
    case TokenKind::Less:
        return "<";
    case TokenKind::LessEqual:
        return "<=";
    case TokenKind::Greater:
        return ">";
    case TokenKind::GreaterEqual:
        return ">=";
    case TokenKind::Else:
        return "else";
    case TokenKind::Fn:
        return "fn";
    case TokenKind::If:
        return "if";
    case TokenKind::Let:
        return "let";
    case TokenKind::While:
        return "while";
    case TokenKind::IntegerLiteral:
        return "integer literal";
    case TokenKind::Identifier:
        return "identifier";
    default:
        HE_PANIC();
    }
}

} // namespace he::script
