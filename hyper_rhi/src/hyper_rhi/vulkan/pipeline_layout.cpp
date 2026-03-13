/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/pipeline_layout.hpp"

#include <array>

#include "hyper_rhi/vulkan/graphics_device.hpp"
#include "hyper_rhi/vulkan/utils.hpp"

namespace he {

VulkanPipelineLayout::VulkanPipelineLayout(VulkanGraphicsDevice &graphics_device, const PipelineLayoutDescriptor &desc)
    : m_graphics_device(graphics_device)
{
    const auto push_constant_range = VkPushConstantRange {
        .stageFlags = VK_SHADER_STAGE_ALL,
        .offset = 0,
        .size = desc.push_constant_size,
    };

    const auto descriptor_set_layouts = std::array {
        m_graphics_device.storage_buffer_layout(),
        m_graphics_device.sampled_image_layout(),
        m_graphics_device.storage_image_layout(),
        m_graphics_device.sampled_image_layout(),
    };

    const auto pipeline_layout_create_info = VkPipelineLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = static_cast<u32>(descriptor_set_layouts.size()),
        .pSetLayouts = descriptor_set_layouts.data(),
        .pushConstantRangeCount = static_cast<u32>(desc.push_constant_size == 0 ? 0 : 1),
        .pPushConstantRanges = desc.push_constant_size == 0 ? nullptr : &push_constant_range,
    };

    HE_VK_CHECK(
        vkCreatePipelineLayout(m_graphics_device.device(), &pipeline_layout_create_info, nullptr, &m_raw),
        "Failed to create vulkan pipeline layout");
    HE_ASSERT(m_raw != VK_NULL_HANDLE);
}

VulkanPipelineLayout::~VulkanPipelineLayout() { vkDestroyPipelineLayout(m_graphics_device.device(), m_raw, nullptr); }

} // namespace he
