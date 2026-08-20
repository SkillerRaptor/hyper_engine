/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/vulkan_buffer.hpp"

#include "hyper_rhi/vulkan/vulkan_conversion.hpp"
#include "hyper_rhi/vulkan/vulkan_macros.hpp"

namespace he {

VulkanBuffer::VulkanBuffer(const BufferDescriptor &desc, const VmaAllocator allocator)
    : Buffer(desc)
    , m_allocator(allocator)
{
    const VkBufferCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .size = desc.size,
        .usage = conversion::to_vk_buffer_usage(desc.usage),
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
    };

    constexpr VmaAllocationCreateInfo allocation_create_info = {
        .flags = 0,
        .usage = VMA_MEMORY_USAGE_AUTO,
        .requiredFlags = 0,
        .preferredFlags = 0,
        .memoryTypeBits = 0,
        .pool = VK_NULL_HANDLE,
        .pUserData = nullptr,
        .priority = 0,
    };

    HE_VK_CHECK(vmaCreateBuffer(m_allocator, &create_info, &allocation_create_info, &m_raw, &m_allocation, nullptr));
}

VulkanBuffer::~VulkanBuffer() { vmaDestroyBuffer(m_allocator, m_raw, m_allocation); }

} // namespace he
