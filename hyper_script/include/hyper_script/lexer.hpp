/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <optional>
#include <string_view>
#include <vector>

#include "hyper_script/diagnostics.hpp"
#include "hyper_script/source_manager.hpp"
#include "hyper_script/token.hpp"

namespace he::script {

class Lexer {
public:
    explicit Lexer(const SourceManager &source_manager, DiagnosticEngine &diagnostic_engine, const SourceId source_id)
        : m_diagnostic_engine(diagnostic_engine)
        , m_source_id(source_id)
        , m_source(source_manager.get_source(source_id))
    {
    }

    std::vector<Token> lex();

private:
    std::optional<Token> next_token();

    Token lex_number(u32 start_line, u32 start_column, size_t start_index);
    Token lex_identifier(u32 start_line, u32 start_column, size_t start_index);

    char advance();
    char peek() const;
    void skip_whitespaces();

    bool has_reached_end() const;

private:
    DiagnosticEngine &m_diagnostic_engine;
    SourceId m_source_id;
    std::string_view m_source;
    size_t m_current_index = 0;

    u32 m_line = 1;
    u32 m_column = 1;
};

} // namespace he::script
