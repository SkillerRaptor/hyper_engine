/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

namespace he
{
    template <typename T>
    class Key
    {
    private:
        friend T;

    private:
        constexpr Key() = default;
    };
} // namespace he
