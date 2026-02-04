/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/compute_pipeline.hpp"

#include <vulkan/vk_enum_string_helper.h>

#include <hyper_core/assertion.hpp>
#include <hyper_core/logger.hpp>

#include "hyper_rhi/validation.hpp"
#include "hyper_rhi/vulkan/graphics_device.hpp"
#include "hyper_rhi/vulkan/pipeline_layout.hpp"
#include "hyper_rhi/vulkan/shader.hpp"

namespace he
{
    RefPtr<VulkanComputePipeline>
        VulkanComputePipeline::create(VulkanGraphicsDevice &graphics_device, const ComputePipelineDescriptor &desc)
    {
        RefPtr<VulkanComputePipeline> compute_pipeline
            = wrap_ref<VulkanComputePipeline>(new VulkanComputePipeline(graphics_device, desc));
        if (!compute_pipeline->initialize(desc))
        {
            HE_ERROR("Failed to initialize compute pipeline");
            return nullptr;
        }

        return compute_pipeline;
    }

    VulkanComputePipeline::~VulkanComputePipeline() { }

    bool VulkanComputePipeline::initialize(const ComputePipelineDescriptor &desc)
    {
        if (!validate_compute_pipeline_descriptor(desc))
        {
            return false;
        }

        const VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stage = VK_SHADER_STAGE_COMPUTE_BIT,
            .module = cast_ref<VulkanShader>(desc.shader)->raw(),
            .pName = desc.shader->entry().data(),
            .pSpecializationInfo = nullptr,
        };

        const VkComputePipelineCreateInfo compute_pipeline_create_info {
            .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stage = pipeline_shader_stage_create_info,
            .layout = cast_ref<VulkanPipelineLayout>(desc.layout)->raw(),
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1,
        };

        const VkResult result = vkCreateComputePipelines(
            m_graphics_device.device(), VK_NULL_HANDLE, 1, &compute_pipeline_create_info, nullptr, &m_raw);
        if (result != VK_SUCCESS)
        {
            HE_ERROR("Failed to create vulkan compute pipeline ({})", string_VkResult(result));
            return result;
        }

        HE_ASSERT(m_raw != VK_NULL_HANDLE);

        return true;
    }
} // namespace he
