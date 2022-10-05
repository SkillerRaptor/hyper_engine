/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use crate::vertex::Vertex;

use ash::{
    vk::{
        self, BlendFactor, BlendOp, ColorComponentFlags, CompareOp, CullModeFlags,
        DescriptorSetLayout, DynamicState, Format, FrontFace, GraphicsPipelineCreateInfo, LogicOp,
        PipelineCache, PipelineColorBlendAttachmentState, PipelineColorBlendStateCreateInfo,
        PipelineDepthStencilStateCreateInfo, PipelineDynamicStateCreateInfo,
        PipelineInputAssemblyStateCreateInfo, PipelineLayout, PipelineLayoutCreateInfo,
        PipelineMultisampleStateCreateInfo, PipelineRasterizationStateCreateInfo,
        PipelineRenderingCreateInfo, PipelineShaderStageCreateInfo,
        PipelineTessellationStateCreateInfo, PipelineVertexInputStateCreateInfo,
        PipelineViewportStateCreateInfo, PolygonMode, PrimitiveTopology, PushConstantRange,
        RenderPass, SampleCountFlags, ShaderModule, ShaderModuleCreateInfo, ShaderStageFlags,
        SpecializationInfo, StencilOpState,
    },
    Device,
};
use log::debug;
use std::{ffi::CString, mem};
use thiserror::Error;

#[repr(C)]
pub(crate) struct BindingsOffset {
    pub bindings_offset: u32,
    pub unused0: u32,
    pub unused1: u32,
    pub unused2: u32,
}

enum ShaderStage {
    Vertex,
    Fragment,
}

#[allow(clippy::enum_variant_names)]
#[derive(Debug, Error)]
pub enum PipelineCreationError {
    #[error("Failed to create vulkan pipeline")]
    PipelineCreation(vk::Result),

    #[error("Failed to create vulkan pipeline layout")]
    PipelineLayoutCreation(vk::Result),

    #[error("Failed to create vulkan shader module")]
    ShaderModuleCreation(vk::Result),
}

pub(crate) struct PipelineCreateInfo<'a> {
    pub logical_device: &'a Device,
    pub descriptor_set_layouts: &'a [DescriptorSetLayout],
    pub image_format: &'a Format,
    pub depth_image_format: &'a Format,
}

pub(crate) struct Pipeline {
    pipeline: vk::Pipeline,
    pipeline_layout: PipelineLayout,

    logical_device: Device,
}

impl Pipeline {
    pub fn new(create_info: &PipelineCreateInfo) -> Result<Self, PipelineCreationError> {
        let pipeline_layout = Self::create_pipeline_layout(
            create_info.logical_device,
            create_info.descriptor_set_layouts,
        )?;

        let pipeline = Self::create_pipeline(
            create_info.logical_device,
            create_info.image_format,
            create_info.depth_image_format,
            &pipeline_layout,
        )?;

        Ok(Self {
            pipeline,
            pipeline_layout,

            logical_device: create_info.logical_device.clone(),
        })
    }

    fn create_pipeline_layout(
        logical_device: &Device,
        descriptor_set_layouts: &[DescriptorSetLayout],
    ) -> Result<PipelineLayout, PipelineCreationError> {
        // TODO: Don't hardcode this instead replace it with a struct
        let push_constant = PushConstantRange::builder()
            .stage_flags(ShaderStageFlags::ALL)
            .offset(0)
            .size(mem::size_of::<BindingsOffset>() as u32);

        let set_layouts = descriptor_set_layouts;
        let push_constant_ranges = [*push_constant];
        let pipeline_layout_info_create_info = PipelineLayoutCreateInfo::builder()
            .set_layouts(set_layouts)
            .push_constant_ranges(&push_constant_ranges);

        let pipeline_layout = unsafe {
            logical_device
                .create_pipeline_layout(&pipeline_layout_info_create_info, None)
                .map_err(PipelineCreationError::PipelineLayoutCreation)?
        };

        debug!("Created pipeline layout");

        Self::print_pipeline_layout_information(&pipeline_layout_info_create_info);

        Ok(pipeline_layout)
    }

