/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use std::ffi::CStr;

use ash::vk;

use crate::{
    render_pipeline::RenderPipelineDescriptor,
    shader_compiler::{self, OutputApi, ShaderStage},
    vulkan::GraphicsDevice,
};

pub struct RenderPipeline {
    pipeline: vk::Pipeline,
    graphics_device: GraphicsDevice,
}

impl RenderPipeline {
    pub(crate) fn new(
        graphics_device: &GraphicsDevice,
        descriptor: &RenderPipelineDescriptor,
    ) -> Self {
        // NOTE: Maybe instead of failure compile shaders on its own for multiple pipelines and don't panic
        let vertex_shader = shader_compiler::compile(
            descriptor.vertex_shader,
            ShaderStage::Vertex,
            OutputApi::Vulkan,
        )
        .expect("failed to compile vertex shader");
        let vertex_shader_module =
            Self::create_shader_module(graphics_device, &vertex_shader, "vertex");

        let pixel_shader = shader_compiler::compile(
            descriptor.pixel_shader,
            ShaderStage::Pixel,
            OutputApi::Vulkan,
        )
        .expect("failed to compile pixel shader");
        let pixel_shader_module =
            Self::create_shader_module(graphics_device, &pixel_shader, "pixel");

        let pipeline =
            Self::create_pipeline(graphics_device, vertex_shader_module, pixel_shader_module);

        unsafe {
            graphics_device
                .device()
                .destroy_shader_module(vertex_shader_module, None);
            graphics_device
                .device()
                .destroy_shader_module(pixel_shader_module, None);
        }

        Self {
            pipeline,
            graphics_device: graphics_device.clone(),
        }
    }

    fn create_shader_module(
        graphics_device: &GraphicsDevice,
        code: &[u8],
        stage: &str,
    ) -> vk::ShaderModule {
        let (prefix, code, suffix) = unsafe { code.align_to::<u32>() };
        if !prefix.is_empty() || !suffix.is_empty() {
            panic!("failed to align {} shader", stage);
        }

        let create_info = vk::ShaderModuleCreateInfo::default().code(code);

        let shader_module = unsafe {
            graphics_device
                .device()
                .create_shader_module(&create_info, None)
        }
        .expect(&format!("failed to create {} shader module", stage));
        shader_module
    }

