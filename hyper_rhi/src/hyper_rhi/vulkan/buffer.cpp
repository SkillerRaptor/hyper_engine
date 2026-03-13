/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/buffer.hpp"

#include "hyper_rhi/vulkan/graphics_device.hpp"
#include "hyper_rhi/vulkan/render_conversion.hpp"
#include "hyper_rhi/vulkan/utils.hpp"

namespace he {

VulkanBuffer::VulkanBuffer(VulkanGraphicsDevice &graphics_device, const BufferDescriptor &desc, const bool staging)
    : m_graphics_device(graphics_device)
{
    const auto buffer_create_info = VkBufferCreateInfo {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .size = desc.size,
        .usage = map_buffer_usage(desc.usage),
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
    };

    auto allocation_flags = VmaAllocationCreateFlags { 0 };
    if (staging) {
        allocation_flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
    }

    const auto allocation_create_info = VmaAllocationCreateInfo {
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

    // FIXME: Maybe move this out?
    if (!desc.initial_data.empty()) {
        void *ptr = nullptr;
        vmaMapMemory(m_graphics_device.allocator(), m_allocation, &ptr);
        HE_ASSERT(ptr != nullptr);
        memcpy(ptr, desc.initial_data.data(), desc.initial_data.size());
        vmaUnmapMemory(m_graphics_device.allocator(), m_allocation);
    }
}

VulkanBuffer::~VulkanBuffer()
{
    if (m_allocation == VK_NULL_HANDLE) {
        return;
    }

    vmaDestroyBuffer(m_graphics_device.allocator(), m_raw, m_allocation);
}

} // namespace he
