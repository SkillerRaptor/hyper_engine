/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <span>
#include <string>
#include <string_view>

#include "hyper_script/ast.hpp"
#include "hyper_script/token.hpp"

namespace he::script {

std::string_view token_kind_to_string(TokenKind);
std::string token_to_string(const Token &);

void dump_tokens(std::span<const Token>);
void dump_ast(const AstNode &);

} // namespace he::script
