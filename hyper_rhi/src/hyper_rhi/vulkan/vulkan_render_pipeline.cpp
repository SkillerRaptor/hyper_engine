/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/vulkan_render_pipeline.hpp"

#include <hyper_core/assertion.hpp>
#include <hyper_core/logger.hpp>
#include <hyper_core/ref_ptr.hpp>

#include "hyper_rhi/vulkan/vulkan_graphics_device.hpp"
#include "hyper_rhi/vulkan/vulkan_pipeline_layout.hpp"
#include "hyper_rhi/vulkan/vulkan_shader_module.hpp"
#include "hyper_rhi/vulkan/vulkan_texture.hpp"

namespace hyper_engine
{
    RefPtr<RenderPipeline> VulkanGraphicsDevice::create_render_pipeline_platform(const RenderPipelineDescriptor &descriptor)
    {
        const VulkanShaderModule &vertex_shader_module = static_cast<const VulkanShaderModule &>(*descriptor.vertex_shader);

        const VkPipelineShaderStageCreateInfo vertex_shader_stage_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stage = VK_SHADER_STAGE_VERTEX_BIT,
            .module = vertex_shader_module.shader_module(),
            .pName = vertex_shader_module.entry_name().data(),
            .pSpecializationInfo = nullptr,
        };

        const VulkanShaderModule &fragment_shader_module = static_cast<const VulkanShaderModule &>(*descriptor.fragment_shader);

