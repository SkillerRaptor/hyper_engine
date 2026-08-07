/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <hyper_core/types.hpp>

namespace he {

class ResourceHandle {
public:
    ResourceHandle() = default;

    explicit ResourceHandle(const u32 handle)
        : m_handle(handle)
    {
    }

    bool is_valid() const { return m_handle != 0xffffffff; }
    u32 get() const { return m_handle; }

private:
    u32 m_handle = 0xffffffff;
};

} // namespace he
