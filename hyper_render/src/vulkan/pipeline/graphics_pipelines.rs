/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::vulkan::{
    core::{debug_utils::DebugName, device::Device},
    pipeline::{pipeline_layout::PipelineLayout, Pipeline},
    resource::shader::Shader,
};

use ash::vk;
use color_eyre::Result;
use smallvec::SmallVec;
use std::{ffi::CStr, rc::Rc};

pub(crate) struct GraphicsPipeline {
    handle: vk::Pipeline,

    device: Rc<Device>,
}

impl GraphicsPipeline {
    pub(crate) fn new(device: Rc<Device>, create_info: GraphicsPipelineCreateInfo) -> Result<Self> {
        let GraphicsPipelineCreateInfo {
            label,

            layout,
            vertex_shader,
            fragment_shader,

            color_image_format,
            depth_image_format,

            input_assembly,
            rasterization_state,
            depth_stencil_state,
            color_blend_attachment_state,
            color_blend_state,
        } = create_info;

        // TODO: Add check for no depth image format, but still enabling depth testing
        // TODO: Add check if no shader at all was set

        let entry_name = unsafe { CStr::from_bytes_with_nul_unchecked(b"main\0") };

        let vertex_shader_stage_create_info = if let Some(ref vertex_shader) = vertex_shader {
            vk::PipelineShaderStageCreateInfo::builder()
                .stage(vk::ShaderStageFlags::VERTEX)
                .module(vertex_shader.handle())
                .name(entry_name)
        } else {
            vk::PipelineShaderStageCreateInfo::builder()
        };

        let fragment_shader_stage_create_info = if let Some(ref fragment_shader) = fragment_shader {
            vk::PipelineShaderStageCreateInfo::builder()
                .stage(vk::ShaderStageFlags::FRAGMENT)
                .module(fragment_shader.handle())
                .name(entry_name)
        } else {
            vk::PipelineShaderStageCreateInfo::builder()
        };

        let mut shader_stages = SmallVec::<[vk::PipelineShaderStageCreateInfo; 2]>::new();
        if vertex_shader.is_some() {
            shader_stages.push(*vertex_shader_stage_create_info);
        }

        if fragment_shader.is_some() {
            shader_stages.push(*fragment_shader_stage_create_info);
        }

        let dynamic_states = [vk::DynamicState::VIEWPORT, vk::DynamicState::SCISSOR];

        let dynamic_state_create_info =
            vk::PipelineDynamicStateCreateInfo::builder().dynamic_states(&dynamic_states);

        let vertex_input_state_create_info = vk::PipelineVertexInputStateCreateInfo::builder()
            .vertex_binding_descriptions(&[])
            .vertex_attribute_descriptions(&[]);

        let input_assembly_state_create_info = vk::PipelineInputAssemblyStateCreateInfo::builder()
            .topology(input_assembly.toplogy)
            .primitive_restart_enable(input_assembly.restart);

        let tessellation_state_create_info =
            vk::PipelineTessellationStateCreateInfo::builder().patch_control_points(0);

        let viewport_state_create_info = vk::PipelineViewportStateCreateInfo::builder()
            .viewport_count(1)
            .scissor_count(1);

        let rasterization_state_create_info = vk::PipelineRasterizationStateCreateInfo::builder()
            .depth_clamp_enable(false)
            .rasterizer_discard_enable(false)
            .polygon_mode(rasterization_state.polygon_mode)
            .line_width(1.0)
            .cull_mode(rasterization_state.cull_mode)
            .front_face(rasterization_state.front_face)
            .depth_bias_enable(rasterization_state.depth_bias_enable)
            .depth_bias_constant_factor(rasterization_state.depth_bias_constant_factor)
            .depth_bias_clamp(rasterization_state.depth_bias_clamp)
            .depth_bias_slope_factor(rasterization_state.depth_bias_slope_factor);

        // TODO: Add multisampling
        let multisample_state_create_info = vk::PipelineMultisampleStateCreateInfo::builder()
            .sample_shading_enable(false)
            .rasterization_samples(vk::SampleCountFlags::TYPE_1)
            .min_sample_shading(1.0)
            .sample_mask(&[])
            .alpha_to_coverage_enable(false)
            .alpha_to_one_enable(false);

        // TODO: Add stencil
        let depth_stencil_state_create_info = vk::PipelineDepthStencilStateCreateInfo::builder()
            .depth_test_enable(depth_stencil_state.depth_test_enable)
            .depth_write_enable(depth_stencil_state.depth_write_enable)
            .depth_compare_op(depth_stencil_state.depth_compare_op)
            .depth_bounds_test_enable(depth_stencil_state.depth_bounds_test_enable)
            .stencil_test_enable(false)
            .front(vk::StencilOpState::default())
            .back(vk::StencilOpState::default())
            .min_depth_bounds(depth_stencil_state.min_depth_bounds)
            .max_depth_bounds(depth_stencil_state.max_depth_bounds);

        let color_blend_attachment_state_create_info =
            vk::PipelineColorBlendAttachmentState::builder()
                .blend_enable(color_blend_attachment_state.blend_enable)
                .src_color_blend_factor(color_blend_attachment_state.src_color_blend_factor)
                .dst_color_blend_factor(color_blend_attachment_state.dst_color_blend_factor)
                .color_blend_op(color_blend_attachment_state.color_blend_op)
                .src_alpha_blend_factor(color_blend_attachment_state.src_alpha_blend_factor)
                .dst_alpha_blend_factor(color_blend_attachment_state.dst_alpha_blend_factor)
                .alpha_blend_op(color_blend_attachment_state.alpha_blend_op)
                .color_write_mask(color_blend_attachment_state.color_write_mask);

        let attachments = &[*color_blend_attachment_state_create_info];

        let color_blend_state_create_info = vk::PipelineColorBlendStateCreateInfo::builder()
            .logic_op_enable(color_blend_state.logic_op_enable)
            .logic_op(color_blend_state.logic_op)
            .attachments(attachments)
            .blend_constants(color_blend_state.blend_constants);

        let color_attachment_formats = &[color_image_format];
        let mut rendering_create_info = vk::PipelineRenderingCreateInfoKHR::builder()
            .color_attachment_formats(color_attachment_formats);

        if depth_stencil_state.depth_test_enable || depth_stencil_state.depth_write_enable {
            let depth_attachment_format = depth_image_format.unwrap_or_default();
            rendering_create_info =
                rendering_create_info.depth_attachment_format(depth_attachment_format);
        }

        let create_info = vk::GraphicsPipelineCreateInfo::builder()
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
            .layout(layout.handle())
            .render_pass(vk::RenderPass::null())
            .subpass(0)
            .base_pipeline_handle(vk::Pipeline::null())
            .base_pipeline_index(-1);

        let handle = unsafe {
            device.handle().create_graphics_pipelines(
                vk::PipelineCache::null(),
                &[*create_info],
                None,
            )
        }
        .map_err(|error| error.1)?[0];

        device.set_object_name(DebugName {
            ty: vk::ObjectType::PIPELINE,
            object: handle,
            name: label,
        })?;

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

pub(crate) struct GraphicsPipelineCreateInfo<'a> {
    pub(crate) label: &'a str,

    pub(crate) layout: &'a PipelineLayout,
    pub(crate) vertex_shader: Option<Shader>,
    pub(crate) fragment_shader: Option<Shader>,

    pub(crate) color_image_format: vk::Format,
    pub(crate) depth_image_format: Option<vk::Format>,

    pub(crate) input_assembly: InputAssemblyCreateInfo,
    pub(crate) rasterization_state: RasterizationStateCreateInfo,
    // TODO: Add multisampling
    pub(crate) depth_stencil_state: DepthStencilStateCreateInfo,
    pub(crate) color_blend_attachment_state: ColorBlendAttachmentStateCreateInfo,
    pub(crate) color_blend_state: ColorBlendStateCreateInfo,
}

#[derive(Clone, Copy, Debug)]
pub(crate) struct InputAssemblyCreateInfo {
    pub(crate) toplogy: vk::PrimitiveTopology,
    pub(crate) restart: bool,
}

impl Default for InputAssemblyCreateInfo {
    fn default() -> Self {
        Self {
            toplogy: vk::PrimitiveTopology::TRIANGLE_LIST,
            restart: false,
        }
    }
}

#[derive(Clone, Copy, Debug)]
pub(crate) struct RasterizationStateCreateInfo {
    pub(crate) polygon_mode: vk::PolygonMode,

