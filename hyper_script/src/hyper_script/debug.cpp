/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_script/debug.hpp"

#include <ostream>

#include <hyper_core/assertion.hpp>
#include <hyper_core/logger.hpp>

namespace he::script {

std::string span_to_string(const Span &span)
{
    return fmt::format(
        "Span {{ source_id: {}, start_offset: {}, end_offset: {} }}",
        span.source_id,
        span.start_offset,
        span.end_offset);
}

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
        "Token {{ kind: {}, lexeme: \"{}\", span: {} }}",
        token_kind_to_string(token.kind),
        token.lexeme,
        span_to_string(token.span));
}

static void print_lines(const std::string &content)
{
    std::stringstream ss(content);
    std::string line;
    while (std::getline(ss, line)) {
        HE_INFO("{}", line);
    }
}

void dump_tokens(const std::span<const Token> tokens, const bool use_pipes)
{
    std::ostringstream ss;
    dump_tokens(ss, tokens, use_pipes);
    print_lines(ss.str());
}

void dump_ast(const AstNode &node, const bool use_pipes)
{
    std::ostringstream ss;
    dump_ast(ss, node, use_pipes);
    print_lines(ss.str());
}

void dump_ir(const IrFunction &function)
{
    std::ostringstream ss;
    dump_ir(ss, function);
    print_lines(ss.str());
}

void dump_tokens(std::ostream &stream, const std::span<const Token> tokens, const bool use_pipes)
{
    if (tokens.empty()) {
        return;
    }

    for (size_t i = 0; i < tokens.size(); ++i) {
        std::string prefix;
        if (use_pipes) {
            prefix = (i == tokens.size() - 1) ? "`- " : "|- ";
        }
        stream << fmt::format("{}{}\n", prefix, token_to_string(tokens[i]));
    }
}

