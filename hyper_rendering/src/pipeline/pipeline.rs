/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use super::super::devices::device::Device;
use super::super::error::{AlignError, Error};
use super::super::vertex::Vertex;
use super::swapchain::Swapchain;

use ash::vk;
use log::debug;
use nalgebra_glm as glm;
use std::mem::size_of;

enum ShaderStage {
    Vertex,
    Fragment,
}

pub struct Pipeline {
    pipeline: vk::Pipeline,
    pipeline_layout: vk::PipelineLayout,
}

impl Pipeline {
    pub fn new(device: &Device, swapchain: &Swapchain) -> Result<Self, Error> {
        let entry_point = std::ffi::CString::new("main")?;

        let specialization_info = vk::SpecializationInfo::builder();

        let vertex = include_bytes!("../../../assets/shaders/default_shader_vertex.spv");
        let vertex_shader_module =
            Self::create_shader_module(&device, ShaderStage::Vertex, vertex)?;
        let vertex_stage = vk::PipelineShaderStageCreateInfo::builder()
            .stage(vk::ShaderStageFlags::VERTEX)
            .module(vertex_shader_module)
            .name(&entry_point)
            .specialization_info(&specialization_info);

        let fragment = include_bytes!("../../../assets/shaders/default_shader_fragment.spv");
        let fragment_shader_module =
            Self::create_shader_module(&device, ShaderStage::Fragment, fragment)?;
        let fragment_stage = vk::PipelineShaderStageCreateInfo::builder()
            .stage(vk::ShaderStageFlags::FRAGMENT)
            .module(fragment_shader_module)
            .name(&entry_point)
            .specialization_info(&specialization_info);

        let binding_description = Vertex::binding_descriptions();
        let attribute_description = Vertex::attribute_descriptions();
        let vertex_input_state_create_info = vk::PipelineVertexInputStateCreateInfo::builder()
            .vertex_binding_descriptions(&binding_description)
            .vertex_attribute_descriptions(&attribute_description);

        let input_assembly_state_create_info = vk::PipelineInputAssemblyStateCreateInfo::builder()
            .topology(vk::PrimitiveTopology::TRIANGLE_LIST)
            .primitive_restart_enable(false);

        let tessellation_state_create_info = vk::PipelineTessellationStateCreateInfo::builder();

        let viewport_state_create_info = vk::PipelineViewportStateCreateInfo::builder()
            .viewports(&[])
            .viewport_count(1)
            .scissors(&[])
            .scissor_count(1);

        let rasterization_state_create_info = vk::PipelineRasterizationStateCreateInfo::builder()
            .depth_clamp_enable(false)
            .rasterizer_discard_enable(false)
            .polygon_mode(vk::PolygonMode::FILL)
            .cull_mode(vk::CullModeFlags::NONE)
            .front_face(vk::FrontFace::CLOCKWISE)
            .depth_bias_enable(false)
            .depth_bias_constant_factor(0.0)
            .depth_bias_clamp(0.0)
            .depth_bias_slope_factor(0.0)
            .line_width(1.0);

        let multisample_state_create_info = vk::PipelineMultisampleStateCreateInfo::builder()
            .rasterization_samples(vk::SampleCountFlags::TYPE_1)
            .sample_shading_enable(false)
            .min_sample_shading(0.0)
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

        let attachment_state = vk::PipelineColorBlendAttachmentState::builder()
            .blend_enable(false)
            .src_color_blend_factor(vk::BlendFactor::ONE)
            .dst_color_blend_factor(vk::BlendFactor::ZERO)
            .color_blend_op(vk::BlendOp::ADD)
            .src_alpha_blend_factor(vk::BlendFactor::ONE)
            .dst_alpha_blend_factor(vk::BlendFactor::ZERO)
            .alpha_blend_op(vk::BlendOp::ADD)
            .color_write_mask(vk::ColorComponentFlags::RGBA);

        let attachments = &[*attachment_state];
        let color_blend_state_create_info = vk::PipelineColorBlendStateCreateInfo::builder()
            .logic_op_enable(false)
            .logic_op(vk::LogicOp::COPY)
            .attachments(attachments)
            .blend_constants([0.0, 0.0, 0.0, 0.0]);

        let dynamic_states = &[vk::DynamicState::VIEWPORT, vk::DynamicState::SCISSOR];
        let dynamic_state_create_info =
            vk::PipelineDynamicStateCreateInfo::builder().dynamic_states(dynamic_states);

        let push_constant = vk::PushConstantRange::builder()
            .stage_flags(vk::ShaderStageFlags::VERTEX)
            .offset(0)
            .size(size_of::<MeshPushConstants>() as u32);

        let set_layouts = &[];
        let push_constant_ranges = &[*push_constant];
        let pipeline_layout_info_create_info = vk::PipelineLayoutCreateInfo::builder()
            .set_layouts(set_layouts)
            .push_constant_ranges(push_constant_ranges);

        let pipeline_layout = unsafe {
            device
                .logical_device()
                .create_pipeline_layout(&pipeline_layout_info_create_info, None)?
        };

        let color_attachment_formats = &[*swapchain.format()];
        let depth_attachment_format = *swapchain.depth_format();
        let mut pipeline_rendering_create_info = vk::PipelineRenderingCreateInfo::builder()
            .view_mask(0)
            .color_attachment_formats(color_attachment_formats)
            .depth_attachment_format(depth_attachment_format)
            .stencil_attachment_format(vk::Format::UNDEFINED);

        let shader_stages = &[*vertex_stage, *fragment_stage];
        let graphics_pipeline_create_info = vk::GraphicsPipelineCreateInfo::builder()
            .push_next(&mut pipeline_rendering_create_info)
            .stages(shader_stages)
            .vertex_input_state(&vertex_input_state_create_info)
            .input_assembly_state(&input_assembly_state_create_info)
            .tessellation_state(&tessellation_state_create_info)
            .viewport_state(&viewport_state_create_info)
            .rasterization_state(&rasterization_state_create_info)
            .multisample_state(&multisample_state_create_info)
            .depth_stencil_state(&depth_stencil_state_create_info)
            .color_blend_state(&color_blend_state_create_info)
            .dynamic_state(&dynamic_state_create_info)
            .layout(pipeline_layout)
            .render_pass(vk::RenderPass::null())
            .subpass(0)
            .base_pipeline_handle(vk::Pipeline::null())
            .base_pipeline_index(-1);

        let pipeline = unsafe {
            device
                .logical_device()
                .create_graphics_pipelines(
                    vk::PipelineCache::null(),
                    &[*graphics_pipeline_create_info],
                    None,
                )
                .map_err(|error| error.1)?[0]
        };

        debug!("Pipeline Info:");
        debug!(
            "  Topology: {:?}",
            input_assembly_state_create_info.topology
        );
        debug!(
            "  Primitive Restart Enable: {}",
            input_assembly_state_create_info.primitive_restart_enable != 0
        );
        debug!(
            "  Depth Clamp Enable: {}",
            rasterization_state_create_info.depth_clamp_enable != 0
        );
        debug!(
            "  Rasterizer Discard Enable: {}",
            rasterization_state_create_info.rasterizer_discard_enable != 0
        );
        debug!(
            "  Polygon Mode: {:?}",
            rasterization_state_create_info.polygon_mode
        );
        debug!(
            "  Cull Mode: {:?}",
            rasterization_state_create_info.cull_mode
        );
        debug!(
            "  Front Face: {:?}",
            rasterization_state_create_info.front_face
        );
        debug!(
            "  Depth Bias Enable: {}",
            rasterization_state_create_info.depth_bias_enable != 0
        );
        debug!(
            "  Depth Bias Constant Factor: {}",
            rasterization_state_create_info.depth_bias_constant_factor
        );
        debug!(
            "  Depth Bias Clamp: {}",
            rasterization_state_create_info.depth_bias_clamp
        );
        debug!(
            "  Depth Bias Slope Factor: {}",
            rasterization_state_create_info.depth_bias_slope_factor
        );
        debug!(
            "  Line Width: {}",
            rasterization_state_create_info.line_width
        );
        debug!(
            "  Rasterization Samples: {:?}",
            multisample_state_create_info.rasterization_samples
        );
        debug!(
            "  Sample Shading Enable: {}",
            multisample_state_create_info.sample_shading_enable != 0
        );
        debug!(
            "  Min Sample Shading: {}",
            multisample_state_create_info.min_sample_shading
        );
        debug!(
            "  Alpha to Coverage Enable: {}",
            multisample_state_create_info.alpha_to_coverage_enable != 0
        );
        debug!(
            "  Alpha to One Enable: {}",
            multisample_state_create_info.alpha_to_one_enable != 0
        );
        debug!("  Blend Enable: {}", attachment_state.blend_enable != 0);
        debug!(
            "  Src Color Blend Factor: {:?}",
            attachment_state.src_color_blend_factor
        );
        debug!(
            "  Dst Color Blend Factor: {:?}",
            attachment_state.dst_color_blend_factor
        );
        debug!("  Color Blend Op: {:?}", attachment_state.color_blend_op);
        debug!(
            "  Src Alpha Blend Factor: {:?}",
            attachment_state.src_alpha_blend_factor
        );
        debug!(
            "  Dst Alpha Blend Factor: {:?}",
            attachment_state.dst_alpha_blend_factor
        );
        debug!("  Alpha Blend Op: {:?}", attachment_state.alpha_blend_op);
        debug!(
            "  Color Write Mask: {:?}",
            attachment_state.color_write_mask
        );
        debug!(
            "  Logic Op Enable: {:?}",
            color_blend_state_create_info.logic_op_enable
        );
        debug!("  Logic Op: {:?}", color_blend_state_create_info.logic_op);
        debug!(
            "  Blend Constants: {:?}",
            color_blend_state_create_info.blend_constants
        );
        debug!(
            "  Attachment Count: {:?}",
            color_blend_state_create_info.attachment_count
        );
        debug!("  Dynamic States: {:?}", dynamic_states);
        debug!(
            "  Descriptor Set Layout Count: {}",
            pipeline_layout_info_create_info.set_layout_count
        );
        debug!(
            "  Push Constant Range Count: {}",
            pipeline_layout_info_create_info.push_constant_range_count
        );
        debug!(
            "  View Mask: {:?}",
            pipeline_rendering_create_info.view_mask
        );
        debug!(
            "  Color Attachment Count: {:?}",
            pipeline_rendering_create_info.color_attachment_count
        );
        debug!(
            "  Depth Attachment Format: {:?}",
            pipeline_rendering_create_info.depth_attachment_format
        );
        debug!(
            "  Stencil Attachment Format: {:?}",
            pipeline_rendering_create_info.stencil_attachment_format
        );

        unsafe {
            device
                .logical_device()
                .destroy_shader_module(fragment_shader_module, None);
            device
                .logical_device()
                .destroy_shader_module(vertex_shader_module, None);
        }

        debug!("Created vulkan pipeline");
        Ok(Self {
            pipeline,
            pipeline_layout,
        })
    }

