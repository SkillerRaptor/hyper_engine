/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>
#include <string_view>
#include <vector>

#include <hyper_core/types.hpp>

namespace he::script {

struct IrBlock;

enum class IrValueKind : u8 {
    Unknown = 0,

    Constant,

    Addition,
    Subtraction,
    Multiplication,
    Division,
    Compare,

    Branch,
    Call,
    Phi,
};

struct IrValue {
    IrValueKind kind;
    u32 id;

    explicit IrValue(const IrValueKind kind, const u32 id)
        : kind(kind)
        , id(id)
    {
    }

    virtual ~IrValue() = default;
};

struct IrConstant : IrValue {
    u32 value;

    IrConstant(const u32 id, const u32 value)
        : IrValue(IrValueKind::Constant, id)
        , value(value)
    {
    }
};

struct IrAdd : IrValue {
    IrValue *left;
    IrValue *right;

    IrAdd(const u32 id, IrValue *left, IrValue *right)
        : IrValue(IrValueKind::Addition, id)
        , left(left)
        , right(right)
    {
    }
};

struct IrSub : IrValue {
    IrValue *left;
    IrValue *right;

    IrSub(const u32 id, IrValue *left, IrValue *right)
        : IrValue(IrValueKind::Subtraction, id)
        , left(left)
        , right(right)
    {
    }
};

struct IrMul : IrValue {
    IrValue *left;
    IrValue *right;

    IrMul(const u32 id, IrValue *left, IrValue *right)
        : IrValue(IrValueKind::Multiplication, id)
        , left(left)
        , right(right)
    {
    }
};

struct IrDiv : IrValue {
    IrValue *left;
    IrValue *right;

    IrDiv(const u32 id, IrValue *left, IrValue *right)
        : IrValue(IrValueKind::Division, id)
        , left(left)
        , right(right)
    {
    }
};

enum class CompareOperation : u8 {
    Equal,
    NotEqual,
    LessThan,
    LessThanOrEqual,
    GreaterThan,
    GreaterThanOrEqual,
};

struct IrCmp : IrValue {
    CompareOperation operation;
    IrValue *left;
    IrValue *right;

    IrCmp(const u32 id, const CompareOperation operation, IrValue *left, IrValue *right)
        : IrValue(IrValueKind::Compare, id)
        , operation(operation)
        , left(left)
        , right(right)
    {
    }
};

struct IrBranch : IrValue {
    IrValue *condition;
    IrBlock *true_target;
    IrBlock *false_target;

    IrBranch(const u32 id, IrBlock *target)
        : IrValue(IrValueKind::Branch, id)
        , condition(nullptr)
        , true_target(target)
        , false_target(nullptr)
    {
    }

    IrBranch(const u32 id, IrValue *condition, IrBlock *true_target, IrBlock *false_target)
        : IrValue(IrValueKind::Branch, id)
        , condition(condition)
        , true_target(true_target)
        , false_target(false_target)
    {
    }
};

struct IrCall : IrValue {
    std::string_view callee;
    std::vector<IrValue *> arguments;

    IrCall(const u32 id, const std::string_view callee, std::vector<IrValue *> arguments)
        : IrValue(IrValueKind::Call, id)
        , callee(callee)
        , arguments(std::move(arguments))
    {
    }
};

struct IrPhi : IrValue {
    IrBlock *parent;
    std::vector<IrValue *> operands;

    IrPhi(const u32 id, IrBlock *parent)
        : IrValue(IrValueKind::Phi, id)
        , parent(parent)
    {
    }
};

struct IrBlock {
    u32 id;
    bool sealed = false;
    std::vector<IrBlock *> predecessors;
    std::vector<IrValue *> instructions;

    explicit IrBlock(const u32 id)
        : id(id)
    {
    }
};

struct IrFunction {
    std::string_view name;
    std::vector<std::unique_ptr<IrBlock>> blocks;
    std::vector<std::unique_ptr<IrValue>> values;

    explicit IrFunction(const std::string_view name)
        : name(name)
    {
    }
};

} // namespace he::script
