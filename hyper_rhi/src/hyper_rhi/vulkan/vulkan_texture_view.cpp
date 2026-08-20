/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/vulkan_texture_view.hpp"

#include "hyper_rhi/vulkan/vulkan_conversion.hpp"
#include "hyper_rhi/vulkan/vulkan_macros.hpp"
#include "hyper_rhi/vulkan/vulkan_texture.hpp"

namespace he {

VulkanTextureView::VulkanTextureView(const TextureViewDescriptor &desc, const VkDevice device)
    : TextureView(desc)
    , m_device(device)
{
    constexpr VkComponentMapping component_mapping = {
        .r = VK_COMPONENT_SWIZZLE_IDENTITY,
        .g = VK_COMPONENT_SWIZZLE_IDENTITY,
        .b = VK_COMPONENT_SWIZZLE_IDENTITY,
        .a = VK_COMPONENT_SWIZZLE_IDENTITY,
    };

    const VulkanTexture *texture = static_cast<const VulkanTexture *>(desc.texture);
    const Format format = texture->format();

    const u32 mip_levels = desc.mip_levels.value_or(texture->mip_levels() - desc.base_mip_level);
    const u32 array_layers = [&desc, texture]() -> u32 {
        if (desc.array_layers.has_value()) {
            return desc.array_layers.value();
        }

        switch (desc.dimension) {
        case ViewDimension::D1:
        case ViewDimension::D2:
            return 1;
        case ViewDimension::D2Array:
            return texture->array_layers() - desc.base_array_layer;
        default:
            HE_UNREACHABLE();
        }
    }();

    const VkImageSubresourceRange subresource_range = {
        .aspectMask = conversion::to_vk_image_aspect(format),
        .baseMipLevel = desc.base_mip_level,
        .levelCount = mip_levels,
        .baseArrayLayer = desc.base_array_layer,
        .layerCount = array_layers,
    };

    const VkImageViewCreateInfo image_view_create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .image = texture->raw(),
        .viewType = conversion::to_vk_image_view_type(desc.dimension),
        .format = conversion::to_vk_format(format),
        .components = component_mapping,
        .subresourceRange = subresource_range,
    };

    HE_VK_CHECK(vkCreateImageView(m_device, &image_view_create_info, nullptr, &m_raw));
}

VulkanTextureView::~VulkanTextureView() { vkDestroyImageView(m_device, m_raw, nullptr); }

} // namespace he
