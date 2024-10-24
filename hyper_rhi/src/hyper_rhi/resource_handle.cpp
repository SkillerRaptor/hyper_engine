/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/resource_handle.hpp"

namespace hyper_rhi
{
    ResourceHandle::ResourceHandle(const uint32_t handle)
        : m_handle(handle)
    {
    }

    uint32_t ResourceHandle::handle() const
    {
        return m_handle;
    }
} // namespace hyper_rhi