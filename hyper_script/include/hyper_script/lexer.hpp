/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <optional>
#include <string_view>
#include <vector>

#include "hyper_script/token.hpp"

namespace he {

class Lexer {
public:
    explicit Lexer(std::string_view source);

    std::vector<Token> lex();

private:
    std::optional<Token> next_token();

    Token lex_number(u32 start_line, u32 start_column, size_t start_index);

    char advance();
    char peek() const;
    void skip_whitespaces();

    bool has_reached_end() const;

private:
    std::string_view m_source;
    size_t m_current_index = 0;

    u32 m_line = 1;
    u32 m_column = 1;
};

} // namespace he
