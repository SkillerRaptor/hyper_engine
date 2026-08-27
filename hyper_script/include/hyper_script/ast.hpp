/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>
#include <string_view>
#include <vector>

#include <hyper_core/assertion.hpp>
#include <hyper_core/types.hpp>

namespace he::script {

enum class AstNodeKind : u8 {
    // Declarations
    FunctionDeclaration,
    TranslationUnitDeclaration,
    VariableDeclaration,

    // Expressions
    BinaryExpression,
    CallExpression,
    VariableExpression,

    // Literals
    IntegerLiteral,

    // Statements
    AssignStatement,
    CompoundStatement,
    DeclarationStatement,
    ExpressionStatement,
    IfStatement,
    WhileStatement,
};

class AstNode {
public:
    virtual ~AstNode() = default;

    virtual constexpr AstNodeKind kind() const = 0;
};

class Declaration : public AstNode { };
class Expression : public AstNode { };
class Literal : public Expression { };
class Statement : public AstNode { };

class FunctionDeclaration : public Declaration {
public:
    FunctionDeclaration(const std::string_view identifier, std::unique_ptr<Statement> body)
        : m_identifier(identifier)
        , m_body(std::move(body))
    {
        HE_ASSERT(!m_identifier.empty());
        HE_ASSERT(m_body != nullptr);
    }

    constexpr AstNodeKind kind() const override { return AstNodeKind::FunctionDeclaration; }

    std::string_view identifier() const { return m_identifier; }
    const Statement &body() const { return *m_body; }

private:
    std::string_view m_identifier;
    // TODO: Add arguments
    // TODO: Add return type
    std::unique_ptr<Statement> m_body;
};

class TranslationUnitDeclaration : public Declaration {
public:
    explicit TranslationUnitDeclaration(std::vector<std::unique_ptr<Declaration>> declarations)
        : m_declarations(std::move(declarations))
    {
    }

    constexpr AstNodeKind kind() const override { return AstNodeKind::TranslationUnitDeclaration; }

    usize declaration_count() const { return m_declarations.size(); }
    const Declaration &declaration(const usize index) const { return *m_declarations[index]; }

private:
    std::vector<std::unique_ptr<Declaration>> m_declarations;
};

class VariableDeclaration : public Declaration {
public:
    explicit VariableDeclaration(const std::string_view identifier, std::unique_ptr<Expression> initializer)
        : m_identifier(identifier)
        , m_initializer(std::move(initializer))
    {
        HE_ASSERT(!m_identifier.empty());
        HE_ASSERT(m_initializer != nullptr);
    }

    constexpr AstNodeKind kind() const override { return AstNodeKind::VariableDeclaration; }

    std::string_view identifier() const { return m_identifier; }
    const Expression &initializer() const { return *m_initializer; }

private:
    std::string_view m_identifier;
    std::unique_ptr<Expression> m_initializer;
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
    const Expression &left() const { return *m_left; }
    const Expression &right() const { return *m_right; }

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
        HE_ASSERT(!m_identifier.empty());
    }

    constexpr AstNodeKind kind() const override { return AstNodeKind::CallExpression; }

    std::string_view identifier() const { return m_identifier; }
    usize argument_count() const { return m_arguments.size(); }
    const Expression &argument(const usize index) const { return *m_arguments[index]; }

private:
    std::string_view m_identifier;
    std::vector<std::unique_ptr<Expression>> m_arguments;
};

class VariableExpression : public Expression {
public:
    explicit VariableExpression(const std::string_view identifier)
        : m_identifier(identifier)
    {
        HE_ASSERT(!m_identifier.empty());
    }

    constexpr AstNodeKind kind() const override { return AstNodeKind::VariableExpression; }

    std::string_view identifier() const { return m_identifier; }

private:
    std::string_view m_identifier;
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

class AssignStatement : public Statement {
public:
    explicit AssignStatement(const std::string_view identifier, std::unique_ptr<Expression> value)
        : m_identifier(identifier)
        , m_value(std::move(value))
    {
        HE_ASSERT(!m_identifier.empty());
        HE_ASSERT(m_value != nullptr);
    }

    constexpr AstNodeKind kind() const override { return AstNodeKind::AssignStatement; }

    std::string_view identifier() const { return m_identifier; }
    const Expression &value() const { return *m_value; }

private:
    std::string_view m_identifier;
    std::unique_ptr<Expression> m_value;
};

class CompoundStatement : public Statement {
public:
    explicit CompoundStatement(std::vector<std::unique_ptr<Statement>> statements)
        : m_statements(std::move(statements))
    {
    }

    constexpr AstNodeKind kind() const override { return AstNodeKind::CompoundStatement; }

    usize statement_count() const { return m_statements.size(); }
    const Statement &statement(const usize index) const { return *m_statements[index]; }

private:
    std::vector<std::unique_ptr<Statement>> m_statements;
};

class DeclarationStatement : public Statement {
public:
    explicit DeclarationStatement(std::unique_ptr<Declaration> declaration)
        : m_declaration(std::move(declaration))
    {
        HE_ASSERT(m_declaration != nullptr);
    }

    constexpr AstNodeKind kind() const override { return AstNodeKind::DeclarationStatement; }

    const Declaration &declaration() const { return *m_declaration; }

private:
    std::unique_ptr<Declaration> m_declaration;
};

class ExpressionStatement : public Statement {
public:
    explicit ExpressionStatement(std::unique_ptr<Expression> expression)
        : m_expression(std::move(expression))
    {
        HE_ASSERT(m_expression != nullptr);
    }

    constexpr AstNodeKind kind() const override { return AstNodeKind::ExpressionStatement; }

    const Expression &expression() const { return *m_expression; }

private:
    std::unique_ptr<Expression> m_expression;
};

class IfStatement : public Statement {
public:
    explicit IfStatement(
        std::unique_ptr<Expression> condition,
        std::unique_ptr<Statement> then_body,
        std::unique_ptr<Statement> else_body)
        : m_condition(std::move(condition))
        , m_then_body(std::move(then_body))
        , m_else_body(std::move(else_body))
    {
        HE_ASSERT(m_condition != nullptr);
        HE_ASSERT(m_then_body != nullptr);
    }

    constexpr AstNodeKind kind() const override { return AstNodeKind::IfStatement; }

    const Expression &condition() const { return *m_condition; }
    const Statement &then_body() const { return *m_then_body; }
    const Statement *else_body() const { return m_else_body.get(); }

private:
    std::unique_ptr<Expression> m_condition;
    std::unique_ptr<Statement> m_then_body;
    std::unique_ptr<Statement> m_else_body;
};

class WhileStatement : public Statement {
public:
    explicit WhileStatement(std::unique_ptr<Expression> condition, std::unique_ptr<Statement> body)
        : m_condition(std::move(condition))
        , m_body(std::move(body))
    {
        HE_ASSERT(m_condition != nullptr);
        HE_ASSERT(m_body != nullptr);
    }

    constexpr AstNodeKind kind() const override { return AstNodeKind::WhileStatement; }

    const Expression &condition() const { return *m_condition; }
    const Statement &body() const { return *m_body; }

private:
    std::unique_ptr<Expression> m_condition;
    std::unique_ptr<Statement> m_body;
};

} // namespace he::script
