/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/texture.hpp"

#include "hyper_rhi/vulkan/graphics_device.hpp"
#include "hyper_rhi/vulkan/render_conversion.hpp"
#include "hyper_rhi/vulkan/utils.hpp"

namespace he {

VulkanTexture::VulkanTexture(
    VulkanGraphicsDevice &graphics_device, const TextureDescriptor &desc, const VkImage image)
    : m_graphics_device(graphics_device)
    , m_raw(image)
{
    if (image != VK_NULL_HANDLE) {
        return;
    }

    VkImageCreateFlags flags = 0;
    if (desc.dimension == Dimension::D2 && desc.extent.depth % 6 == 0 && desc.extent.width == desc.extent.height) {
        flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    }

    const VkExtent3D extent = {
        .width = desc.extent.width,
        .height = desc.extent.height,
        .depth = desc.dimension != Dimension::D3 ? 1 : desc.extent.depth,
    };

    const VkImageCreateInfo image_create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = flags,
        .imageType = map_dimension(desc.dimension),
        .format = map_format(desc.format),
        .extent = extent,
        .mipLevels = desc.mip_levels,
        .arrayLayers = desc.extent.depth,
        .samples = map_sample_count(desc.sample_count),
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = map_texture_usage(desc.usage, desc.format),
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

    HE_VK_CHECK(
        vmaCreateImage(
            m_graphics_device.allocator(),
            &image_create_info,
            &allocation_create_info,
            &m_raw,
            &m_allocation,
            nullptr),
        "Failed to create vulkan texture");
    HE_ASSERT(m_raw != VK_NULL_HANDLE);
    HE_ASSERT(m_allocation != VK_NULL_HANDLE);
}

VulkanTexture::~VulkanTexture()
{
    if (m_allocation == VK_NULL_HANDLE) {
        return;
    }

    vmaDestroyImage(m_graphics_device.allocator(), m_raw, m_allocation);
}

} // namespace he
