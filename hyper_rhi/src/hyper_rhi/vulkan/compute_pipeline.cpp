/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/compute_pipeline.hpp"

#include "hyper_rhi/validation.hpp"
#include "hyper_rhi/vulkan/graphics_device.hpp"
#include "hyper_rhi/vulkan/pipeline_layout.hpp"
#include "hyper_rhi/vulkan/shader.hpp"
#include "hyper_rhi/vulkan/utils.hpp"

namespace he
{
    VulkanComputePipeline::VulkanComputePipeline(
        VulkanGraphicsDevice &graphics_device, const ComputePipelineDescriptor &desc)
        : ComputePipeline(desc)
        , m_graphics_device(graphics_device)
    {
        validate_compute_pipeline_descriptor(desc);

        const VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stage = VK_SHADER_STAGE_COMPUTE_BIT,
            .module = cast_ref<VulkanShader>(desc.shader)->raw(),
            .pName = desc.shader->entry().data(),
            .pSpecializationInfo = nullptr,
        };

        const VkComputePipelineCreateInfo compute_pipeline_create_info = {
            .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stage = pipeline_shader_stage_create_info,
            .layout = cast_ref<VulkanPipelineLayout>(desc.layout)->raw(),
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1,
        };

        HE_VK_CHECK(
            vkCreateComputePipelines(
                m_graphics_device.device(), VK_NULL_HANDLE, 1, &compute_pipeline_create_info, nullptr, &m_raw),
            "Failed to create vulkan compute pipeline");
        HE_ASSERT(m_raw != VK_NULL_HANDLE);
    }

    VulkanComputePipeline::~VulkanComputePipeline() { vkDestroyPipeline(m_graphics_device.device(), m_raw, nullptr); }
} // namespace he