        const VkPipelineShaderStageCreateInfo fragment_shader_stage_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
            .module = fragment_shader_module.shader_module(),
            .pName = fragment_shader_module.entry_name().data(),
            .pSpecializationInfo = nullptr,
        };

        const std::array<VkPipelineShaderStageCreateInfo, 2> shader_stage_create_infos = {
            vertex_shader_stage_create_info,
            fragment_shader_stage_create_info,
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

        const VkPrimitiveTopology primitive_topology = VulkanRenderPipeline::get_primitive_topology(descriptor.primitive_state.topology);
        const VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .topology = primitive_topology,
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

        const VkPolygonMode polygon_mode = VulkanRenderPipeline::get_polygon_mode(descriptor.primitive_state.polygon_mode);
        const VkCullModeFlags cull_mode = VulkanRenderPipeline::get_cull_mode_flags(descriptor.primitive_state.cull_mode);
        const VkFrontFace front_face = VulkanRenderPipeline::get_front_face(descriptor.primitive_state.front_face);
        const VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .depthClampEnable = false,
            .rasterizerDiscardEnable = false,
            .polygonMode = polygon_mode,
            .cullMode = cull_mode,
            .frontFace = front_face,
            .depthBiasEnable = descriptor.depth_stencil_state.depth_bias_state.depth_bias_enable,
            .depthBiasConstantFactor = descriptor.depth_stencil_state.depth_bias_state.constant,
            .depthBiasClamp = descriptor.depth_stencil_state.depth_bias_state.clamp,
            .depthBiasSlopeFactor = descriptor.depth_stencil_state.depth_bias_state.slope,
            .lineWidth = 1.0,
        };

        // FIXME: Add multisampling
        constexpr VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable = false,
            .minSampleShading = 1.0,
            .pSampleMask = nullptr,
            .alphaToCoverageEnable = false,
            .alphaToOneEnable = false,
        };

        // FIXME: Add stencil
        // FIXME: Add depth bounds
        const VkCompareOp depth_compare_operation =
            VulkanRenderPipeline::get_compare_operation(descriptor.depth_stencil_state.depth_compare_operation);
        const VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .depthTestEnable = descriptor.depth_stencil_state.depth_test_enable,
            .depthWriteEnable = descriptor.depth_stencil_state.depth_write_enable,
            .depthCompareOp = depth_compare_operation,
            .depthBoundsTestEnable = false,
            .stencilTestEnable = false,
            .front = {},
            .back = {},
            .minDepthBounds = 0.0,
            .maxDepthBounds = 1.0,
        };

        std::vector<VkPipelineColorBlendAttachmentState> color_blend_attachment_states = {};
        for (const ColorAttachmentState &color_attachment_state : descriptor.color_attachment_states)
        {
            const BlendState &blend_state = color_attachment_state.blend_state;

            const VkBlendFactor src_color_blend_factor = VulkanRenderPipeline::get_blend_factor(blend_state.src_blend_factor);
            const VkBlendFactor dst_color_blend_factor = VulkanRenderPipeline::get_blend_factor(blend_state.dst_blend_factor);
            const VkBlendOp color_blend_operation = VulkanRenderPipeline::get_blend_operation(blend_state.operation);
            const VkBlendFactor alpha_src_blend_factor = VulkanRenderPipeline::get_blend_factor(blend_state.alpha_src_blend_factor);
            const VkBlendFactor alpha_dst_blend_factor = VulkanRenderPipeline::get_blend_factor(blend_state.alpha_dst_blend_factor);
            const VkBlendOp alpha_blend_operation = VulkanRenderPipeline::get_blend_operation(blend_state.alpha_operation);
            const VkColorComponentFlags color_write_mask = VulkanRenderPipeline::get_color_component_flags(blend_state.color_writes);
            const VkPipelineColorBlendAttachmentState color_blend_attachment_state = {
                .blendEnable = blend_state.blend_enable,
                .srcColorBlendFactor = src_color_blend_factor,
                .dstColorBlendFactor = dst_color_blend_factor,
                .colorBlendOp = color_blend_operation,
                .srcAlphaBlendFactor = alpha_src_blend_factor,
                .dstAlphaBlendFactor = alpha_dst_blend_factor,
                .alphaBlendOp = alpha_blend_operation,
                .colorWriteMask = color_write_mask,
            };

            color_blend_attachment_states.push_back(color_blend_attachment_state);
        }

        const VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .logicOpEnable = false,
            .logicOp = VK_LOGIC_OP_NO_OP,
            .attachmentCount = static_cast<uint32_t>(color_blend_attachment_states.size()),
            .pAttachments = color_blend_attachment_states.data(),
            .blendConstants =
                {
                    0.0,
                    0.0,
                    0.0,
                    0.0,
                },
        };

        constexpr std::array<VkDynamicState, 2> dynamic_states = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
        };

        const VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .dynamicStateCount = static_cast<uint32_t>(dynamic_states.size()),
            .pDynamicStates = dynamic_states.data(),
        };

        std::vector<VkFormat> color_attachment_formats = {};
        for (const ColorAttachmentState &color_attachment_state : descriptor.color_attachment_states)
        {
            color_attachment_formats.push_back(VulkanTexture::get_format(color_attachment_state.format));
        }

        const VkFormat depth_attachment_format = VulkanTexture::get_format(descriptor.depth_stencil_state.depth_format);
        const VkPipelineRenderingCreateInfo rendering_create_info = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
            .pNext = nullptr,
            .viewMask = 0,
            .colorAttachmentCount = static_cast<uint32_t>(color_attachment_formats.size()),
            .pColorAttachmentFormats = color_attachment_formats.data(),
            .depthAttachmentFormat = depth_attachment_format,
            .stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
        };

        const VulkanPipelineLayout &layout = static_cast<const VulkanPipelineLayout &>(*descriptor.layout);

        const VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {
            .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext = &rendering_create_info,
            .flags = 0,
            .stageCount = static_cast<uint32_t>(shader_stage_create_infos.size()),
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
            .layout = layout.pipeline_layout(),
            .renderPass = VK_NULL_HANDLE,
            .subpass = 0,
            .basePipelineHandle = VK_NULL_HANDLE,
            .basePipelineIndex = -1,
        };

        VkPipeline pipeline = VK_NULL_HANDLE;
        HE_VK_CHECK(vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, nullptr, &pipeline));

        HE_ASSERT(pipeline != VK_NULL_HANDLE);

        set_object_name(pipeline, ObjectType::Pipeline, descriptor.label);

        return make_ref<VulkanRenderPipeline>(descriptor, *this, pipeline);
    }

    VulkanRenderPipeline::VulkanRenderPipeline(
        const RenderPipelineDescriptor &descriptor,
        VulkanGraphicsDevice &graphics_device,
        const VkPipeline pipeline)
        : RenderPipeline(descriptor)
        , m_graphics_device(graphics_device)
        , m_pipeline(pipeline)
    {
    }

    VulkanRenderPipeline::~VulkanRenderPipeline()
    {
        m_graphics_device.resource_queue().graphics_pipelines.emplace_back(m_pipeline);
    }

    VkPipeline VulkanRenderPipeline::pipeline() const
    {
        return m_pipeline;
    }

    VkPrimitiveTopology VulkanRenderPipeline::get_primitive_topology(const PrimitiveTopology primitive_topology)
    {
        switch (primitive_topology)
        {
        case PrimitiveTopology::PointList:
            return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        case PrimitiveTopology::LineList:
            return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        case PrimitiveTopology::LineStrip:
            return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
        case PrimitiveTopology::TriangleList:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        case PrimitiveTopology::TriangleStrip:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        case PrimitiveTopology::TriangleFan:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
        default:
            HE_UNREACHABLE();
        }
    }

    VkPolygonMode VulkanRenderPipeline::get_polygon_mode(const PolygonMode polygon_mode)
    {
        switch (polygon_mode)
        {
        case PolygonMode::Fill:
            return VK_POLYGON_MODE_FILL;
        case PolygonMode::Line:
            return VK_POLYGON_MODE_LINE;
        case PolygonMode::Point:
            return VK_POLYGON_MODE_POINT;
        default:
            HE_UNREACHABLE();
        }
    }

    VkCullModeFlags VulkanRenderPipeline::get_cull_mode_flags(const Face face)
    {
        switch (face)
        {
        case Face::None:
            return VK_CULL_MODE_NONE;
        case Face::Front:
            return VK_CULL_MODE_FRONT_BIT;
        case Face::Back:
            return VK_CULL_MODE_BACK_BIT;
        default:
            HE_UNREACHABLE();
        }
    }

    VkFrontFace VulkanRenderPipeline::get_front_face(const FrontFace front_face)
    {
        switch (front_face)
        {
        case FrontFace::CounterClockwise:
            return VK_FRONT_FACE_COUNTER_CLOCKWISE;
        case FrontFace::Clockwise:
            return VK_FRONT_FACE_CLOCKWISE;
        default:
            HE_UNREACHABLE();
        }
    }

    VkCompareOp VulkanRenderPipeline::get_compare_operation(const CompareOperation compare_operation)
    {
        switch (compare_operation)
        {
        case CompareOperation::Never:
            return VK_COMPARE_OP_NEVER;
        case CompareOperation::Less:
            return VK_COMPARE_OP_LESS;
        case CompareOperation::Equal:
            return VK_COMPARE_OP_EQUAL;
        case CompareOperation::LessEqual:
            return VK_COMPARE_OP_LESS_OR_EQUAL;
        case CompareOperation::Greater:
            return VK_COMPARE_OP_GREATER;
        case CompareOperation::NotEqual:
            return VK_COMPARE_OP_NOT_EQUAL;
        case CompareOperation::GreaterEqual:
            return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case CompareOperation::Always:
            return VK_COMPARE_OP_ALWAYS;
        default:
            HE_UNREACHABLE();
        }
    }

    VkBlendFactor VulkanRenderPipeline::get_blend_factor(const BlendFactor blend_factor)
    {
        switch (blend_factor)
        {
        case BlendFactor::Zero:
            return VK_BLEND_FACTOR_ZERO;
        case BlendFactor::One:
            return VK_BLEND_FACTOR_ONE;
        case BlendFactor::SrcColor:
            return VK_BLEND_FACTOR_SRC_COLOR;
        case BlendFactor::OneMinusSrcColor:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        case BlendFactor::DstColor:
            return VK_BLEND_FACTOR_DST_COLOR;
        case BlendFactor::OneMinusDstColor:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case BlendFactor::SrcAlpha:
            return VK_BLEND_FACTOR_SRC_ALPHA;
        case BlendFactor::OneMinusSrcAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case BlendFactor::DstAlpha:
            return VK_BLEND_FACTOR_DST_ALPHA;
        case BlendFactor::OneMinusDstAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        case BlendFactor::ConstantColor:
            return VK_BLEND_FACTOR_CONSTANT_ALPHA;
        case BlendFactor::OneMinusConstantColor:
            return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
        case BlendFactor::ConstantAlpha:
            return VK_BLEND_FACTOR_CONSTANT_ALPHA;
        case BlendFactor::OneMinusConstantAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
        case BlendFactor::SrcAlphaSaturate:
            return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
        case BlendFactor::Src1Color:
            return VK_BLEND_FACTOR_SRC1_COLOR;
        case BlendFactor::OneMinusSrc1Color:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
        case BlendFactor::Src1Alpha:
            return VK_BLEND_FACTOR_SRC1_ALPHA;
        case BlendFactor::OneMinusSrc1Alpha:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
        default:
            HE_UNREACHABLE();
        }
    }

    VkBlendOp VulkanRenderPipeline::get_blend_operation(const BlendOperation blend_operation)
    {
        switch (blend_operation)
        {
        case BlendOperation::Add:
            return VK_BLEND_OP_ADD;
        case BlendOperation::Subtract:
            return VK_BLEND_OP_SUBTRACT;
        case BlendOperation::ReverseSubtract:
            return VK_BLEND_OP_REVERSE_SUBTRACT;
        case BlendOperation::Min:
            return VK_BLEND_OP_MIN;
        case BlendOperation::Max:
            return VK_BLEND_OP_MAX;
        default:
            HE_UNREACHABLE();
        }
    }

    VkColorComponentFlags VulkanRenderPipeline::get_color_component_flags(const BitFlags<ColorWrites> color_writes)
    {
        VkColorComponentFlags color_component_flags = 0;
        if ((color_writes & ColorWrites::R) == ColorWrites::R)
        {
            color_component_flags |= VK_COLOR_COMPONENT_R_BIT;
        }

        if ((color_writes & ColorWrites::G) == ColorWrites::G)
        {
            color_component_flags |= VK_COLOR_COMPONENT_G_BIT;
        }

        if ((color_writes & ColorWrites::B) == ColorWrites::B)
        {
            color_component_flags |= VK_COLOR_COMPONENT_B_BIT;
        }

        if ((color_writes & ColorWrites::A) == ColorWrites::A)
        {
            color_component_flags |= VK_COLOR_COMPONENT_A_BIT;
        }

        return color_component_flags;
    }
} // namespace hyper_engine