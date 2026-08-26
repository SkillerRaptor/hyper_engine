/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_script/debug.hpp"

namespace he::script {

void dump_tokens(const std::span<const Token> tokens)
{
    if (tokens.empty()) {
        return;
    }

    for (size_t i = 0; i < tokens.size() - 1; ++i) {
        HE_INFO("|- {}", tokens[i].to_string());
    }

    HE_INFO("`- {}", tokens.back().to_string());
}

static void dump_node(const AstNode &node, const std::string &prefix, const bool is_last, const bool is_root = false)
{
    const std::string connector = is_root ? "" : (is_last ? "`- " : "|- ");
    const std::string child_prefix = prefix + (is_root ? "" : (is_last ? "   " : "|  "));

    switch (node.kind()) {
    case AstNodeKind::BinaryExpression: {
        const BinaryExpression &binary_expression = static_cast<const BinaryExpression &>(node);
        const std::string_view operation = [&]() {
            switch (binary_expression.operation()) {
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
        dump_node(*binary_expression.left(), child_prefix, false);
        dump_node(*binary_expression.right(), child_prefix, true);
        break;
    }
    case AstNodeKind::CallExpression: {
        const CallExpression &call_expression = static_cast<const CallExpression &>(node);
        HE_INFO("{}{}CallExpression {{ identifier: {} }}", prefix, connector, call_expression.identifier());

        if (call_expression.argument_count() > 0) {
            for (usize i = 0; i < call_expression.argument_count() - 1; ++i) {
                dump_node(*call_expression.argument(i), child_prefix, false);
            }

            dump_node(*call_expression.argument(call_expression.argument_count() - 1), child_prefix, true);
        }
        break;
    }
    case AstNodeKind::IntegerLiteral: {
        const IntegerLiteral &integer_literal = static_cast<const IntegerLiteral &>(node);
        HE_INFO("{}{}IntegerLiteral {{ value: {} }}", prefix, connector, integer_literal.value());
        break;
    }
    case AstNodeKind::CompoundStatement: {
        const CompoundStatement &compound_statement = static_cast<const CompoundStatement &>(node);
        HE_INFO("{}{}CompoundStatement", prefix, connector);
        if (compound_statement.statement_count() > 0) {
            for (usize i = 0; i < compound_statement.statement_count() - 1; ++i) {
                dump_node(*compound_statement.statement(i), child_prefix, false);
            }

            dump_node(*compound_statement.statement(compound_statement.statement_count() - 1), child_prefix, true);
        }
        break;
    }
    case AstNodeKind::ExpressionStatement: {
        const ExpressionStatement &expression_statement = static_cast<const ExpressionStatement &>(node);
        HE_INFO("{}{}ExpressionStatement", prefix, connector);
        dump_node(*expression_statement.expression(), child_prefix, true);
        break;
    }
    default:
        HE_PANIC("Encountered an unexpected node while dumping");
    }
}

void dump_ast(const AstNode &node) { dump_node(node, "", true, true); }

} // namespace he::script
