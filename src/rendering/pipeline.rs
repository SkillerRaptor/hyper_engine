/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use crate::rendering::device::Device;
use crate::rendering::error::{AlignError, Error};
use crate::rendering::swapchain::Swapchain;

use ash::vk;
use log::debug;

enum ShaderStage {
    Vertex,
    Fragment,
}

pub struct Pipeline {
    pipeline: vk::Pipeline,
    pipeline_layout: vk::PipelineLayout,
    device: std::rc::Rc<Device>,
}

impl Pipeline {
    pub fn new(
        device: &std::rc::Rc<Device>,
        swapchain: &std::rc::Rc<Swapchain>,
    ) -> Result<Self, Error> {
        let entry_point = std::ffi::CString::new("main")?;

        let vertex = include_bytes!("../../assets/shaders/default_shader_vertex.spv");
        let vertex_shader_module =
            Self::create_shader_module(ShaderStage::Vertex, &device.device, vertex)?;
        let vertex_stage = vk::PipelineShaderStageCreateInfo {
            stage: vk::ShaderStageFlags::VERTEX,
            module: vertex_shader_module,
            p_name: entry_point.as_ptr(),
            p_specialization_info: std::ptr::null(),
            ..Default::default()
        };

        let fragment = include_bytes!("../../assets/shaders/default_shader_fragment.spv");
        let fragment_shader_module =
            Self::create_shader_module(ShaderStage::Fragment, &device.device, fragment)?;
        let fragment_stage = vk::PipelineShaderStageCreateInfo {
            stage: vk::ShaderStageFlags::FRAGMENT,
            module: fragment_shader_module,
            p_name: entry_point.as_ptr(),
            p_specialization_info: std::ptr::null(),
            ..Default::default()
        };

        let vertex_input_state_create_info = vk::PipelineVertexInputStateCreateInfo {
            ..Default::default()
        };

        let input_assembly_state_create_info = vk::PipelineInputAssemblyStateCreateInfo {
            topology: vk::PrimitiveTopology::TRIANGLE_LIST,
            primitive_restart_enable: false as u32,
            ..Default::default()
        };

        let viewport = vk::Viewport {
            x: 0.0,
            y: 0.0,
            width: swapchain.extent.width as f32,
            height: swapchain.extent.height as f32,
            min_depth: 0.0,
            max_depth: 1.0,
            ..Default::default()
        };

        let scissor = vk::Rect2D {
            offset: vk::Offset2D { x: 0, y: 0 },
            extent: swapchain.extent,
            ..Default::default()
        };

        let viewports = &[viewport];
        let scissors = &[scissor];
        let viewport_state_create_info = vk::PipelineViewportStateCreateInfo {
            viewport_count: viewports.len() as u32,
            p_viewports: viewports.as_ptr(),
            scissor_count: scissors.len() as u32,
            p_scissors: scissors.as_ptr(),
            ..Default::default()
        };

        let rasterization_state_create_info = vk::PipelineRasterizationStateCreateInfo {
            depth_clamp_enable: false as u32,
            rasterizer_discard_enable: false as u32,
            polygon_mode: vk::PolygonMode::FILL,
            cull_mode: vk::CullModeFlags::BACK,
            front_face: vk::FrontFace::CLOCKWISE,
            depth_bias_enable: false as u32,
            depth_bias_constant_factor: f32::default(),
            depth_bias_clamp: f32::default(),
            depth_bias_slope_factor: f32::default(),
            line_width: 1.0,
            ..Default::default()
        };

        let multisample_state_create_info = vk::PipelineMultisampleStateCreateInfo {
            rasterization_samples: vk::SampleCountFlags::TYPE_1,
            sample_shading_enable: false as u32,
            min_sample_shading: f32::default(),
            p_sample_mask: std::ptr::null(),
            alpha_to_coverage_enable: vk::Bool32::default(),
            alpha_to_one_enable: vk::Bool32::default(),
            ..Default::default()
        };

        let attachment_state = vk::PipelineColorBlendAttachmentState {
            blend_enable: false as u32,
            src_color_blend_factor: vk::BlendFactor::ONE,
            dst_color_blend_factor: vk::BlendFactor::ZERO,
            color_blend_op: vk::BlendOp::ADD,
            src_alpha_blend_factor: vk::BlendFactor::ONE,
            dst_alpha_blend_factor: vk::BlendFactor::ZERO,
            alpha_blend_op: vk::BlendOp::ADD,
            color_write_mask: vk::ColorComponentFlags::RGBA,
            ..Default::default()
        };

        let attachments = &[attachment_state];
        let color_blend_state_create_info = vk::PipelineColorBlendStateCreateInfo {
            logic_op_enable: false as u32,
            logic_op: vk::LogicOp::COPY,
            attachment_count: attachments.len() as u32,
            p_attachments: attachments.as_ptr(),
            blend_constants: [0.0, 0.0, 0.0, 0.0],
            ..Default::default()
        };

        let pipeline_layout_info_create_info = vk::PipelineLayoutCreateInfo {
            set_layout_count: 0 as u32,
            p_set_layouts: std::ptr::null(),
            push_constant_range_count: 0,
            p_push_constant_ranges: std::ptr::null(),
            ..Default::default()
        };

        let pipeline_layout = unsafe {
            device
                .device
                .create_pipeline_layout(&pipeline_layout_info_create_info, None)?
        };

        let shader_stages = &[vertex_stage, fragment_stage];

        let pipeline_rendering_create_info = vk::PipelineRenderingCreateInfo {
            view_mask: 0,
            color_attachment_count: 1,
            p_color_attachment_formats: *&[swapchain.format].as_ptr(),
            depth_attachment_format: vk::Format::UNDEFINED,
            stencil_attachment_format: vk::Format::UNDEFINED,
            ..Default::default()
        };

        let graphics_pipeline_create_info = vk::GraphicsPipelineCreateInfo {
            p_next: unsafe { std::mem::transmute(&pipeline_rendering_create_info) },
            stage_count: shader_stages.len() as u32,
            p_stages: shader_stages.as_ptr(),
            p_vertex_input_state: &vertex_input_state_create_info,
            p_input_assembly_state: &input_assembly_state_create_info,
            p_tessellation_state: std::ptr::null(),
            p_viewport_state: &viewport_state_create_info,
            p_rasterization_state: &rasterization_state_create_info,
            p_multisample_state: &multisample_state_create_info,
            p_depth_stencil_state: std::ptr::null(),
            p_color_blend_state: &color_blend_state_create_info,
            p_dynamic_state: std::ptr::null(),
            layout: pipeline_layout,
            render_pass: vk::RenderPass::null(),
            subpass: 0,
            base_pipeline_handle: vk::Pipeline::null(),
            base_pipeline_index: -1,
            ..Default::default()
        };

        let pipeline = unsafe {
            match device.device.create_graphics_pipelines(
                vk::PipelineCache::null(),
                &[graphics_pipeline_create_info],
                None,
            ) {
                Ok(pipelines) => pipelines[0],
                Err(error) => return Err(Error::VulkanError(error.1)),
            }
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
            "  Viewport: x={}, y={}, width={}, height={}, min depth={}, max depth={}",
            viewport.x,
            viewport.y,
            viewport.width,
            viewport.height,
            viewport.min_depth,
            viewport.max_depth
        );
        debug!(
            "  Scissor: x={}, y={}, width={}, height={}",
            scissor.offset.x, scissor.offset.y, scissor.extent.width, scissor.extent.height
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
                .device
                .destroy_shader_module(fragment_shader_module, None);
            device
                .device
                .destroy_shader_module(vertex_shader_module, None);
        }

        debug!("Created vulkan pipeline");
        Ok(Self {
            pipeline,
            pipeline_layout,
            device: device.clone(),
        })
    }

    fn create_shader_module(
        shader_stage: ShaderStage,
        device: &ash::Device,
        bytecode: &[u8],
    ) -> Result<vk::ShaderModule, Error> {
        let bytecode = Vec::<u8>::from(bytecode);
        let (prefix, code, suffix) = unsafe { bytecode.align_to::<u32>() };
        if !prefix.is_empty() || !suffix.is_empty() {
            return Err(Error::AlignError(AlignError(
                "Shader bytecode is not properly aligned.",
            )));
        }

        let shader_module_create_info = vk::ShaderModuleCreateInfo {
            code_size: bytecode.len(),
            p_code: code.as_ptr(),
            ..Default::default()
        };

        let shader_module =
            unsafe { device.create_shader_module(&shader_module_create_info, None)? };

        let shader_stage = match shader_stage {
            ShaderStage::Vertex => "vertex",
            ShaderStage::Fragment => "fragment",
        };

        debug!("Created {} shader module", shader_stage,);

        Ok(shader_module)
    }
}

impl Drop for Pipeline {
    fn drop(&mut self) {
        unsafe {
            self.device.device.destroy_pipeline(self.pipeline, None);
            self.device
                .device
                .destroy_pipeline_layout(self.pipeline_layout, None);
        }
    }
}
