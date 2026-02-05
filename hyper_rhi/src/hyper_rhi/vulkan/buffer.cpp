/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/buffer.hpp"

#include <hyper_core/assertion.hpp>

#include "hyper_rhi/validation.hpp"
#include "hyper_rhi/vulkan/graphics_device.hpp"
#include "hyper_rhi/vulkan/render_conversion.hpp"
#include "hyper_rhi/vulkan/utils.hpp"

namespace he
{
    VulkanBuffer::VulkanBuffer(VulkanGraphicsDevice &graphics_device, const BufferDescriptor &desc, const bool staging)
        : Buffer(desc)
        , m_graphics_device(graphics_device)
    {
        validate_buffer_descriptor(desc);

        const VkBufferCreateInfo buffer_create_info = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .size = desc.size,
            .usage = map_buffer_usage(desc.usage),
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
        };

        VmaAllocationCreateFlags allocation_flags = 0;
        if (staging)
        {
            allocation_flags
                |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
        }

        const VmaAllocationCreateInfo allocation_create_info = {
            .flags = allocation_flags,
            .usage = VMA_MEMORY_USAGE_AUTO,
            .requiredFlags = 0,
            .preferredFlags = 0,
            .memoryTypeBits = 0,
            .pool = VK_NULL_HANDLE,
            .pUserData = nullptr,
            .priority = 0,
        };

        HE_VK_CHECK(
            vmaCreateBuffer(
                m_graphics_device.allocator(),
                &buffer_create_info,
                &allocation_create_info,
                &m_raw,
                &m_allocation,
                nullptr),
            "Failed to create vulkan buffer");
        HE_ASSERT(m_raw != VK_NULL_HANDLE);
        HE_ASSERT(m_allocation != VK_NULL_HANDLE);

        if (!desc.initial_data.empty())
        {
            u8 *ptr = static_cast<u8 *>(map());
            memcpy(ptr, desc.initial_data.data(), desc.initial_data.size());
            unmap();
        }
    }

    VulkanBuffer::~VulkanBuffer() { vmaDestroyBuffer(m_graphics_device.allocator(), m_raw, m_allocation); }

    void *VulkanBuffer::map() const
    {
        void *ptr = nullptr;
        vmaMapMemory(m_graphics_device.allocator(), m_allocation, &ptr);
        HE_ASSERT(ptr != nullptr);
        return ptr;
    }

    void VulkanBuffer::unmap() const { vmaUnmapMemory(m_graphics_device.allocator(), m_allocation); }
} // namespace he
