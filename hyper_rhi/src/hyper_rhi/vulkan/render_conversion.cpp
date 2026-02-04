/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/render_conversion.hpp"

namespace he
{
    VkBufferUsageFlags map_buffer_usage(const BitFlags<BufferUsage> usage)
    {
        VkBufferUsageFlags usage_flags { 0 };

        if (usage.has(BufferUsage::TransferSrc))
        {
            usage_flags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        }

        if (usage.has(BufferUsage::TransferDst))
        {
            usage_flags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        }

        if (usage.has(BufferUsage::Index))
        {
            usage_flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        }

        if (usage.has(BufferUsage::Indirect))
        {
            usage_flags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        }

        if (usage.has(BufferUsage::Storage))
        {
            usage_flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        }

        return usage_flags;
    }
} // namespace he
