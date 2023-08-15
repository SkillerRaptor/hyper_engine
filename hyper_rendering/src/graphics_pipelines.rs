/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    device::Device,
    error::{CreationError, CreationResult},
    pipeline::Pipeline,
    pipeline_layout::PipelineLayout,
    shader::Shader,
    swapchain::Swapchain,
};

use ash::vk;
use std::{ffi::CStr, rc::Rc};

pub(crate) struct GraphicsPipeline {
    handle: vk::Pipeline,

    device: Rc<Device>,
}

impl GraphicsPipeline {
    pub(crate) fn new(
        device: Rc<Device>,
        layout: &PipelineLayout,
        swapchain: &Swapchain,
        vertex_shader: Shader,
        fragment_shader: Shader,
    ) -> CreationResult<Self> {
        let entry_name = unsafe { CStr::from_bytes_with_nul_unchecked(b"main\0") };

        let vertex_create_info = vk::PipelineShaderStageCreateInfo::builder()
            .stage(vk::ShaderStageFlags::VERTEX)
            .module(vertex_shader.handle())
            .name(entry_name);

        let fragment_create_info = vk::PipelineShaderStageCreateInfo::builder()
            .stage(vk::ShaderStageFlags::FRAGMENT)
            .module(fragment_shader.handle())
            .name(entry_name);

        let shader_stages = [*vertex_create_info, *fragment_create_info];

        let dynamic_states = [vk::DynamicState::VIEWPORT, vk::DynamicState::SCISSOR];

        let dynamic_state_create_info =
            vk::PipelineDynamicStateCreateInfo::builder().dynamic_states(&dynamic_states);

        let vertex_input_state_create_info = vk::PipelineVertexInputStateCreateInfo::builder()
            .vertex_binding_descriptions(&[])
            .vertex_attribute_descriptions(&[]);

        let input_assembly_state_create_info = vk::PipelineInputAssemblyStateCreateInfo::builder()
            .topology(vk::PrimitiveTopology::TRIANGLE_LIST)
            .primitive_restart_enable(false);

        let viewport_state_create_info = vk::PipelineViewportStateCreateInfo::builder()
            .viewport_count(1)
            .scissor_count(1);

        let rasterization_state_create_info = vk::PipelineRasterizationStateCreateInfo::builder()
            .depth_clamp_enable(false)
            .rasterizer_discard_enable(false)
            .polygon_mode(vk::PolygonMode::FILL)
            .line_width(1.0)
            .cull_mode(vk::CullModeFlags::BACK)
            .front_face(vk::FrontFace::COUNTER_CLOCKWISE)
            .depth_bias_enable(false)
            .depth_bias_constant_factor(0.0)
            .depth_bias_clamp(0.0)
            .depth_bias_slope_factor(0.0);

        let multisample_state_create_info = vk::PipelineMultisampleStateCreateInfo::builder()
            .sample_shading_enable(false)
            .rasterization_samples(vk::SampleCountFlags::TYPE_1)
            .min_sample_shading(1.0)
            .sample_mask(&[])
            .alpha_to_coverage_enable(false)
            .alpha_to_one_enable(false);

        let depth_stencil_state_create_info = vk::PipelineDepthStencilStateCreateInfo::builder()
            .depth_test_enable(true)
            .depth_write_enable(true)
            .depth_compare_op(vk::CompareOp::LESS_OR_EQUAL)
            .depth_bounds_test_enable(false)
            .stencil_test_enable(false)
            .front(vk::StencilOpState::default())
            .back(vk::StencilOpState::default())
            .min_depth_bounds(0.0)
            .max_depth_bounds(1.0);

        let color_blend_attachment_state = vk::PipelineColorBlendAttachmentState::builder()
            .color_write_mask(vk::ColorComponentFlags::RGBA)
            .blend_enable(false)
            .src_color_blend_factor(vk::BlendFactor::ONE)
            .dst_color_blend_factor(vk::BlendFactor::ZERO)
            .color_blend_op(vk::BlendOp::ADD)
            .src_alpha_blend_factor(vk::BlendFactor::ONE)
            .dst_alpha_blend_factor(vk::BlendFactor::ZERO)
            .alpha_blend_op(vk::BlendOp::ADD);

        let attachments = &[*color_blend_attachment_state];

        let color_blend_state_create_info = vk::PipelineColorBlendStateCreateInfo::builder()
            .logic_op_enable(false)
            .logic_op(vk::LogicOp::COPY)
            .attachments(attachments)
            .blend_constants([0.0; 4]);

        let color_attachment_formats = &[swapchain.format()];
        let depth_attachment_format = swapchain.depth_image_format();
        let mut rendering_create_info = vk::PipelineRenderingCreateInfoKHR::builder()
            .color_attachment_formats(color_attachment_formats)
            .depth_attachment_format(depth_attachment_format);

        let create_info = vk::GraphicsPipelineCreateInfo::builder()
            .push_next(&mut rendering_create_info)
            .stages(&shader_stages)
            .vertex_input_state(&vertex_input_state_create_info)
            .input_assembly_state(&input_assembly_state_create_info)
            .viewport_state(&viewport_state_create_info)
            .rasterization_state(&rasterization_state_create_info)
            .multisample_state(&multisample_state_create_info)
            .depth_stencil_state(&depth_stencil_state_create_info)
            .color_blend_state(&color_blend_state_create_info)
            .dynamic_state(&dynamic_state_create_info)
            .layout(layout.handle())
            .render_pass(vk::RenderPass::null())
            .subpass(0)
            .base_pipeline_handle(vk::Pipeline::null())
            .base_pipeline_index(-1);

        let handle = unsafe {
            device
                .handle()
                .create_graphics_pipelines(vk::PipelineCache::null(), &[*create_info], None)
                .map_err(|error| CreationError::VulkanCreation(error.1, "graphics pipeline"))
        }?[0];

        Ok(Self { handle, device })
    }
}

impl Drop for GraphicsPipeline {
    fn drop(&mut self) {
        unsafe {
            self.device.handle().destroy_pipeline(self.handle, None);
        }
    }
}

impl Pipeline for GraphicsPipeline {
    fn handle(&self) -> vk::Pipeline {
        self.handle
    }
}
