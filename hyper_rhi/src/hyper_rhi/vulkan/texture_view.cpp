/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/texture_view.hpp"

#include "hyper_rhi/vulkan/graphics_device.hpp"
#include "hyper_rhi/vulkan/render_conversion.hpp"
#include "hyper_rhi/vulkan/texture.hpp"
#include "hyper_rhi/vulkan/utils.hpp"

namespace he {

VulkanTextureView::VulkanTextureView(VulkanGraphicsDevice &graphics_device, const TextureViewDescriptor &desc)
    : m_graphics_device(graphics_device)
{
    const auto *texture = graphics_device.get_internal_state<VulkanTexture>(desc.texture);

    constexpr auto component_mapping = VkComponentMapping {
        .r = VK_COMPONENT_SWIZZLE_IDENTITY,
        .g = VK_COMPONENT_SWIZZLE_IDENTITY,
        .b = VK_COMPONENT_SWIZZLE_IDENTITY,
        .a = VK_COMPONENT_SWIZZLE_IDENTITY,
    };

    const auto format = desc.texture.format();

    const auto mip_levels = desc.mip_levels.value_or(desc.texture.mip_levels());
    const auto array_layers = [&desc]() -> u32 {
        if (desc.array_layers.has_value()) {
            return desc.array_layers.value();
        }

        switch (desc.dimension) {
        case ViewDimension::D1:
        case ViewDimension::D2:
        case ViewDimension::D3:
            return 1;
        case ViewDimension::D2Array:
            return desc.texture.extent().depth;
        case ViewDimension::Cube:
            return 6;
        default:
            HE_UNREACHABLE();
        }
    }();

    const auto subresource_range = VkImageSubresourceRange {
        .aspectMask = map_aspect(format),
        .baseMipLevel = desc.base_mip_level,
        .levelCount = mip_levels,
        .baseArrayLayer = desc.base_array_layer,
        .layerCount = array_layers,
    };

    const auto image_view_create_info = VkImageViewCreateInfo {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .image = texture->raw(),
        .viewType = map_view_dimension(desc.dimension),
        .format = map_format(format),
        .components = component_mapping,
        .subresourceRange = subresource_range,
    };

    HE_VK_CHECK(
        vkCreateImageView(m_graphics_device.device(), &image_view_create_info, nullptr, &m_raw),
        "Failed to create vulkan texture view");
    HE_ASSERT(m_raw != VK_NULL_HANDLE);
}

VulkanTextureView::~VulkanTextureView() { vkDestroyImageView(m_graphics_device.device(), m_raw, nullptr); }

} // namespace he
