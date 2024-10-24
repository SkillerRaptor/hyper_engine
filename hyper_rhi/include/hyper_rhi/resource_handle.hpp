/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

namespace hyper_rhi
{
    class ResourceHandle
    {
    public:
        explicit ResourceHandle(uint32_t handle);

        [[nodiscard]] uint32_t handle() const;

    private:
        uint32_t m_handle;
    };
} // namespace hyper_rhi