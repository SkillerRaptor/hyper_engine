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

#include <hyper_core/memory.hpp>

#include "rhi/buffer.hpp"

namespace he
{
    class VulkanGraphicsDevice;

    class VulkanBuffer final : public Buffer
    {
    public:
        static RefPtr<VulkanBuffer> create(VulkanGraphicsDevice &, const BufferDescriptor &);
        static RefPtr<VulkanBuffer> create_staging(VulkanGraphicsDevice &, const BufferDescriptor &);

        ~VulkanBuffer() override;

        void *map() const override;
        void unmap() const override;

        HE_ALWAYS_INLINE VkBuffer raw() const { return m_raw; }

    private:
        static RefPtr<VulkanBuffer> create_internal(VulkanGraphicsDevice &, const BufferDescriptor &, bool staging);

        explicit VulkanBuffer(VulkanGraphicsDevice &graphics_device, const BufferDescriptor &desc)
            : Buffer(desc)
            , m_graphics_device(graphics_device)
        {
        }

        bool initialize(const BufferDescriptor &, bool staging);

    private:
        VulkanGraphicsDevice &m_graphics_device;

        VkBuffer m_raw = VK_NULL_HANDLE;
        VmaAllocation m_allocation = VK_NULL_HANDLE;
    };
} // namespace he