    fn print_pipeline_layout_information(
        pipeline_layout_info_create_info: &PipelineLayoutCreateInfo,
    ) {
        debug!("Pipeline Layout Info:");
        debug!(
            "  Descriptor Set Layout Count: {}",
            pipeline_layout_info_create_info.set_layout_count
        );
        debug!(
            "  Push Constant Range Count: {}",
            pipeline_layout_info_create_info.push_constant_range_count
        );
    }

    fn create_pipeline(
        logical_device: &Device,
        image_format: &Format,
        depth_image_format: &Format,
        pipeline_layout: &PipelineLayout,
    ) -> Result<vk::Pipeline, PipelineCreationError> {
        // TODO: Propagate error
        let entry_point = CString::new("main").expect("FIXME");

        let specialization_info = SpecializationInfo::builder();

        let vertex = include_bytes!("../../../assets/shaders/compiled/default_shader_vs.spv");
        let vertex_shader_module =
            Self::create_shader_module(logical_device, ShaderStage::Vertex, vertex)?;
        let vertex_stage = PipelineShaderStageCreateInfo::builder()
            .stage(ShaderStageFlags::VERTEX)
            .module(vertex_shader_module)
            .name(&entry_point)
            .specialization_info(&specialization_info);

        let fragment = include_bytes!("../../../assets/shaders/compiled/default_shader_ps.spv");
        let fragment_shader_module =
            Self::create_shader_module(logical_device, ShaderStage::Fragment, fragment)?;
        let fragment_stage = PipelineShaderStageCreateInfo::builder()
            .stage(ShaderStageFlags::FRAGMENT)
            .module(fragment_shader_module)
            .name(&entry_point)
            .specialization_info(&specialization_info);

        // FIXME: REMOVE COMMENTS
        let _binding_description = Vertex::binding_descriptions();
        let _attribute_description = Vertex::attribute_descriptions();
        let vertex_input_state_create_info = PipelineVertexInputStateCreateInfo::builder();
        //.vertex_binding_descriptions(&binding_description)
        //.vertex_attribute_descriptions(&attribute_description);

        let input_assembly_state_create_info = PipelineInputAssemblyStateCreateInfo::builder()
            .topology(PrimitiveTopology::TRIANGLE_LIST)
            .primitive_restart_enable(false);

        let tessellation_state_create_info = PipelineTessellationStateCreateInfo::builder();

        let viewport_state_create_info = PipelineViewportStateCreateInfo::builder()
            .viewports(&[])
            .viewport_count(1)
            .scissors(&[])
            .scissor_count(1);

        let rasterization_state_create_info = PipelineRasterizationStateCreateInfo::builder()
            .depth_clamp_enable(false)
            .rasterizer_discard_enable(false)
            .polygon_mode(PolygonMode::FILL)
            .cull_mode(CullModeFlags::NONE)
            .front_face(FrontFace::CLOCKWISE)
            .depth_bias_enable(false)
            .depth_bias_constant_factor(0.0)
            .depth_bias_clamp(0.0)
            .depth_bias_slope_factor(0.0)
            .line_width(1.0);

        let multisample_state_create_info = PipelineMultisampleStateCreateInfo::builder()
            .rasterization_samples(SampleCountFlags::TYPE_1)
            .sample_shading_enable(false)
            .min_sample_shading(0.0)
            .sample_mask(&[])
            .alpha_to_coverage_enable(false)
            .alpha_to_one_enable(false);

        let depth_stencil_state_create_info = PipelineDepthStencilStateCreateInfo::builder()
            .depth_test_enable(true)
            .depth_write_enable(true)
            .depth_compare_op(CompareOp::LESS_OR_EQUAL)
            .depth_bounds_test_enable(false)
            .stencil_test_enable(false)
            .front(StencilOpState::default())
            .back(StencilOpState::default())
            .min_depth_bounds(0.0)
            .max_depth_bounds(1.0);

        let attachment_state = PipelineColorBlendAttachmentState::builder()
            .blend_enable(false)
            .src_color_blend_factor(BlendFactor::ONE)
            .dst_color_blend_factor(BlendFactor::ZERO)
            .color_blend_op(BlendOp::ADD)
            .src_alpha_blend_factor(BlendFactor::ONE)
            .dst_alpha_blend_factor(BlendFactor::ZERO)
            .alpha_blend_op(BlendOp::ADD)
            .color_write_mask(ColorComponentFlags::RGBA);

        let attachments = &[*attachment_state];
        let color_blend_state_create_info = PipelineColorBlendStateCreateInfo::builder()
            .logic_op_enable(false)
            .logic_op(LogicOp::COPY)
            .attachments(attachments)
            .blend_constants([0.0, 0.0, 0.0, 0.0]);

        let dynamic_states = &[DynamicState::VIEWPORT, DynamicState::SCISSOR];
        let dynamic_state_create_info =
            PipelineDynamicStateCreateInfo::builder().dynamic_states(dynamic_states);

        let color_attachment_formats = &[*image_format];
        let depth_attachment_format = *depth_image_format;
        let mut pipeline_rendering_create_info = PipelineRenderingCreateInfo::builder()
            .view_mask(0)
            .color_attachment_formats(color_attachment_formats)
            .depth_attachment_format(depth_attachment_format)
            .stencil_attachment_format(Format::UNDEFINED);

        let shader_stages = &[*vertex_stage, *fragment_stage];
        let graphics_pipeline_create_info = GraphicsPipelineCreateInfo::builder()
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
            .layout(*pipeline_layout)
            .render_pass(RenderPass::null())
            .subpass(0)
            .base_pipeline_handle(vk::Pipeline::null())
            .base_pipeline_index(-1);

        let pipeline = unsafe {
            logical_device
                .create_graphics_pipelines(
                    PipelineCache::null(),
                    &[*graphics_pipeline_create_info],
                    None,
                )
                .map_err(|error| PipelineCreationError::PipelineCreation(error.1))?[0]
        };

        unsafe {
            logical_device.destroy_shader_module(fragment_shader_module, None);
            logical_device.destroy_shader_module(vertex_shader_module, None);
        }

        debug!("Created pipeline");

        Self::print_pipeline_information(
            &input_assembly_state_create_info,
            &rasterization_state_create_info,
            &multisample_state_create_info,
            &attachment_state,
            &color_blend_state_create_info,
            &pipeline_rendering_create_info,
            dynamic_states,
        );

        Ok(pipeline)
    }

