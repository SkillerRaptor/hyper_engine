/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <span>

#include "hyper_script/ast.hpp"
#include "hyper_script/token.hpp"

namespace he::script {

void dump_tokens(std::span<const Token>);
void dump_ast(const AstNode &);

} // namespace he::script
