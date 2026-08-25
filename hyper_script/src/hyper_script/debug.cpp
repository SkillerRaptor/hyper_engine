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
            default:
                HE_UNREACHABLE();
            }
        }();
        HE_INFO("{}{}BinaryExpression {{ operation: {} }}", prefix, connector, operation);
        dump_node(*binary_expression.left(), child_prefix, false);
        dump_node(*binary_expression.right(), child_prefix, true);
        break;
    }
    case AstNodeKind::IntegerLiteral: {
        const IntegerLiteral &integer_literal = static_cast<const IntegerLiteral &>(node);
        HE_INFO("{}{}IntegerLiteral {{ value: {} }}", prefix, connector, integer_literal.value());
        break;
    }
    default:
        HE_PANIC();
    }
}

void dump_ast(const AstNode &node) { dump_node(node, "", true, true); }

} // namespace he::script
