/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/vulkan_sampler.hpp"

#include "hyper_rhi/vulkan/vulkan_conversion.hpp"
#include "hyper_rhi/vulkan/vulkan_macros.hpp"

namespace he {

VulkanSampler::VulkanSampler(const SamplerDescriptor &desc, const VkDevice device)
    : Sampler(desc)
    , m_device(device)
{
    const VkSamplerCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .magFilter = conversion::to_vk_filter(desc.mag_filter),
        .minFilter = conversion::to_vk_filter(desc.min_filter),
        .mipmapMode = conversion::to_vk_sampler_mipmap_mode(desc.mipmap_filter),
        .addressModeU = conversion::to_vk_sampler_address_mode(desc.address_mode_u),
        .addressModeV = conversion::to_vk_sampler_address_mode(desc.address_mode_v),
        .addressModeW = conversion::to_vk_sampler_address_mode(desc.address_mode_w),
        .mipLodBias = 0.0f,
        .anisotropyEnable = VK_FALSE,
        .maxAnisotropy = 0.0,
        .compareEnable = desc.compare_enable,
        .compareOp = conversion::to_vk_compare_operation(desc.compare_operation),
        .minLod = desc.min_lod,
        .maxLod = desc.max_lod,
        .borderColor = conversion::to_vk_border_color(desc.border_color),
        .unnormalizedCoordinates = VK_FALSE,
    };

    HE_VK_CHECK(vkCreateSampler(m_device, &create_info, nullptr, &m_raw));
}

VulkanSampler::~VulkanSampler() { vkDestroySampler(m_device, m_raw, nullptr); }

} // namespace he
