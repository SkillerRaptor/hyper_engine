/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

// clang-format off
#include <volk.h>
#include <vk_mem_alloc.h>
// clang-format on

#include "hyper_rhi/buffer.hpp"

namespace he
{
    class VulkanGraphicsDevice;

    class VulkanBuffer
    {
    public:
        VulkanBuffer(VulkanGraphicsDevice &, const BufferDescriptor &, bool staging);
        ~VulkanBuffer();

        HE_ALWAYS_INLINE VkBuffer raw() const { return m_raw; }
        HE_ALWAYS_INLINE VmaAllocation allocation() const { return m_allocation; }

    private:
        VulkanGraphicsDevice &m_graphics_device;

        VkBuffer m_raw = VK_NULL_HANDLE;
        VmaAllocation m_allocation = VK_NULL_HANDLE;
    };
} // namespace he
