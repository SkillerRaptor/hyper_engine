/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/vulkan_buffer.hpp"

#include <hyper_core/assertion.hpp>

#include "hyper_rhi/vulkan/vulkan_descriptor_manager.hpp"
#include "hyper_rhi/vulkan/vulkan_graphics_device.hpp"

namespace hyper_engine
{
    std::shared_ptr<Buffer> VulkanGraphicsDevice::create_buffer_platform(const BufferDescriptor &descriptor, const ResourceHandle handle)
    {
        return create_buffer_internal(descriptor, handle, false);
    }

    std::shared_ptr<Buffer>
        VulkanGraphicsDevice::create_buffer_internal(const BufferDescriptor &descriptor, const ResourceHandle handle, const bool staging)
    {
        const VkBufferUsageFlags usage_flags = VulkanBuffer::get_buffer_usage_flags(descriptor.usage);

        // NOTE: To be able to use vkCmdUpdateBuffer the buffer size needs to be a multiple of 4 if the size is less than 64kb
        size_t byte_size = descriptor.byte_size;
        if (descriptor.byte_size < 65536)
        {
            byte_size = (descriptor.byte_size + 3) & ~3ull;
        }

        const VkBufferCreateInfo buffer_create_info = {
            .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .size = byte_size,
            .usage = usage_flags,
            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
            .queueFamilyIndexCount = 0,
            .pQueueFamilyIndices = nullptr,
        };

        // FIXME: Add more explicit allocation
        VmaAllocationCreateFlags allocation_flags = 0;
        if (staging)
        {
            allocation_flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
            allocation_flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
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

        VkBuffer buffer = VK_NULL_HANDLE;
        VmaAllocation allocation = VK_NULL_HANDLE;
        HE_VK_CHECK(vmaCreateBuffer(m_allocator, &buffer_create_info, &allocation_create_info, &buffer, &allocation, nullptr));

        HE_ASSERT(buffer != VK_NULL_HANDLE);
        HE_ASSERT(allocation != VK_NULL_HANDLE);

        set_object_name(buffer, ObjectType::Buffer, descriptor.label);

        return std::make_shared<VulkanBuffer>(descriptor, handle, *this, buffer, allocation);
    }

    VulkanBuffer::VulkanBuffer(
        const BufferDescriptor &descriptor,
        const ResourceHandle handle,
        VulkanGraphicsDevice &graphics_device,
        const VkBuffer buffer,
        const VmaAllocation allocation)
        : Buffer(descriptor, handle)
        , m_graphics_device(graphics_device)
        , m_buffer(buffer)
        , m_allocation(allocation)
    {
    }

    VulkanBuffer::~VulkanBuffer()
    {
        m_graphics_device.resource_queue().buffers.emplace_back(m_buffer, m_allocation, m_handle);
    }

    VkBuffer VulkanBuffer::buffer() const
    {
        return m_buffer;
    }

    VmaAllocation VulkanBuffer::allocation() const
    {
        return m_allocation;
    }

    VkBufferUsageFlags VulkanBuffer::get_buffer_usage_flags(const BitFlags<BufferUsage> buffer_usage_flags)
    {
        VkBufferUsageFlags usage_flags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

        if (buffer_usage_flags & BufferUsage::Index)
        {
            usage_flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        }

        if (buffer_usage_flags & BufferUsage::Indirect)
        {
            usage_flags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        }

        if (buffer_usage_flags & BufferUsage::Storage)
        {
            usage_flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
        }

        return usage_flags;
    }
} // namespace hyper_engine