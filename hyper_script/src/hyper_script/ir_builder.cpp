/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_script/ir_builder.hpp"

#include <ranges>

#include <hyper_core/assertion.hpp>
#include <hyper_core/logger.hpp>

namespace he::script {

std::unique_ptr<IrFunction> IrBuilder::build()
{
    // TODO: Improve this function
    m_ir_function = std::make_unique<IrFunction>(m_function.identifier);

    IrBlock *entry = create_block();
    m_current_block = entry;
    seal_block(entry);

    build_node(m_function);

    return std::move(m_ir_function);
}

IrBlock *IrBuilder::create_block()
{
    std::unique_ptr<IrBlock> block = std::make_unique<IrBlock>(m_next_block_id);
    m_next_block_id += 1;
    IrBlock *block_ptr = block.get();
    m_ir_function->blocks.push_back(std::move(block));
    return block_ptr;
}

IrPhi *IrBuilder::create_phi(IrBlock *block)
{
    std::unique_ptr<IrPhi> phi = std::make_unique<IrPhi>(m_next_value_id, block);
    m_next_value_id += 1;
    IrPhi *phi_ptr = phi.get();
    m_ir_function->values.push_back(std::move(phi));
    block->instructions.insert(block->instructions.begin(), phi_ptr);
    return phi_ptr;
}

IrValue *IrBuilder::build_node(const AstNode &node)
{
    switch (node.kind) {
    case AstNodeKind::FunctionDeclaration: {
        const FunctionDeclaration &declaration = static_cast<const FunctionDeclaration &>(node);
        build_node(*declaration.body);
        return nullptr;
    }
    case AstNodeKind::VariableDeclaration: {
        const VariableDeclaration &declaration = static_cast<const VariableDeclaration &>(node);
        IrValue *value = build_node(*declaration.initializer);
        write_variable(declaration.identifier, m_current_block, value);
        return nullptr;
    }
    case AstNodeKind::BinaryExpression: {
        const BinaryExpression &expression = static_cast<const BinaryExpression &>(node);

        IrValue *left = build_node(*expression.left);
        IrValue *right = build_node(*expression.right);

        switch (expression.operation) {
        case BinaryOperation::Addition:
            return emit<IrAdd>(left, right);
        case BinaryOperation::Subtraction:
            return emit<IrSub>(left, right);
        case BinaryOperation::Multiplication:
            return emit<IrMul>(left, right);
        case BinaryOperation::Division:
            return emit<IrDiv>(left, right);
        case BinaryOperation::Equal:
            return emit<IrCmp>(CompareOperation::Equal, left, right);
        case BinaryOperation::NotEqual:
            return emit<IrCmp>(CompareOperation::NotEqual, left, right);
        case BinaryOperation::LessThan:
            return emit<IrCmp>(CompareOperation::LessThan, left, right);
        case BinaryOperation::LessThanOrEqual:
            return emit<IrCmp>(CompareOperation::LessThanOrEqual, left, right);
        case BinaryOperation::GreaterThan:
            return emit<IrCmp>(CompareOperation::GreaterThan, left, right);
        case BinaryOperation::GreaterThanOrEqual:
            return emit<IrCmp>(CompareOperation::GreaterThanOrEqual, left, right);
        }
    }
    case AstNodeKind::CallExpression: {
        const CallExpression &expression = static_cast<const CallExpression &>(node);
        std::vector<IrValue *> arguments;
        for (const std::unique_ptr<Expression> &argument : expression.arguments) {
            arguments.push_back(build_node(*argument));
        }
        return emit<IrCall>(expression.identifier, std::move(arguments));
    }
    case AstNodeKind::LiteralExpression: {
        const LiteralExpression &expression = static_cast<const LiteralExpression &>(node);
        return build_node(*expression.literal);
    }
    case AstNodeKind::VariableExpression: {
        const VariableExpression &expression = static_cast<const VariableExpression &>(node);
        return read_variable(expression.identifier, m_current_block);
    }
    case AstNodeKind::IntegerLiteral: {
        const IntegerLiteral &literal = static_cast<const IntegerLiteral &>(node);
        return emit<IrConstant>(literal.value);
    }
    case AstNodeKind::AssignStatement: {
        const AssignStatement &statement = static_cast<const AssignStatement &>(node);
        IrValue *value = build_node(*statement.value);
        write_variable(statement.identifier, m_current_block, value);
        return nullptr;
    }
    case AstNodeKind::CompoundStatement: {
        const CompoundStatement &compound_statement = static_cast<const CompoundStatement &>(node);
        for (const std::unique_ptr<Statement> &statement : compound_statement.statements) {
            build_node(*statement);
        }
        return nullptr;
    }
    case AstNodeKind::DeclarationStatement: {
        const DeclarationStatement &statement = static_cast<const DeclarationStatement &>(node);
        return build_node(*statement.declaration);
    }
    case AstNodeKind::ExpressionStatement: {
        const ExpressionStatement &statement = static_cast<const ExpressionStatement &>(node);
        return build_node(*statement.expression);
    }
    case AstNodeKind::IfStatement: {
        const IfStatement &statement = static_cast<const IfStatement &>(node);

        IrValue *condition = build_node(*statement.condition);
        IrBlock *branch_block = m_current_block;

        IrBlock *then_block = create_block();
        IrBlock *else_block = statement.else_body != nullptr ? create_block() : nullptr;
        IrBlock *merge_block = create_block();

        IrBlock *else_target = else_block != nullptr ? else_block : merge_block;
        emit<IrBranch>(condition, then_block, else_target);

        then_block->predecessors.push_back(branch_block);
        seal_block(then_block);
        m_current_block = then_block;
        build_node(*statement.then_body);
        emit<IrBranch>(merge_block);
        merge_block->predecessors.push_back(m_current_block);

        if (statement.else_body != nullptr) {
            else_block->predecessors.push_back(branch_block);
            seal_block(else_block);
            m_current_block = else_block;
            build_node(*statement.else_body);
            emit<IrBranch>(merge_block);
            merge_block->predecessors.push_back(m_current_block);
        } else {
            merge_block->predecessors.push_back(branch_block);
        }

        m_current_block = merge_block;
        seal_block(merge_block);

        return nullptr;
    }
    case AstNodeKind::WhileStatement: {
        const WhileStatement &statement = static_cast<const WhileStatement &>(node);

        IrBlock *preheader_block = m_current_block;

        IrBlock *loop_header_block = create_block();
        emit<IrBranch>(loop_header_block);
        loop_header_block->predecessors.push_back(preheader_block);

        m_current_block = loop_header_block;
        IrValue *condition = build_node(*statement.condition);

        IrBlock *loop_body_block = create_block();
        IrBlock *loop_exit_block = create_block();
        emit<IrBranch>(condition, loop_body_block, loop_exit_block);

        loop_body_block->predecessors.push_back(loop_header_block);
        seal_block(loop_body_block);

        m_current_block = loop_body_block;
        build_node(*statement.body);
        emit<IrBranch>(loop_header_block);
        loop_header_block->predecessors.push_back(m_current_block);
        seal_block(loop_header_block);

        loop_exit_block->predecessors.push_back(loop_header_block);
        seal_block(loop_exit_block);
        m_current_block = loop_exit_block;

        return nullptr;
    }
    default:
        HE_PANIC("Encountered an unexpected node while dumping");
        break;
    }

    HE_PANIC();
}

void IrBuilder::write_variable(const std::string_view variable, IrBlock *block, IrValue *value)
{
    m_current_definitions[variable][block] = value;
}

IrValue *IrBuilder::read_variable(const std::string_view variable, IrBlock *block)
{
    if (m_current_definitions[variable].contains(block)) {
        return m_current_definitions[variable][block];
    }

    return read_variable_recursive(variable, block);
}

IrValue *IrBuilder::read_variable_recursive(const std::string_view variable, IrBlock *block)
{
    IrValue *value = nullptr;

    if (!block->sealed) {
        value = create_phi(block);
        m_incomplete_phis[block][variable] = static_cast<IrPhi *>(value);
    } else if (block->predecessors.size() == 1) {
        value = read_variable(variable, block->predecessors[0]);
    } else if (block->predecessors.empty()) {
        HE_PANIC();
    } else {
        value = create_phi(block);
        write_variable(variable, block, value);
        value = add_phi_operands(variable, static_cast<IrPhi *>(value));
    }
    write_variable(variable, block, value);
    return value;
}

IrValue *IrBuilder::add_phi_operands(const std::string_view variable, IrPhi *phi)
{
    for (IrBlock *predecessor : phi->parent->predecessors) {
        phi->operands.push_back(read_variable(variable, predecessor));
    }

    return try_remove_trivial_phi(phi);
}

IrValue *IrBuilder::try_remove_trivial_phi(IrPhi *phi)
{
    // TODO: Implement
    return phi;
}

void IrBuilder::seal_block(IrBlock *block)
{
    for (const std::string_view variable : m_incomplete_phis[block] | std::views::keys) {
        add_phi_operands(variable, m_incomplete_phis[block][variable]);
    }

    block->sealed = true;
}

} // namespace he::script