    fn print_pipeline_information(
        input_assembly_state_create_info: &PipelineInputAssemblyStateCreateInfo,
        rasterization_state_create_info: &PipelineRasterizationStateCreateInfo,
        multisample_state_create_info: &PipelineMultisampleStateCreateInfo,
        attachment_state: &PipelineColorBlendAttachmentState,
        color_blend_state_create_info: &PipelineColorBlendStateCreateInfo,
        pipeline_rendering_create_info: &PipelineRenderingCreateInfo,
        dynamic_states: &[DynamicState],
    ) {
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
        debug!("  Dynamic States: {:?}", dynamic_states);
    }

    fn create_shader_module(
        logical_device: &Device,
        shader_stage: ShaderStage,
        bytecode: &[u8],
    ) -> Result<ShaderModule, PipelineCreationError> {
        let bytecode = Vec::<u8>::from(bytecode);
        let (prefix, code, suffix) = unsafe { bytecode.align_to::<u32>() };
        if !prefix.is_empty() || !suffix.is_empty() {
            panic!("Failed to align shader bytecode");
        }

        let shader_module_create_info = ShaderModuleCreateInfo::builder().code(code);

        let shader_module = unsafe {
            logical_device
                .create_shader_module(&shader_module_create_info, None)
                .map_err(PipelineCreationError::ShaderModuleCreation)?
        };

        let shader_stage = match shader_stage {
            ShaderStage::Vertex => "vertex",
            ShaderStage::Fragment => "fragment",
        };

        debug!("Created {} shader module", shader_stage);

        Ok(shader_module)
    }

    pub fn pipeline(&self) -> &vk::Pipeline {
        &self.pipeline
    }

    pub fn pipeline_layout(&self) -> &PipelineLayout {
        &self.pipeline_layout
    }
}

impl Drop for Pipeline {
    fn drop(&mut self) {
        unsafe {
            self.logical_device.destroy_pipeline(self.pipeline, None);
            self.logical_device
                .destroy_pipeline_layout(self.pipeline_layout, None);
        }
    }
}
