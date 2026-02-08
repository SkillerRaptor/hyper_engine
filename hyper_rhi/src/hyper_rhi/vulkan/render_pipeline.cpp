/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/render_pipeline.hpp"

#include <array>

#include "hyper_rhi/validation.hpp"
#include "hyper_rhi/vulkan/graphics_device.hpp"
#include "hyper_rhi/vulkan/pipeline_layout.hpp"
#include "hyper_rhi/vulkan/render_conversion.hpp"
#include "hyper_rhi/vulkan/shader.hpp"
#include "hyper_rhi/vulkan/utils.hpp"

namespace he
{
    VulkanRenderPipeline::VulkanRenderPipeline(
        VulkanGraphicsDevice &graphics_device, const RenderPipelineDescriptor &desc)
        : RenderPipeline(desc)
        , m_graphics_device(graphics_device)
    {
        validate_render_pipeline_descriptor(desc);

        const RefPtr<VulkanShader> vertex_shader = cast_ref<VulkanShader>(desc.vertex_shader);
        const VkPipelineShaderStageCreateInfo vertex_pipeline_shader_stage_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = vertex_shader->raw(),
            .pName = vertex_shader->entry().data(),
            .pSpecializationInfo = nullptr,
        };

        const RefPtr<VulkanShader> fragment_shader = cast_ref<VulkanShader>(desc.fragment_shader);
        const VkPipelineShaderStageCreateInfo fragment_pipeline_shader_stage_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = fragment_shader->raw(),
            .pName = fragment_shader->entry().data(),
            .pSpecializationInfo = nullptr,
        };

        const std::array<VkPipelineShaderStageCreateInfo, 2> shader_stage_create_infos = {
            vertex_pipeline_shader_stage_create_info,
            fragment_pipeline_shader_stage_create_info,
        };

        constexpr VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .vertexBindingDescriptionCount = 0,
            .pVertexBindingDescriptions = nullptr,
            .vertexAttributeDescriptionCount = 0,
            .pVertexAttributeDescriptions = nullptr,
        };

        const VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .topology = map_primitive_topology(desc.primitive_state.topology),
            .primitiveRestartEnable = false,
        };

        constexpr VkPipelineTessellationStateCreateInfo tessellation_state_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .patchControlPoints = 0,
        };

        constexpr VkPipelineViewportStateCreateInfo viewport_state_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .viewportCount = 1,
            .pViewports = nullptr,
            .scissorCount = 1,
            .pScissors = nullptr,
        };

        VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .depthClampEnable = false,
            .rasterizerDiscardEnable = false,
            .polygonMode = map_polygon_mode(desc.primitive_state.polygon_mode),
            .cullMode = map_cull_mode(desc.primitive_state.cull_mode),
            .frontFace = map_front_face(desc.primitive_state.front_face),
            .depthBiasEnable = false,
            .depthBiasConstantFactor = 0.0f,
            .depthBiasClamp = 0.0f,
            .depthBiasSlopeFactor = 0.0f,
            .lineWidth = 1.0,
        };

        if (desc.depth_stencil_state.has_value())
        {
            const DepthStencilState &depth_stencil_state = desc.depth_stencil_state.value();
            rasterization_state_create_info.depthBiasEnable = depth_stencil_state.depth_bias_state.enable;
            rasterization_state_create_info.depthBiasConstantFactor = depth_stencil_state.depth_bias_state.constant;
            rasterization_state_create_info.depthBiasClamp = depth_stencil_state.depth_bias_state.clamp;
            rasterization_state_create_info.depthBiasSlopeFactor = depth_stencil_state.depth_bias_state.slope;
        }

        const VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .rasterizationSamples = map_sample_count(desc.multisample_state.sample_count),
            .sampleShadingEnable = true,
            .minSampleShading = 0.2f,
            .pSampleMask = nullptr,
            .alphaToCoverageEnable = false,
            .alphaToOneEnable = false,
        };

        // FIXME: Add depth bounds & stencil
        VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .depthTestEnable = false,
            .depthWriteEnable = false,
            .depthCompareOp = VK_COMPARE_OP_NEVER,
            .depthBoundsTestEnable = false,
            .stencilTestEnable = false,
            .front = {},
            .back = {},
            .minDepthBounds = 0.0,
            .maxDepthBounds = 1.0,
        };

        if (desc.depth_stencil_state.has_value())
        {
            const DepthStencilState &depth_stencil_state = desc.depth_stencil_state.value();
            depth_stencil_state_create_info.depthTestEnable = depth_stencil_state.depth_test_enable;
            depth_stencil_state_create_info.depthWriteEnable = depth_stencil_state.depth_write_enable;

            const VkCompareOp depth_compare_operation
                = map_compare_operation(depth_stencil_state.depth_compare_operation);
            depth_stencil_state_create_info.depthCompareOp = depth_compare_operation;
        }

        std::vector<VkPipelineColorBlendAttachmentState> color_blend_attachment_states;
        for (const ColorAttachmentState &color_attachment_state : desc.color_attachment_states)
        {
            const BlendState &blend_state = color_attachment_state.blend_state;

            const VkPipelineColorBlendAttachmentState color_blend_attachment_state = {
                .blendEnable = blend_state.enable,
                .srcColorBlendFactor = map_blend_factor(blend_state.src_factor),
                .dstColorBlendFactor = map_blend_factor(blend_state.dst_factor),
                .colorBlendOp = map_blend_operation(blend_state.operation),
                .srcAlphaBlendFactor = map_blend_factor(blend_state.alpha_src_factor),
                .dstAlphaBlendFactor = map_blend_factor(blend_state.alpha_dst_factor),
                .alphaBlendOp = map_blend_operation(blend_state.alpha_operation),
                .colorWriteMask = map_color_writes(blend_state.color_writes),
            };

            color_blend_attachment_states.push_back(color_blend_attachment_state);
        }

        const VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .logicOpEnable = false,
            .logicOp = VK_LOGIC_OP_NO_OP,
            .attachmentCount = static_cast<u32>(color_blend_attachment_states.size()),
            .pAttachments = color_blend_attachment_states.data(),
            .blendConstants = { 0.0, 0.0, 0.0, 0.0 },
        };

        constexpr std::array<VkDynamicState, 2> dynamic_states = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
        };

        const VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .dynamicStateCount = static_cast<u32>(dynamic_states.size()),
            .pDynamicStates = dynamic_states.data(),
        };

        std::vector<VkFormat> color_attachment_formats;
        for (const ColorAttachmentState &color_attachment_state : desc.color_attachment_states)
        {
            color_attachment_formats.push_back(map_format(color_attachment_state.format));
        }

        const VkFormat depth_attachment_format = desc.depth_stencil_state.has_value()
            ? map_format(desc.depth_stencil_state.value().depth_format)
            : VK_FORMAT_UNDEFINED;
        const VkPipelineRenderingCreateInfo rendering_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
            .pNext = nullptr,
            .viewMask = 0,
            .colorAttachmentCount = static_cast<u32>(color_attachment_formats.size()),
            .pColorAttachmentFormats = color_attachment_formats.data(),
            .depthAttachmentFormat = depth_attachment_format,
            .stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
        };

        const VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = &rendering_create_info,
            .flags = 0,
            .stageCount = static_cast<u32>(shader_stage_create_infos.size()),
            .pStages = shader_stage_create_infos.data(),
            .pVertexInputState = &vertex_input_state_create_info,
            .pInputAssemblyState = &input_assembly_state_create_info,
            .pTessellationState = &tessellation_state_create_info,
            .pViewportState = &viewport_state_create_info,
            .pRasterizationState = &rasterization_state_create_info,
            .pMultisampleState = &multisample_state_create_info,
            .pDepthStencilState = &depth_stencil_state_create_info,
            .pColorBlendState = &color_blend_state_create_info,
            .pDynamicState = &dynamic_state_create_info,
            .layout = cast_ref<VulkanPipelineLayout>(desc.layout)->raw(),
            .renderPass = VK_NULL_HANDLE,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1,
        };

        HE_VK_CHECK(
            vkCreateGraphicsPipelines(
                m_graphics_device.device(), VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, nullptr, &m_raw),
            "Failed to create vulkan graphics pipeline");
        HE_ASSERT(m_raw != VK_NULL_HANDLE);
    }

    VulkanRenderPipeline::~VulkanRenderPipeline() { vkDestroyPipeline(m_graphics_device.device(), m_raw, nullptr); }
} // namespace he
