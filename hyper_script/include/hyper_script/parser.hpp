/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <optional>
#include <span>

#include "hyper_script/ast.hpp"
#include "hyper_script/token.hpp"

namespace he::script {

class Parser {
public:
    explicit Parser(const std::span<const Token> tokens)
        : m_tokens(tokens)
    {
    }

    std::unique_ptr<AstNode> parse();

private:
    std::unique_ptr<Declaration> parse_declaration();
    std::unique_ptr<Declaration> parse_function_declaration();
    std::unique_ptr<Declaration> parse_translation_unit_declaration();
    std::unique_ptr<Declaration> parse_variable_declaration();

    std::unique_ptr<Expression> parse_primary_expression();
    std::unique_ptr<Expression> parse_binary_expression(u8 precedence);
    std::unique_ptr<Expression> parse_call_expression();
    std::unique_ptr<Expression> parse_variable_expression();

    std::unique_ptr<Literal> parse_integer_literal();

    std::unique_ptr<Statement> parse_statement();
    std::unique_ptr<Statement> parse_assign_statement();
    std::unique_ptr<Statement> parse_compound_statement();
    std::unique_ptr<Statement> parse_declaration_statement(std::unique_ptr<Declaration>);
    std::unique_ptr<Statement> parse_expression_statement(std::unique_ptr<Expression>);
    std::unique_ptr<Statement> parse_if_statement();
    std::unique_ptr<Statement> parse_while_statement();

    std::string_view parse_identifier();

    static u8 get_operator_precedence(TokenKind);

    std::optional<Token> current_token() const;
    std::optional<Token> peek_token() const;

    bool match(TokenKind) const;

    Token consume();
    Token consume(TokenKind);

    bool has_reached_end() const;

private:
    std::span<const Token> m_tokens;
    size_t m_current_token_index = 0;
};

} // namespace he::script
