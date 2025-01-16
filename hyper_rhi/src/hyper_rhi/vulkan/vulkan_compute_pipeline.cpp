/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/vulkan_compute_pipeline.hpp"

#include <hyper_core/assertion.hpp>
#include <hyper_core/ref_ptr.hpp>

#include "hyper_rhi/vulkan/vulkan_graphics_device.hpp"
#include "hyper_rhi/vulkan/vulkan_pipeline_layout.hpp"
#include "hyper_rhi/vulkan/vulkan_shader_module.hpp"

namespace hyper_engine
{
    RefPtr<ComputePipeline> VulkanGraphicsDevice::create_compute_pipeline_platform(const ComputePipelineDescriptor &descriptor)
    {
        const VulkanPipelineLayout &layout = static_cast<const VulkanPipelineLayout &>(*descriptor.layout);
        const VulkanShaderModule &shader_module = static_cast<const VulkanShaderModule &>(*descriptor.shader);

        const VkPipelineShaderStageCreateInfo shader_stage_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stage = VK_SHADER_STAGE_COMPUTE_BIT,
            .module = shader_module.shader_module(),
            .pName = shader_module.entry_name().data(),
            .pSpecializationInfo = nullptr,
        };

        const VkComputePipelineCreateInfo compute_pipeline_create_info = {
            .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stage = shader_stage_create_info,
            .layout = layout.pipeline_layout(),
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1,
        };

        VkPipeline pipeline = VK_NULL_HANDLE;
        HE_VK_CHECK(vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &compute_pipeline_create_info, nullptr, &pipeline));

        HE_ASSERT(pipeline != VK_NULL_HANDLE);

        set_object_name(pipeline, ObjectType::Pipeline, descriptor.label);

        return make_ref<VulkanComputePipeline>(descriptor, *this, pipeline);
    }

    VulkanComputePipeline::VulkanComputePipeline(
        const ComputePipelineDescriptor &descriptor,
        VulkanGraphicsDevice &graphics_device,
        const VkPipeline pipeline)
        : ComputePipeline(descriptor)
        , m_graphics_device(graphics_device)
        , m_pipeline(pipeline)
    {
    }

    VulkanComputePipeline::~VulkanComputePipeline()
    {
        m_graphics_device.resource_queue().compute_pipelines.emplace_back(m_pipeline);
    }

    VkPipeline VulkanComputePipeline::pipeline() const
    {
        return m_pipeline;
    }
} // namespace hyper_engine