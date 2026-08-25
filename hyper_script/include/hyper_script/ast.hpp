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

    // Literals
    IntegerLiteral,

    // Statements
};

class AstNode {
public:
    virtual ~AstNode() = default;

    virtual AstNodeKind kind() const = 0;
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

    AstNodeKind kind() const override { return AstNodeKind::BinaryExpression; }

    BinaryOperation operation() const { return m_operation; }
    const Expression *left() const { return m_left.get(); }
    const Expression *right() const { return m_right.get(); }

private:
    BinaryOperation m_operation;
    std::unique_ptr<Expression> m_left;
    std::unique_ptr<Expression> m_right;
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

    AstNodeKind kind() const override { return AstNodeKind::IntegerLiteral; }

    u32 value() const { return m_value; }

private:
    u32 m_value;
};

class Statement : public AstNode {
public:
};

} // namespace he::script
