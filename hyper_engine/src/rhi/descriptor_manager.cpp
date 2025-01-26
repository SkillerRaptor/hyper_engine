/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/descriptor_manager.hpp"

// FIXME: Add a versioning system to resource handles so they can't be reused, if wrongfully used

namespace hyper_engine
{
    ResourceHandle DescriptorManager::allocate_handle()
    {
        if (m_recycled_descriptors.empty())
        {
            return ResourceHandle(m_current_index++);
        }

        const ResourceHandle handle = m_recycled_descriptors.top();
        m_recycled_descriptors.pop();
        return handle;
    }

    void DescriptorManager::retire_handle(const ResourceHandle handle)
    {
        if (handle.is_valid())
        {
            return;
        }

        m_recycled_descriptors.push(handle);
    }
} // namespace hyper_engine