/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <unordered_map>

#include "hyper_script/ast.hpp"
#include "hyper_script/ir.hpp"

namespace he::script {

class IrBuilder {
public:
    explicit IrBuilder(const FunctionDeclaration &function)
        : m_function(function)
    {
    }

    std::unique_ptr<IrFunction> build();

private:
    IrBlock *create_block();
    IrPhi *create_phi(IrBlock *block);

    template <typename T, typename... Args>
    IrValue *emit(Args &&...args)
    {
        std::unique_ptr<T> value = std::make_unique<T>(m_next_value_id, std::forward<Args>(args)...);
        m_next_value_id += 1;
        T *value_ptr = value.get();
        m_ir_function->values.push_back(std::move(value));
        m_current_block->instructions.push_back(value_ptr);
        return static_cast<IrValue *>(value_ptr);
    }

    IrValue *build_node(const AstNode &node);

    void write_variable(std::string_view variable, IrBlock *block, IrValue *value);
    IrValue *read_variable(std::string_view variable, IrBlock *block);
    IrValue *read_variable_recursive(std::string_view variable, IrBlock *block);

    IrValue *add_phi_operands(std::string_view variable, IrPhi *phi);
    IrValue *try_remove_trivial_phi(IrPhi *phi);

    void seal_block(IrBlock *block);

private:
    const FunctionDeclaration &m_function;

    std::unique_ptr<IrFunction> m_ir_function = nullptr;
    IrBlock *m_current_block = nullptr;
    u32 m_next_block_id = 0;
    u32 m_next_value_id = 0;

    std::unordered_map<std::string_view, std::unordered_map<IrBlock *, IrValue *>> m_current_definitions;
    std::unordered_map<IrBlock *, std::unordered_map<std::string_view, IrPhi *>> m_incomplete_phis;
};

} // namespace he::script
