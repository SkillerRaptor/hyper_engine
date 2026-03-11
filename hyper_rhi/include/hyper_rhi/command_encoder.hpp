/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

namespace he {

class BackendCommandEncoder {
public:
    virtual ~BackendCommandEncoder() = default;

    virtual void acquire() = 0;
    virtual void submit() = 0;
};

class CommandEncoder {
public:
    CommandEncoder(CommandEncoder &&) noexcept = default;
    CommandEncoder &operator=(CommandEncoder &&) noexcept = default;

    CommandEncoder(const CommandEncoder &) = delete;
    CommandEncoder &operator=(const CommandEncoder &) = delete;
};

} // namespace he
