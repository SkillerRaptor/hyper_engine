/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <hyper_core/assertion.hpp>

namespace he::script {

enum class AstNodeKind : u8 {
    // Declarations

    // Expressions
    BinaryExpression,
    CallExpression,

    // Literals
    IntegerLiteral,

    // Statements
    CompoundStatement,
    ExpressionStatement,
};

class AstNode {
public:
    virtual ~AstNode() = default;

    virtual constexpr AstNodeKind kind() const = 0;
};

class Declaration : public AstNode {
public:
};

class Expression : public AstNode {
public:
};

enum class BinaryOperation : u8 {
    Addition,
    Subtraction,
    Multiplication,
    Division,
    Equal,
    NotEqual,
    LessThan,
    LessThanOrEqual,
    GreaterThan,
    GreaterThanOrEqual,
};

class BinaryExpression final : public Expression {
public:
    BinaryExpression(
        const BinaryOperation operation, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : m_operation(operation)
        , m_left(std::move(left))
        , m_right(std::move(right))
    {
        HE_ASSERT(m_left != nullptr);
        HE_ASSERT(m_right != nullptr);
    }

    constexpr AstNodeKind kind() const override { return AstNodeKind::BinaryExpression; }

    BinaryOperation operation() const { return m_operation; }
    const Expression *left() const { return m_left.get(); }
    const Expression *right() const { return m_right.get(); }

private:
    BinaryOperation m_operation;
    std::unique_ptr<Expression> m_left;
    std::unique_ptr<Expression> m_right;
};

class CallExpression : public Expression {
public:
    CallExpression(const std::string_view identifier, std::vector<std::unique_ptr<Expression>> arguments)
        : m_identifier(identifier)
        , m_arguments(std::move(arguments))
    {
    }

    constexpr AstNodeKind kind() const override { return AstNodeKind::CallExpression; }

    std::string_view identifier() const { return m_identifier; }
    usize argument_count() const { return m_arguments.size(); }
    Expression *argument(const usize index) const { return m_arguments[index].get(); }

private:
    std::string_view m_identifier;
    std::vector<std::unique_ptr<Expression>> m_arguments;
};

class Literal : public Expression {
public:
};

class IntegerLiteral : public Literal {
public:
    explicit IntegerLiteral(const u32 value)
        : m_value(value)
    {
    }

    constexpr AstNodeKind kind() const override { return AstNodeKind::IntegerLiteral; }

    u32 value() const { return m_value; }

private:
    u32 m_value;
};

class Statement : public AstNode {
public:
};

class CompoundStatement : public Statement {
public:
    explicit CompoundStatement(std::vector<std::unique_ptr<Statement>> statements)
        : m_statements(std::move(statements))
    {
    }

    constexpr AstNodeKind kind() const override { return AstNodeKind::CompoundStatement; }

    usize statement_count() const { return m_statements.size(); }
    Statement *statement(const usize index) const { return m_statements[index].get(); }

private:
    std::vector<std::unique_ptr<Statement>> m_statements;
};

class ExpressionStatement : public Statement {
public:
    explicit ExpressionStatement(std::unique_ptr<Expression> expression)
        : m_expression(std::move(expression))
    {
    }

    constexpr AstNodeKind kind() const override { return AstNodeKind::ExpressionStatement; }

    Expression *expression() const { return m_expression.get(); }

private:
    std::unique_ptr<Expression> m_expression;
};

} // namespace he::script
