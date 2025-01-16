/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_rhi/buffer.hpp"
#include "hyper_rhi/vulkan/vulkan_common.hpp"

#include <vk_mem_alloc.h>

namespace hyper_engine
{
    class VulkanGraphicsDevice;

    class VulkanBuffer final : public Buffer
    {
    public:
        VulkanBuffer(
            const BufferDescriptor &descriptor,
            ResourceHandle handle,
            VulkanGraphicsDevice &graphics_device,
            VkBuffer buffer,
            VmaAllocation allocation);
        ~VulkanBuffer() override;

        VkBuffer buffer() const;
        VmaAllocation allocation() const;

        static VkBufferUsageFlags get_buffer_usage_flags(BitFlags<BufferUsage> buffer_usage_flags);

    private:
        VulkanGraphicsDevice &m_graphics_device;

        VkBuffer m_buffer = VK_NULL_HANDLE;
        VmaAllocation m_allocation = VK_NULL_HANDLE;
    };
} // namespace hyper_engine