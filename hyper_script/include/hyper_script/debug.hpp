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
#include "hyper_script/diagnostics.hpp"
#include "hyper_script/ir.hpp"
#include "hyper_script/token.hpp"

namespace he::script {

std::string span_to_string(const Span &);

std::string_view token_kind_to_string(TokenKind);
std::string token_to_string(const Token &);

void dump_tokens(std::span<const Token>, bool use_pipes = true);
void dump_ast(const AstNode &, bool use_pipes = true);
void dump_ir(const IrFunction &);

void dump_tokens(std::ostream &, std::span<const Token>, bool use_pipes = true);
void dump_ast(std::ostream &, const AstNode &, bool use_pipes = true);
void dump_ir(std::ostream &, const IrFunction &);

} // namespace he::script
