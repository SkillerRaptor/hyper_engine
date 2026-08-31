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
    explicit Lexer(const SourceManager &source_manager, DiagnosticEngine &diagnostics, const SourceId source_id)
        : m_diagnostics(diagnostics)
        , m_source_id(source_id)
        , m_source(source_manager.get_source(source_id))
    {
    }

    std::vector<Token> lex();

private:
    Token next_token();

    Token lex_number(size_t start_index);
    Token lex_identifier(size_t start_index);

    char advance();
    char peek() const;
    void skip_whitespaces();

    bool has_reached_end() const;

    Span make_span(usize start) const;

private:
    DiagnosticEngine &m_diagnostics;
    SourceId m_source_id;
    std::string_view m_source;
    size_t m_current_index = 0;
};

} // namespace he::script
