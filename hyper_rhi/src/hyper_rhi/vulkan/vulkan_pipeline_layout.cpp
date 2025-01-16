/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/vulkan_pipeline_layout.hpp"

#include <hyper_core/assertion.hpp>
#include <hyper_core/logger.hpp>

#include "hyper_rhi/vulkan/vulkan_descriptor_manager.hpp"
#include "hyper_rhi/vulkan/vulkan_graphics_device.hpp"

namespace hyper_engine
{
    std::shared_ptr<PipelineLayout> VulkanGraphicsDevice::create_pipeline_layout_platform(const PipelineLayoutDescriptor &descriptor)
    {
        const auto &descriptor_set_layouts = m_descriptor_manager->descriptor_set_layouts();

        const VkPushConstantRange push_constant_range = {
            .stageFlags = VK_SHADER_STAGE_ALL,
            .offset = 0,
            .size = descriptor.push_constant_size,
        };

        const VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = static_cast<uint32_t>(descriptor_set_layouts.size()),
            .pSetLayouts = descriptor_set_layouts.data(),
            .pushConstantRangeCount = static_cast<uint32_t>(descriptor.push_constant_size == 0 ? 0 : 1),
            .pPushConstantRanges = descriptor.push_constant_size == 0 ? nullptr : &push_constant_range,
        };

        VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
        HE_VK_CHECK(vkCreatePipelineLayout(m_device, &pipeline_layout_create_info, nullptr, &pipeline_layout));

        HE_ASSERT(pipeline_layout != VK_NULL_HANDLE);

        set_object_name(pipeline_layout, ObjectType::PipelineLayout, descriptor.label);

        return std::make_shared<VulkanPipelineLayout>(descriptor, *this, pipeline_layout);
    }

    VulkanPipelineLayout::VulkanPipelineLayout(
        const PipelineLayoutDescriptor &descriptor,
        VulkanGraphicsDevice &graphics_device,
        const VkPipelineLayout pipeline_layout)
        : PipelineLayout(descriptor)
        , m_graphics_device(graphics_device)
        , m_pipeline_layout(pipeline_layout)
    {
    }

    VulkanPipelineLayout::~VulkanPipelineLayout()
    {
        m_graphics_device.resource_queue().pipeline_layouts.emplace_back(m_pipeline_layout);
    }

    VkPipelineLayout VulkanPipelineLayout::pipeline_layout() const
    {
        return m_pipeline_layout;
    }
} // namespace hyper_engine