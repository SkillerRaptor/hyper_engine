/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/vulkan_compute_pipeline.hpp"

#include "hyper_rhi/vulkan/vulkan_macros.hpp"
#include "hyper_rhi/vulkan/vulkan_pipeline_layout.hpp"
#include "hyper_rhi/vulkan/vulkan_shader.hpp"

namespace he {

VulkanComputePipeline::VulkanComputePipeline(const ComputePipelineDescriptor &desc, const VkDevice device)
    : ComputePipeline(desc)
    , m_device(device)
{
    const VulkanShader *shader = static_cast<const VulkanShader *>(desc.shader);

    const VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = VK_SHADER_STAGE_COMPUTE_BIT,
        .module = shader->raw(),
        .pName = shader->entry().data(),
        .pSpecializationInfo = nullptr,
    };

    const VulkanPipelineLayout *pipeline_layout = static_cast<const VulkanPipelineLayout *>(desc.layout);

    const VkComputePipelineCreateInfo create_info = {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = pipeline_shader_stage_create_info,
        .layout = pipeline_layout->raw(),
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
    };

    HE_VK_CHECK(vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &create_info, nullptr, &m_raw));
}

VulkanComputePipeline::~VulkanComputePipeline() { vkDestroyPipeline(m_device, m_raw, nullptr); }

} // namespace he