    fn create_pipeline(
        graphics_device: &GraphicsDevice,
        vertex_shader: vk::ShaderModule,
        pixel_shader: vk::ShaderModule,
    ) -> vk::Pipeline {
        let entry_name = unsafe { CStr::from_bytes_with_nul_unchecked(b"main\0") };

        let vertex_shader_stage_create_info = vk::PipelineShaderStageCreateInfo::default()
            .stage(vk::ShaderStageFlags::VERTEX)
            .module(vertex_shader)
            .name(entry_name);

        let pixel_shader_stage_create_info = vk::PipelineShaderStageCreateInfo::default()
            .stage(vk::ShaderStageFlags::FRAGMENT)
            .module(pixel_shader)
            .name(entry_name);

        let shader_stages = [
            vertex_shader_stage_create_info,
            pixel_shader_stage_create_info,
        ];
        let dynamic_states = [vk::DynamicState::VIEWPORT, vk::DynamicState::SCISSOR];

        let dynamic_state_create_info =
            vk::PipelineDynamicStateCreateInfo::default().dynamic_states(&dynamic_states);

        let vertex_input_state_create_info = vk::PipelineVertexInputStateCreateInfo::default()
            .vertex_binding_descriptions(&[])
            .vertex_attribute_descriptions(&[]);

        let input_assembly_state_create_info = vk::PipelineInputAssemblyStateCreateInfo::default()
            .topology(vk::PrimitiveTopology::TRIANGLE_LIST)
            .primitive_restart_enable(false);

        let tessellation_state_create_info =
            vk::PipelineTessellationStateCreateInfo::default().patch_control_points(0);

        let viewport_state_create_info = vk::PipelineViewportStateCreateInfo::default()
            .viewport_count(1)
            .scissor_count(1);

        let rasterization_state_create_info = vk::PipelineRasterizationStateCreateInfo::default()
            .depth_clamp_enable(false)
            .rasterizer_discard_enable(false)
            .polygon_mode(vk::PolygonMode::FILL)
            .line_width(1.0)
            .cull_mode(vk::CullModeFlags::NONE)
            .front_face(vk::FrontFace::COUNTER_CLOCKWISE)
            .depth_bias_enable(false)
            .depth_bias_constant_factor(0.0)
            .depth_bias_clamp(0.0)
            .depth_bias_slope_factor(0.0);

        // TODO: Add multisampling
        let multisample_state_create_info = vk::PipelineMultisampleStateCreateInfo::default()
            .sample_shading_enable(false)
            .rasterization_samples(vk::SampleCountFlags::TYPE_1)
            .min_sample_shading(1.0)
            .sample_mask(&[])
            .alpha_to_coverage_enable(false)
            .alpha_to_one_enable(false);

        // TODO: Add stencil
        let depth_stencil_state_create_info = vk::PipelineDepthStencilStateCreateInfo::default()
            .depth_test_enable(false)
            .depth_write_enable(false)
            .depth_compare_op(vk::CompareOp::NEVER)
            .depth_bounds_test_enable(false)
            .stencil_test_enable(false)
            .front(vk::StencilOpState::default())
            .back(vk::StencilOpState::default())
            .min_depth_bounds(0.0)
            .max_depth_bounds(0.0);

        let color_blend_attachment_state_create_info =
            vk::PipelineColorBlendAttachmentState::default()
                .blend_enable(false)
                .src_color_blend_factor(vk::BlendFactor::ONE)
                .dst_color_blend_factor(vk::BlendFactor::ZERO)
                .color_blend_op(vk::BlendOp::ADD)
                .src_alpha_blend_factor(vk::BlendFactor::ONE)
                .dst_alpha_blend_factor(vk::BlendFactor::ZERO)
                .alpha_blend_op(vk::BlendOp::ADD)
                .color_write_mask(vk::ColorComponentFlags::RGBA);

        let attachments = &[color_blend_attachment_state_create_info];

        let color_blend_state_create_info = vk::PipelineColorBlendStateCreateInfo::default()
            .logic_op_enable(false)
            .logic_op(vk::LogicOp::NO_OP)
            .attachments(attachments)
            .blend_constants([0.0; 4]);

        let color_attachment_formats = &[vk::Format::B8G8R8A8_UNORM];
        let mut rendering_create_info = vk::PipelineRenderingCreateInfoKHR::default()
            .color_attachment_formats(color_attachment_formats);

        let create_info = vk::GraphicsPipelineCreateInfo::default()
            .push_next(&mut rendering_create_info)
            .stages(&shader_stages)
            .vertex_input_state(&vertex_input_state_create_info)
            .input_assembly_state(&input_assembly_state_create_info)
            .tessellation_state(&tessellation_state_create_info)
            .viewport_state(&viewport_state_create_info)
            .rasterization_state(&rasterization_state_create_info)
            .multisample_state(&multisample_state_create_info)
            .depth_stencil_state(&depth_stencil_state_create_info)
            .color_blend_state(&color_blend_state_create_info)
            .dynamic_state(&dynamic_state_create_info)
            .layout(graphics_device.pipeline_layout())
            .render_pass(vk::RenderPass::null())
            .subpass(0)
            .base_pipeline_handle(vk::Pipeline::null())
            .base_pipeline_index(-1);

        let pipeline = unsafe {
            graphics_device
                .device()
                .create_graphics_pipelines(vk::PipelineCache::null(), &[create_info], None)
                .expect("failed to create render pipeline")[0]
        };
        pipeline
    }

    pub(crate) fn pipeline(&self) -> vk::Pipeline {
        self.pipeline
    }
}

impl Drop for RenderPipeline {
    fn drop(&mut self) {
        unsafe {
            self.graphics_device
                .device()
                .destroy_pipeline(self.pipeline, None);
        }
    }
}
