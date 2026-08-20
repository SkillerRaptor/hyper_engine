/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/vulkan_pipeline_layout.hpp"

#include "hyper_core/types.hpp"
#include "hyper_rhi/vulkan/vulkan_macros.hpp"

namespace he {

VulkanPipelineLayout::VulkanPipelineLayout(
    const PipelineLayoutDescriptor &desc,
    const VkDevice device,
    const std::span<const VkDescriptorSetLayout> descriptor_set_layouts)
    : PipelineLayout(desc)
    , m_device(device)
{
    const VkPushConstantRange push_constant_range = {
        .stageFlags = VK_SHADER_STAGE_ALL,
        .offset = 0,
        .size = desc.push_constant_size,
    };

    const VkPipelineLayoutCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = static_cast<u32>(descriptor_set_layouts.size()),
        .pSetLayouts = descriptor_set_layouts.data(),
        .pushConstantRangeCount = static_cast<u32>(desc.push_constant_size == 0 ? 0 : 1),
        .pPushConstantRanges = desc.push_constant_size == 0 ? nullptr : &push_constant_range,
    };

    HE_VK_CHECK(vkCreatePipelineLayout(m_device, &create_info, nullptr, &m_raw));
}

VulkanPipelineLayout::~VulkanPipelineLayout() { vkDestroyPipelineLayout(m_device, m_raw, nullptr); }

} // namespace he