static void dump_node(
    std::ostream &stream,
    const AstNode &node,
    const std::string &prefix,
    const bool is_last,
    const bool use_pipes,
    const bool is_root = false)
{
    std::string connector;
    std::string child_prefix;

    if (use_pipes) {
        connector = is_root ? "" : (is_last ? "`- " : "|- ");
        child_prefix = prefix + (is_root ? "" : (is_last ? "   " : "|  "));
    } else {
        connector = is_root ? "" : "   ";
        child_prefix = prefix + (is_root ? "" : "   ");
    }

    switch (node.kind) {
    case AstNodeKind::FunctionDeclaration: {
        const FunctionDeclaration &declaration = static_cast<const FunctionDeclaration &>(node);
        stream
            << fmt::format("{}{}FunctionDeclaration {{ identifier: {} }}\n", prefix, connector, declaration.identifier);
        dump_node(stream, *declaration.body, child_prefix, true, use_pipes);
        break;
    }
    case AstNodeKind::TranslationUnitDeclaration: {
        const TranslationUnitDeclaration &declaration = static_cast<const TranslationUnitDeclaration &>(node);
        stream << fmt::format("{}{}TranslationUnitDeclaration\n", prefix, connector);
        if (declaration.declarations.empty()) {
            break;
        }
        for (usize i = 0; i < declaration.declarations.size() - 1; ++i) {
            dump_node(stream, *declaration.declarations[i], child_prefix, false, use_pipes);
        }
        dump_node(stream, *declaration.declarations.back(), child_prefix, true, use_pipes);
        break;
    }
    case AstNodeKind::VariableDeclaration: {
        const VariableDeclaration &declaration = static_cast<const VariableDeclaration &>(node);
        stream
            << fmt::format("{}{}VariableDeclaration {{ identifier: {} }}\n", prefix, connector, declaration.identifier);
        dump_node(stream, *declaration.initializer, child_prefix, true, use_pipes);
        break;
    }
    case AstNodeKind::BinaryExpression: {
        const BinaryExpression &expression = static_cast<const BinaryExpression &>(node);
        const std::string_view operation = [&]() {
            switch (expression.operation) {
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
        stream << fmt::format("{}{}BinaryExpression {{ operation: {} }}\n", prefix, connector, operation);
        dump_node(stream, *expression.left, child_prefix, false, use_pipes);
        dump_node(stream, *expression.right, child_prefix, true, use_pipes);
        break;
    }
    case AstNodeKind::CallExpression: {
        const CallExpression &expression = static_cast<const CallExpression &>(node);
        stream << fmt::format("{}{}CallExpression {{ identifier: {} }}\n", prefix, connector, expression.identifier);
        if (expression.arguments.empty()) {
            break;
        }
        for (usize i = 0; i < expression.arguments.size() - 1; ++i) {
            dump_node(stream, *expression.arguments[i], child_prefix, false, use_pipes);
        }
        dump_node(stream, *expression.arguments.back(), child_prefix, true, use_pipes);
        break;
    }
    case AstNodeKind::LiteralExpression: {
        const LiteralExpression &expression = static_cast<const LiteralExpression &>(node);
        stream << fmt::format("{}{}LiteralExpression\n", prefix, connector);
        dump_node(stream, *expression.literal, child_prefix, true, use_pipes);
        break;
    }
    case AstNodeKind::VariableExpression: {
        const VariableExpression &expression = static_cast<const VariableExpression &>(node);
        stream
            << fmt::format("{}{}VariableExpression {{ identifier: {} }}\n", prefix, connector, expression.identifier);
        break;
    }
    case AstNodeKind::IntegerLiteral: {
        const IntegerLiteral &literal = static_cast<const IntegerLiteral &>(node);
        stream << fmt::format("{}{}IntegerLiteral {{ value: {} }}\n", prefix, connector, literal.value);
        break;
    }
    case AstNodeKind::AssignStatement: {
        const AssignStatement &statement = static_cast<const AssignStatement &>(node);
        stream << fmt::format("{}{}AssignStatement {{ identifier: {} }}\n", prefix, connector, statement.identifier);
        dump_node(stream, *statement.value, child_prefix, true, use_pipes);
        break;
    }
    case AstNodeKind::CompoundStatement: {
        const CompoundStatement &statement = static_cast<const CompoundStatement &>(node);
        stream << fmt::format("{}{}CompoundStatement\n", prefix, connector);
        if (statement.statements.empty()) {
            break;
        }
        for (usize i = 0; i < statement.statements.size() - 1; ++i) {
            dump_node(stream, *statement.statements[i], child_prefix, false, use_pipes);
        }
        dump_node(stream, *statement.statements.back(), child_prefix, true, use_pipes);
        break;
    }
    case AstNodeKind::DeclarationStatement: {
        const DeclarationStatement &statement = static_cast<const DeclarationStatement &>(node);
        stream << fmt::format("{}{}DeclarationStatement\n", prefix, connector);
        dump_node(stream, *statement.declaration, child_prefix, true, use_pipes);
        break;
    }
    case AstNodeKind::ExpressionStatement: {
        const ExpressionStatement &statement = static_cast<const ExpressionStatement &>(node);
        stream << fmt::format("{}{}ExpressionStatement\n", prefix, connector);
        dump_node(stream, *statement.expression, child_prefix, true, use_pipes);
        break;
    }
    case AstNodeKind::IfStatement: {
        const IfStatement &statement = static_cast<const IfStatement &>(node);
        stream << fmt::format("{}{}IfStatement\n", prefix, connector);
        dump_node(stream, *statement.condition, child_prefix, false, use_pipes);
        dump_node(stream, *statement.then_body, child_prefix, statement.else_body == nullptr, use_pipes);
        if (statement.else_body) {
            dump_node(stream, *statement.else_body, child_prefix, true, use_pipes);
        }
        break;
    }
    case AstNodeKind::WhileStatement: {
        const WhileStatement &statement = static_cast<const WhileStatement &>(node);
        stream << fmt::format("{}{}WhileStatement\n", prefix, connector);
        dump_node(stream, *statement.condition, child_prefix, false, use_pipes);
        dump_node(stream, *statement.body, child_prefix, true, use_pipes);
        break;
    }
    default:
        HE_PANIC("Encountered an unexpected node while dumping");
    }
}

void dump_ast(std::ostream &stream, const AstNode &node, const bool use_pipes)
{
    dump_node(stream, node, "", true, use_pipes, true);
}

void dump_ir(std::ostream &stream, const IrFunction &function)
{
    stream << fmt::format("define @{}() {{\n", function.name);

    for (usize i = 0; i < function.blocks.size(); ++i) {
        const std::unique_ptr<IrBlock> &block = function.blocks[i];

        if (block->id > 0) {
            stream << "\n";
        }
        stream << fmt::format("block_{}:\n", block->id);

        for (const IrValue *instruction : block->instructions) {
            switch (instruction->kind) {
            case IrValueKind::Unknown:
                break;
            case IrValueKind::Constant: {
                const IrConstant *constant = static_cast<const IrConstant *>(instruction);
                stream << fmt::format("  %{} = iconst {}\n", instruction->id, constant->value);
                break;
            }
            case IrValueKind::Addition: {
                const IrAdd *add = static_cast<const IrAdd *>(instruction);
                stream << fmt::format("  %{} = iadd %{} %{}\n", instruction->id, add->left->id, add->right->id);
                break;
            }
            case IrValueKind::Subtraction: {
                const IrSub *sub = static_cast<const IrSub *>(instruction);
                stream << fmt::format("  %{} = isub %{} %{}\n", instruction->id, sub->left->id, sub->right->id);
                break;
            }
            case IrValueKind::Multiplication: {
                const IrMul *mul = static_cast<const IrMul *>(instruction);
                stream << fmt::format("  %{} = imul %{} %{}\n", instruction->id, mul->left->id, mul->right->id);
                break;
            }
            case IrValueKind::Division: {
                const IrDiv *div = static_cast<const IrDiv *>(instruction);
                stream << fmt::format("  %{} = idiv %{} %{}\n", instruction->id, div->left->id, div->right->id);
                break;
            }
            case IrValueKind::Compare: {
                const IrCmp *cmp = static_cast<const IrCmp *>(instruction);
                std::string_view operation_string;
                switch (cmp->operation) {
                case CompareOperation::Equal:
                    operation_string = "eq";
                    break;
                case CompareOperation::NotEqual:
                    operation_string = "ne";
                    break;
                case CompareOperation::LessThan:
                    operation_string = "lt";
                    break;
                case CompareOperation::LessThanOrEqual:
                    operation_string = "le";
                    break;
                case CompareOperation::GreaterThan:
                    operation_string = "gt";
                    break;
                case CompareOperation::GreaterThanOrEqual:
                    operation_string = "ge";
                    break;
                }
                stream << fmt::format(
                    "  %{} = icmp {} %{} %{}\n",
                    instruction->id,
                    operation_string,
                    cmp->left->id,
                    cmp->right->id);
                break;
            }
            case IrValueKind::Branch: {
                const IrBranch *branch = static_cast<const IrBranch *>(instruction);
                if (branch->condition != nullptr) {
                    stream << fmt::format(
                        "  br %{}, %block_{}, %block_{}\n",
                        branch->condition->id,
                        branch->true_target->id,
                        branch->false_target->id);
                } else {
                    stream << fmt::format("  br %block_{}\n", branch->true_target->id);
                }
                break;
            }
            case IrValueKind::Call: {
                const IrCall *call = static_cast<const IrCall *>(instruction);
                std::string arguments;
                for (usize j = 0; j < call->arguments.size(); ++j) {
                    arguments += fmt::format("%{}", call->arguments[j]->id);
                    if (j + 1 < call->arguments.size()) {
                        arguments += ", ";
                    }
                }
                stream << fmt::format("  %{} = call @{}({})\n", instruction->id, call->callee, arguments);
                break;
            }
            case IrValueKind::Phi: {
                const IrPhi *phi = static_cast<const IrPhi *>(instruction);
                std::string operands;
                for (usize j = 0; j < phi->operands.size(); ++j) {
                    operands
                        += fmt::format("[ %{}, %block_{} ]", phi->operands[j]->id, phi->parent->predecessors[j]->id);
                    if (j + 1 < phi->operands.size()) {
                        operands += ", ";
                    }
                }
                stream << fmt::format("  %{} = φ {}\n", instruction->id, operands);
                break;
            }
            case IrValueKind::Undef:
                break;
            }
        }
    }

    stream << "}\n";
}

} // namespace he::script