    pub(crate) cull_mode: vk::CullModeFlags,
    pub(crate) front_face: vk::FrontFace,

    pub(crate) depth_bias_enable: bool,
    pub(crate) depth_bias_constant_factor: f32,
    pub(crate) depth_bias_clamp: f32,
    pub(crate) depth_bias_slope_factor: f32,
}

impl Default for RasterizationStateCreateInfo {
    fn default() -> Self {
        Self {
            polygon_mode: vk::PolygonMode::FILL,

            cull_mode: vk::CullModeFlags::NONE,
            front_face: vk::FrontFace::COUNTER_CLOCKWISE,

            depth_bias_enable: false,
            depth_bias_constant_factor: 0.0,
            depth_bias_clamp: 0.0,
            depth_bias_slope_factor: 0.0,
        }
    }
}

#[derive(Clone, Copy, Debug)]
pub(crate) struct DepthStencilStateCreateInfo {
    pub(crate) depth_test_enable: bool,
    pub(crate) depth_write_enable: bool,
    pub(crate) depth_compare_op: vk::CompareOp,

    pub(crate) depth_bounds_test_enable: bool,
    pub(crate) min_depth_bounds: f32,
    pub(crate) max_depth_bounds: f32,
    // TODO: Add stencil
}

impl Default for DepthStencilStateCreateInfo {
    fn default() -> Self {
        Self {
            depth_test_enable: false,
            depth_write_enable: false,
            depth_compare_op: vk::CompareOp::NEVER,

            depth_bounds_test_enable: false,
            min_depth_bounds: 0.0,
            max_depth_bounds: 0.0,
        }
    }
}

#[derive(Clone, Copy, Debug)]
pub(crate) struct ColorBlendAttachmentStateCreateInfo {
    pub(crate) blend_enable: bool,

