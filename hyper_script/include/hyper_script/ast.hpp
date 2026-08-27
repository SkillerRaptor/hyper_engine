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

enum struct AstNodeKind : u8 {
    Unknown = 0,

    FunctionDeclaration,
    TranslationUnitDeclaration,
    VariableDeclaration,

    BinaryExpression,
    CallExpression,
    LiteralExpression,
    VariableExpression,

    IntegerLiteral,

    AssignStatement,
    CompoundStatement,
    DeclarationStatement,
    ExpressionStatement,
    IfStatement,
    WhileStatement,
};

struct AstNode {
    AstNodeKind kind = AstNodeKind::Unknown;

    explicit AstNode(const AstNodeKind kind)
        : kind(kind)
    {
    }

    virtual ~AstNode() = default;
};

struct Declaration : AstNode {
    explicit Declaration(const AstNodeKind kind)
        : AstNode(kind)
    {
    }
};

struct Expression : AstNode {
    explicit Expression(const AstNodeKind kind)
        : AstNode(kind)
    {
    }
};

struct Literal : AstNode {
    explicit Literal(const AstNodeKind kind)
        : AstNode(kind)
    {
    }
};

struct Statement : AstNode {
    explicit Statement(const AstNodeKind kind)
        : AstNode(kind)
    {
    }
};

struct FunctionDeclaration : Declaration {
    std::string_view identifier;
    std::unique_ptr<Statement> body;

    explicit FunctionDeclaration(const std::string_view identifier, std::unique_ptr<Statement> body)
        : Declaration(AstNodeKind::FunctionDeclaration)
        , identifier(identifier)
        , body(std::move(body))
    {
        HE_ASSERT(this->body != nullptr);
    }
};

struct TranslationUnitDeclaration : Declaration {
    std::vector<std::unique_ptr<Declaration>> declarations;

    explicit TranslationUnitDeclaration(std::vector<std::unique_ptr<Declaration>> declarations)
        : Declaration(AstNodeKind::TranslationUnitDeclaration)
        , declarations(std::move(declarations))
    {
    }
};

struct VariableDeclaration : Declaration {
    std::string_view identifier;
    std::unique_ptr<Expression> initializer;

    explicit VariableDeclaration(const std::string_view identifier, std::unique_ptr<Expression> initializer)
        : Declaration(AstNodeKind::VariableDeclaration)
        , identifier(identifier)
        , initializer(std::move(initializer))
    {
        HE_ASSERT(!this->identifier.empty());
        HE_ASSERT(this->initializer != nullptr);
    }
};

enum struct BinaryOperation : u8 {
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

struct BinaryExpression final : Expression {
    BinaryOperation operation;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

    BinaryExpression(
        const BinaryOperation operation, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : Expression(AstNodeKind::BinaryExpression)
        , operation(operation)
        , left(std::move(left))
        , right(std::move(right))
    {
        HE_ASSERT(this->left != nullptr);
        HE_ASSERT(this->right != nullptr);
    }
};

struct CallExpression : Expression {
    std::string_view identifier;
    std::vector<std::unique_ptr<Expression>> arguments;

    CallExpression(const std::string_view identifier, std::vector<std::unique_ptr<Expression>> arguments)
        : Expression(AstNodeKind::CallExpression)
        , identifier(identifier)
        , arguments(std::move(arguments))
    {
        HE_ASSERT(!this->identifier.empty());
    }
};

struct LiteralExpression : Expression {
    std::unique_ptr<Literal> literal;

    explicit LiteralExpression(std::unique_ptr<Literal> literal)
        : Expression(AstNodeKind::LiteralExpression)
        , literal(std::move(literal))
    {
        HE_ASSERT(this->literal != nullptr);
    }
};

struct VariableExpression : Expression {
    std::string_view identifier;

    explicit VariableExpression(const std::string_view identifier)
        : Expression(AstNodeKind::VariableExpression)
        , identifier(identifier)
    {
        HE_ASSERT(!this->identifier.empty());
    }
};

struct IntegerLiteral : Literal {
    u32 value;

    explicit IntegerLiteral(const u32 value)
        : Literal(AstNodeKind::IntegerLiteral)
        , value(value)
    {
    }
};

struct AssignStatement : Statement {
    std::string_view identifier;
    std::unique_ptr<Expression> value;

    explicit AssignStatement(const std::string_view identifier, std::unique_ptr<Expression> value)
        : Statement(AstNodeKind::AssignStatement)
        , identifier(identifier)
        , value(std::move(value))
    {
        HE_ASSERT(!this->identifier.empty());
        HE_ASSERT(this->value != nullptr);
    }
};

struct CompoundStatement : Statement {
    std::vector<std::unique_ptr<Statement>> statements;

    explicit CompoundStatement(std::vector<std::unique_ptr<Statement>> statements)
        : Statement(AstNodeKind::CompoundStatement)
        , statements(std::move(statements))
    {
    }
};

struct DeclarationStatement : Statement {
    std::unique_ptr<Declaration> declaration;

    explicit DeclarationStatement(std::unique_ptr<Declaration> declaration)
        : Statement(AstNodeKind::DeclarationStatement)
        , declaration(std::move(declaration))
    {
        HE_ASSERT(this->declaration != nullptr);
    }
};

struct ExpressionStatement : Statement {
    std::unique_ptr<Expression> expression;

    explicit ExpressionStatement(std::unique_ptr<Expression> expression)
        : Statement(AstNodeKind::ExpressionStatement)
        , expression(std::move(expression))
    {
        HE_ASSERT(this->expression != nullptr);
    }
};

struct IfStatement : Statement {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> then_body;
    std::unique_ptr<Statement> else_body;

    explicit IfStatement(
        std::unique_ptr<Expression> condition,
        std::unique_ptr<Statement> then_body,
        std::unique_ptr<Statement> else_body)
        : Statement(AstNodeKind::IfStatement)
        , condition(std::move(condition))
        , then_body(std::move(then_body))
        , else_body(std::move(else_body))
    {
        HE_ASSERT(this->condition != nullptr);
        HE_ASSERT(this->then_body != nullptr);
    }
};

struct WhileStatement : Statement {
    std::unique_ptr<Expression> condition;
    std::unique_ptr<Statement> body;

    explicit WhileStatement(std::unique_ptr<Expression> condition, std::unique_ptr<Statement> body)
        : Statement(AstNodeKind::WhileStatement)
        , condition(std::move(condition))
        , body(std::move(body))
    {
        HE_ASSERT(this->condition != nullptr);
        HE_ASSERT(this->body != nullptr);
    }
};

} // namespace he::script
