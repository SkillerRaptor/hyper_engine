/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "rhi/vulkan/pipeline_layout.hpp"

#include <array>

#include <vulkan/vk_enum_string_helper.h>

#include "core/assertion.hpp"
#include "core/logger.hpp"
#include "rhi/validation.hpp"
#include "rhi/vulkan/graphics_device.hpp"

namespace he
{
    RefPtr<VulkanPipelineLayout>
        VulkanPipelineLayout::create(VulkanGraphicsDevice &graphics_device, const PipelineLayoutDescriptor &desc)
    {
        RefPtr<VulkanPipelineLayout> pipeline_layout
            = wrap_ref<VulkanPipelineLayout>(new VulkanPipelineLayout(graphics_device, desc));
        if (!pipeline_layout->initialize(desc))
        {
            HE_ERROR("Failed to initialize pipeline layout");
            return nullptr;
        }

        return pipeline_layout;
    }

    VulkanPipelineLayout::~VulkanPipelineLayout()
    {
        vkDestroyPipelineLayout(m_graphics_device.device(), m_raw, nullptr);
    }

    bool VulkanPipelineLayout::initialize(const PipelineLayoutDescriptor &desc)
    {
        if (!validate_pipeline_layout_descriptor(desc))
        {
            return false;
        }

        const VkPushConstantRange push_constant_range = {
            .stageFlags = VK_SHADER_STAGE_ALL,
            .offset = 0,
            .size = desc.push_constant_size,
        };

        const std::array<VkDescriptorSetLayout, 4> descriptor_set_layouts = {
            m_graphics_device.storage_buffer_layout(),
            m_graphics_device.sampled_image_layout(),
            m_graphics_device.storage_image_layout(),
            m_graphics_device.sampled_image_layout(),
        };

        const VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .setLayoutCount = static_cast<u32>(descriptor_set_layouts.size()),
            .pSetLayouts = descriptor_set_layouts.data(),
            .pushConstantRangeCount = static_cast<u32>(desc.push_constant_size == 0 ? 0 : 1),
            .pPushConstantRanges = desc.push_constant_size == 0 ? nullptr : &push_constant_range,
        };

        const VkResult result
            = vkCreatePipelineLayout(m_graphics_device.device(), &pipeline_layout_create_info, nullptr, &m_raw);
        if (result != VK_SUCCESS)
        {
            HE_ERROR("Failed to create vulkan pipeline layout ({})", string_VkResult(result));
            return false;
        }

        HE_ASSERT(m_raw != VK_NULL_HANDLE);

        return true;
    }
} // namespace he
