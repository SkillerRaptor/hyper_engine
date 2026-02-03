/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/prerequisites.hpp"
#include "core/types.hpp"

namespace he
{
    class ResourceHandle
    {
    public:
        ResourceHandle() = default;

        explicit ResourceHandle(const u32 handle)
            : m_handle(handle)
        {
        }

        HE_ALWAYS_INLINE bool is_valid() const { return m_handle != 0xffffffff; }
        HE_ALWAYS_INLINE u32 get() const { return m_handle; }

    private:
        u32 m_handle = 0xffffffff;
    };
} // namespace he