    fn create_shader_module(
        device: &Device,
        shader_stage: ShaderStage,
        bytecode: &[u8],
    ) -> Result<vk::ShaderModule, Error> {
        let bytecode = Vec::<u8>::from(bytecode);
        let (prefix, code, suffix) = unsafe { bytecode.align_to::<u32>() };
        if !prefix.is_empty() || !suffix.is_empty() {
            return Err(Error::AlignError(AlignError(
                "Shader bytecode is not properly aligned.",
            )));
        }

        let shader_module_create_info = vk::ShaderModuleCreateInfo::builder().code(code);

        let shader_module = unsafe {
            device
                .logical_device()
                .create_shader_module(&shader_module_create_info, None)?
        };

        let shader_stage = match shader_stage {
            ShaderStage::Vertex => "vertex",
            ShaderStage::Fragment => "fragment",
        };

        debug!("Created {} shader module", shader_stage,);

        Ok(shader_module)
    }

    pub fn cleanup(&mut self, device: &Device) {
        unsafe {
            device
                .logical_device()
                .destroy_pipeline(self.pipeline, None);
            device
                .logical_device()
                .destroy_pipeline_layout(self.pipeline_layout, None);
        }
    }

    pub fn pipeline(&self) -> &vk::Pipeline {
        &self.pipeline
    }

    pub fn pipeline_layout(&self) -> &vk::PipelineLayout {
        &self.pipeline_layout
    }
}

pub struct MeshPushConstants {
    pub data: glm::Vec4,
    pub render_matrix: glm::Mat4,
}
