/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/sampler.hpp"

#include "hyper_rhi/vulkan/graphics_device.hpp"
#include "hyper_rhi/vulkan/render_conversion.hpp"
#include "hyper_rhi/vulkan/utils.hpp"

namespace he {

VulkanSampler::VulkanSampler(VulkanGraphicsDevice &graphics_device, const SamplerDescriptor &desc)
    : m_graphics_device(graphics_device)
{
    const VkSamplerCreateInfo sampler_create_info = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .magFilter = map_filter(desc.mag_filter),
        .minFilter = map_filter(desc.min_filter),
        .mipmapMode = map_mipmap_mode(desc.mipmap_filter),
        .addressModeU = map_address_mode(desc.address_mode_u),
        .addressModeV = map_address_mode(desc.address_mode_v),
        .addressModeW = map_address_mode(desc.address_mode_w),
        .mipLodBias = 0.0f,
        .anisotropyEnable = VK_FALSE,
        .maxAnisotropy = 0.0,
        .compareEnable = VK_TRUE,
        .compareOp = map_compare_operation(desc.compare_operation),
        .minLod = desc.min_lod,
        .maxLod = desc.max_lod,
        .borderColor = map_border_color(desc.border_color),
        .unnormalizedCoordinates = VK_FALSE,
    };

    HE_VK_CHECK(
        vkCreateSampler(m_graphics_device.device(), &sampler_create_info, nullptr, &m_raw),
        "Failed to create vulkan sampler");
    HE_ASSERT(m_raw != VK_NULL_HANDLE);
}

VulkanSampler::~VulkanSampler() { vkDestroySampler(m_graphics_device.device(), m_raw, nullptr); }

} // namespace he
