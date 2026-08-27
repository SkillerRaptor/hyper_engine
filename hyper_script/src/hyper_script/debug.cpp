/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_script/debug.hpp"

#include <hyper_core/assertion.hpp>
#include <hyper_core/logger.hpp>

namespace he::script {

std::string_view token_kind_to_string(const TokenKind kind)
{
    switch (kind) {
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
    default:
        HE_PANIC("Encountered an unknown token kind (token_kind={})", static_cast<u8>(kind));
    }
}

std::string token_to_string(const Token &token)
{
    return fmt::format(
        "Token {{ kind: {}, lexeme: \"{}\", line: {}, column: {} }}",
        token_kind_to_string(token.kind),
        token.lexeme,
        token.line,
        token.column);
}

void dump_tokens(const std::span<const Token> tokens)
{
    if (tokens.empty()) {
        return;
    }

    for (size_t i = 0; i < tokens.size() - 1; ++i) {
        HE_INFO("|- {}", token_to_string(tokens[i]));
    }

    HE_INFO("`- {}", token_to_string(tokens.back()));
}

static void dump_node(const AstNode &node, const std::string &prefix, const bool is_last, const bool is_root = false)
{
    const std::string connector = is_root ? "" : (is_last ? "`- " : "|- ");
    const std::string child_prefix = prefix + (is_root ? "" : (is_last ? "   " : "|  "));

    switch (node.kind()) {
    case AstNodeKind::FunctionDeclaration: {
        const FunctionDeclaration &declaration = static_cast<const FunctionDeclaration &>(node);
        HE_INFO("{}{}FunctionDeclaration {{ identifier: {} }}", prefix, connector, declaration.identifier());
        dump_node(declaration.body(), child_prefix, true);
        break;
    }
    case AstNodeKind::TranslationUnitDeclaration: {
        const TranslationUnitDeclaration &declaration = static_cast<const TranslationUnitDeclaration &>(node);
        HE_INFO("{}{}TranslationUnitDeclaration", prefix, connector);
        if (declaration.declaration_count() > 0) {
            for (usize i = 0; i < declaration.declaration_count() - 1; ++i) {
                dump_node(declaration.declaration(i), child_prefix, false);
            }

            dump_node(declaration.declaration(declaration.declaration_count() - 1), child_prefix, true);
        }
        break;
    }
    case AstNodeKind::VariableDeclaration: {
        const VariableDeclaration &declaration = static_cast<const VariableDeclaration &>(node);
        HE_INFO("{}{}VariableDeclaration {{ identifier: {} }}", prefix, connector, declaration.identifier());
        dump_node(declaration.initializer(), child_prefix, true);
        break;
    }
    case AstNodeKind::BinaryExpression: {
        const BinaryExpression &expression = static_cast<const BinaryExpression &>(node);
        const std::string_view operation = [&]() {
            switch (expression.operation()) {
            case BinaryOperation::Addition:
                return "Addition";
            case BinaryOperation::Subtraction:
                return "Subtraction";
            case BinaryOperation::Multiplication:
                return "Multiplication";
            case BinaryOperation::Division:
                return "Division";
            case BinaryOperation::Equal:
                return "Equal";
            case BinaryOperation::NotEqual:
                return "NotEqual";
            case BinaryOperation::LessThan:
                return "LessThan";
            case BinaryOperation::LessThanOrEqual:
                return "LessThanOrEqual";
            case BinaryOperation::GreaterThan:
                return "GreaterThan";
            case BinaryOperation::GreaterThanOrEqual:
                return "GreaterThanOrEqual";
            default:
                HE_UNREACHABLE();
            }
        }();
        HE_INFO("{}{}BinaryExpression {{ operation: {} }}", prefix, connector, operation);
        dump_node(expression.left(), child_prefix, false);
        dump_node(expression.right(), child_prefix, true);
        break;
    }
    case AstNodeKind::CallExpression: {
        const CallExpression &expression = static_cast<const CallExpression &>(node);
        HE_INFO("{}{}CallExpression {{ identifier: {} }}", prefix, connector, expression.identifier());

        if (expression.argument_count() > 0) {
            for (usize i = 0; i < expression.argument_count() - 1; ++i) {
                dump_node(expression.argument(i), child_prefix, false);
            }

            dump_node(expression.argument(expression.argument_count() - 1), child_prefix, true);
        }
        break;
    }
    case AstNodeKind::VariableExpression: {
        const VariableExpression &expression = static_cast<const VariableExpression &>(node);
        HE_INFO("{}{}VariableExpression {{ identifier: {} }}", prefix, connector, expression.identifier());
        break;
    }
    case AstNodeKind::IntegerLiteral: {
        const IntegerLiteral &literal = static_cast<const IntegerLiteral &>(node);
        HE_INFO("{}{}IntegerLiteral {{ value: {} }}", prefix, connector, literal.value());
        break;
    }
    case AstNodeKind::AssignStatement: {
        const AssignStatement &statement = static_cast<const AssignStatement &>(node);
        HE_INFO("{}{}AssignStatement {{ identifier: {} }}", prefix, connector, statement.identifier());
        dump_node(statement.value(), child_prefix, true);
        break;
    }
    case AstNodeKind::CompoundStatement: {
        const CompoundStatement &statement = static_cast<const CompoundStatement &>(node);
        HE_INFO("{}{}CompoundStatement", prefix, connector);
        if (statement.statement_count() > 0) {
            for (usize i = 0; i < statement.statement_count() - 1; ++i) {
                dump_node(statement.statement(i), child_prefix, false);
            }

            dump_node(statement.statement(statement.statement_count() - 1), child_prefix, true);
        }
        break;
    }
    case AstNodeKind::DeclarationStatement: {
        const DeclarationStatement &statement = static_cast<const DeclarationStatement &>(node);
        HE_INFO("{}{}DeclarationStatement", prefix, connector);
        dump_node(statement.declaration(), child_prefix, true);
        break;
    }
    case AstNodeKind::ExpressionStatement: {
        const ExpressionStatement &statement = static_cast<const ExpressionStatement &>(node);
        HE_INFO("{}{}ExpressionStatement", prefix, connector);
        dump_node(statement.expression(), child_prefix, true);
        break;
    }
    case AstNodeKind::IfStatement: {
        const IfStatement &statement = static_cast<const IfStatement &>(node);
        HE_INFO("{}{}IfStatement", prefix, connector);
        dump_node(statement.condition(), child_prefix, false);
        dump_node(statement.then_body(), child_prefix, statement.else_body() == nullptr);
        if (statement.else_body()) {
            dump_node(*statement.else_body(), child_prefix, true);
        }
        break;
    }
    case AstNodeKind::WhileStatement: {
        const WhileStatement &statement = static_cast<const WhileStatement &>(node);
        HE_INFO("{}{}WhileStatement", prefix, connector);
        dump_node(statement.condition(), child_prefix, false);
        dump_node(statement.body(), child_prefix, true);
        break;
    }
    default:
        HE_PANIC("Encountered an unexpected node while dumping");
    }
}

void dump_ast(const AstNode &node) { dump_node(node, "", true, true); }

} // namespace he::script
