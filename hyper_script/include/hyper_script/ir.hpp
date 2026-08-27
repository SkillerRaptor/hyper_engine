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
    Undef,
};

struct IrValue {
    IrValueKind kind;
    u32 id = std::numeric_limits<u32>::max();

    explicit IrValue(const IrValueKind kind)
        : kind(kind)
    {
    }

    virtual ~IrValue() = default;
};

struct IrConstant : IrValue {
    u32 value;

    IrConstant(const u32 value)
        : IrValue(IrValueKind::Constant)
        , value(value)
    {
    }
};

struct IrAdd : IrValue {
    IrValue *left;
    IrValue *right;

    IrAdd(IrValue *left, IrValue *right)
        : IrValue(IrValueKind::Addition)
        , left(left)
        , right(right)
    {
    }
};

struct IrSub : IrValue {
    IrValue *left;
    IrValue *right;

    IrSub(IrValue *left, IrValue *right)
        : IrValue(IrValueKind::Subtraction)
        , left(left)
        , right(right)
    {
    }
};

struct IrMul : IrValue {
    IrValue *left;
    IrValue *right;

    IrMul(IrValue *left, IrValue *right)
        : IrValue(IrValueKind::Multiplication)
        , left(left)
        , right(right)
    {
    }
};

struct IrDiv : IrValue {
    IrValue *left;
    IrValue *right;

    IrDiv(IrValue *left, IrValue *right)
        : IrValue(IrValueKind::Division)
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

    IrCmp(const CompareOperation operation, IrValue *left, IrValue *right)
        : IrValue(IrValueKind::Compare)
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

    IrBranch(IrBlock *target)
        : IrValue(IrValueKind::Branch)
        , condition(nullptr)
        , true_target(target)
        , false_target(nullptr)
    {
    }

    IrBranch(IrValue *condition, IrBlock *true_target, IrBlock *false_target)
        : IrValue(IrValueKind::Branch)
        , condition(condition)
        , true_target(true_target)
        , false_target(false_target)
    {
    }
};

struct IrCall : IrValue {
    std::string_view callee;
    std::vector<IrValue *> arguments;

    IrCall(const std::string_view callee, std::vector<IrValue *> arguments)
        : IrValue(IrValueKind::Call)
        , callee(callee)
        , arguments(std::move(arguments))
    {
    }
};

struct IrPhi : IrValue {
    IrBlock *parent;
    std::vector<IrValue *> operands;
    std::vector<IrValue *> users;

    IrPhi(IrBlock *parent)
        : IrValue(IrValueKind::Phi)
        , parent(parent)
    {
    }
};

struct IrUndef : IrValue {
    explicit IrUndef()
        : IrValue(IrValueKind::Undef)
    {
    }
};

struct IrBlock {
    u32 id = std::numeric_limits<u32>::max();
    bool sealed = false;
    std::vector<IrBlock *> predecessors;
    std::vector<IrValue *> instructions;
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
