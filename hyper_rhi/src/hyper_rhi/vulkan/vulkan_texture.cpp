/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/vulkan_texture.hpp"

#include "hyper_rhi/vulkan/vulkan_conversion.hpp"
#include "hyper_rhi/vulkan/vulkan_macros.hpp"

namespace he {

VulkanTexture::VulkanTexture(const TextureDescriptor &desc, const VmaAllocator allocator)
    : Texture(desc)
    , m_allocator(allocator)
{
    const VkExtent3D extent = {
        .width = desc.extent.width,
        .height = desc.extent.height,
        .depth = 1,
    };

    const VkImageCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .imageType = conversion::to_vk_image_type(desc.dimension),
        .format = conversion::to_vk_format(desc.format),
        .extent = extent,
        .mipLevels = desc.mip_levels,
        .arrayLayers = desc.array_layers,
        .samples = conversion::to_vk_sample_count(desc.sample_count),
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = conversion::to_vk_image_usage(desc.usage, desc.format),
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
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

    HE_VK_CHECK(vmaCreateImage(m_allocator, &create_info, &allocation_create_info, &m_raw, &m_allocation, nullptr));
}

VulkanTexture::~VulkanTexture() { vmaDestroyImage(m_allocator, m_raw, m_allocation); }

} // namespace he
