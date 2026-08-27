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
        std::unique_ptr<T> value = std::make_unique<T>(std::forward<Args>(args)...);
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

    IrValue *get_undef();

    void add_use(IrValue *operand, IrValue *user);
    void replace_value(IrValue *user, const IrValue *old_value, IrValue *new_value);

private:
    const FunctionDeclaration &m_function;

    std::unique_ptr<IrFunction> m_ir_function = nullptr;
    IrBlock *m_current_block = nullptr;
    IrValue *m_undef_value = nullptr;

    std::unordered_map<std::string_view, std::unordered_map<IrBlock *, IrValue *>> m_current_definitions;
    std::unordered_map<IrBlock *, std::unordered_map<std::string_view, IrPhi *>> m_incomplete_phis;
};

} // namespace he::script