    pub(crate) src_color_blend_factor: vk::BlendFactor,
    pub(crate) dst_color_blend_factor: vk::BlendFactor,
    pub(crate) color_blend_op: vk::BlendOp,

    pub(crate) src_alpha_blend_factor: vk::BlendFactor,
    pub(crate) dst_alpha_blend_factor: vk::BlendFactor,
    pub(crate) alpha_blend_op: vk::BlendOp,

    pub(crate) color_write_mask: vk::ColorComponentFlags,
}

impl Default for ColorBlendAttachmentStateCreateInfo {
    fn default() -> Self {
        Self {
            blend_enable: false,

            src_color_blend_factor: vk::BlendFactor::ZERO,
            dst_color_blend_factor: vk::BlendFactor::ZERO,
            color_blend_op: vk::BlendOp::ADD,

            src_alpha_blend_factor: vk::BlendFactor::ZERO,
            dst_alpha_blend_factor: vk::BlendFactor::ZERO,
            alpha_blend_op: vk::BlendOp::ADD,

            color_write_mask: vk::ColorComponentFlags::RGBA,
        }
    }
}

#[derive(Clone, Copy, Debug)]
pub(crate) struct ColorBlendStateCreateInfo {
    pub(crate) logic_op_enable: bool,
    pub(crate) logic_op: vk::LogicOp,

    pub(crate) blend_constants: [f32; 4],
}

impl Default for ColorBlendStateCreateInfo {
    fn default() -> Self {
        Self {
            logic_op_enable: false,
            logic_op: vk::LogicOp::NO_OP,

            blend_constants: [0.0; 4],
        }
    }
}
