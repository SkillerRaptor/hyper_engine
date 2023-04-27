/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{command_buffer::CommandBuffer, device::Device, swapchain::Swapchain};

use ash::vk::{
    self, AccessFlags, AttachmentLoadOp, AttachmentStoreOp, BlendFactor, BlendOp, ClearValue,
    ColorComponentFlags, CullModeFlags, DependencyFlags, DynamicState, FrontFace,
    GraphicsPipelineCreateInfo, Image, ImageAspectFlags, ImageLayout, ImageMemoryBarrier,
    ImageSubresourceRange, ImageView, LogicOp, Offset2D, PipelineBindPoint, PipelineCache,
    PipelineColorBlendAttachmentState, PipelineColorBlendStateCreateInfo,
    PipelineDynamicStateCreateInfo, PipelineInputAssemblyStateCreateInfo, PipelineLayout,
    PipelineLayoutCreateInfo, PipelineMultisampleStateCreateInfo,
    PipelineRasterizationStateCreateInfo, PipelineRenderingCreateInfoKHR,
    PipelineShaderStageCreateInfo, PipelineStageFlags, PipelineVertexInputStateCreateInfo,
    PipelineViewportStateCreateInfo, PolygonMode, PrimitiveTopology, Rect2D, RenderPass,
    RenderingAttachmentInfoKHR, RenderingInfoKHR, SampleCountFlags, ShaderModule,
    ShaderModuleCreateInfo, ShaderStageFlags,
};
use std::{
    ffi::CStr,
    fs::File,
    io::{self, BufReader, Read},
    sync::Arc,
};
use thiserror::Error;

/// An enum representing the errors that can occur while constructing a pipeline
#[derive(Debug, Error)]
pub enum CreationError {
    /// Creation of a vulkan object failed
    #[error("creation of vulkan {1} failed")]
    Creation(#[source] vk::Result, &'static str),

    /// Open of file failed
    #[error("file failed to open")]
    OpenFailure(#[source] io::Error),

    /// Read of file failed
    #[error("file failed to be read")]
    ReadFailure(#[source] io::Error),

    /// Unaligned shader code
    #[error("shader is unaligned")]
    Unaligned,
}

/// A struct representing a wrapper for the vulkan pipeline
pub(crate) struct Pipeline {
    /// Pipeline itself
    graphics_pipeline: vk::Pipeline,

    /// Layout of the pipeline
    graphics_pipeline_layout: PipelineLayout,

    /// Device Wrapper
    device: Arc<Device>,
}

impl Pipeline {
    /// Constructs a new pipeline
    ///
    /// Arguments:
    ///
    /// * `device`: Device wrapper
    /// * `swapchain`: Swapchain wrapper
    pub(crate) fn new(device: &Arc<Device>, swapchain: &Swapchain) -> Result<Self, CreationError> {
        let graphics_pipeline_layout = Self::create_graphics_pipeline_layout(device)?;
        let graphics_pipeline =
            Self::create_graphics_pipeline(device, swapchain, &graphics_pipeline_layout)?;

        Ok(Self {
            graphics_pipeline,
            graphics_pipeline_layout,
            device: device.clone(),
        })
    }

    /// Creates a new graphics pipeline layout
    ///
    /// Arguments:
    ///
    /// * `device`: Device wrapper
    fn create_graphics_pipeline_layout(
        device: &Arc<Device>,
    ) -> Result<PipelineLayout, CreationError> {
        let pipeline_layout_create_info = PipelineLayoutCreateInfo::builder()
            .set_layouts(&[])
            .push_constant_ranges(&[]);

        let pipeline_layout = unsafe {
            device
                .handle()
                .create_pipeline_layout(&pipeline_layout_create_info, None)
                .map_err(|error| CreationError::Creation(error, "pipeline layout"))
        }?;

        Ok(pipeline_layout)
    }

    /// Creates a new graphics pipeline
    ///
    /// Arguments:
    ///
    /// * `device`: Device wrapper
    /// * `render_pass`: Render pass of the pipeline
    /// * `pipeline_layout`: Layout of the pipeline
    fn create_graphics_pipeline(
        device: &Arc<Device>,
        swapchain: &Swapchain,
        pipeline_layout: &PipelineLayout,
    ) -> Result<vk::Pipeline, CreationError> {
        let vertex_shader_code = Self::parse_shader("./assets/shaders/compiled/triangle_vert.spv")?;
        let fragment_shader_code =
            Self::parse_shader("./assets/shaders/compiled/triangle_frag.spv")?;

        let vertex_shader_module = Self::create_shader_module(device, &vertex_shader_code)?;
        let fragment_shader_module = Self::create_shader_module(device, &fragment_shader_code)?;

        let entry_name = unsafe { CStr::from_bytes_with_nul_unchecked(b"main\0") };

        let vertex_pipeline_shader_stage_create_info = PipelineShaderStageCreateInfo::builder()
            .stage(ShaderStageFlags::VERTEX)
            .module(vertex_shader_module)
            .name(entry_name);

        let fragment_pipeline_shader_stage_create_info = PipelineShaderStageCreateInfo::builder()
            .stage(ShaderStageFlags::FRAGMENT)
            .module(fragment_shader_module)
            .name(entry_name);

        let shader_stages = [
            *vertex_pipeline_shader_stage_create_info,
            *fragment_pipeline_shader_stage_create_info,
        ];

        let dynamic_states = [DynamicState::VIEWPORT, DynamicState::SCISSOR];

        let pipeline_dynamic_state_create_info =
            PipelineDynamicStateCreateInfo::builder().dynamic_states(&dynamic_states);

        let pipeline_vertex_input_state_create_info = PipelineVertexInputStateCreateInfo::builder()
            .vertex_binding_descriptions(&[])
            .vertex_attribute_descriptions(&[]);

        let pipeline_input_assembly_state_create_info =
            PipelineInputAssemblyStateCreateInfo::builder()
                .topology(PrimitiveTopology::TRIANGLE_LIST)
                .primitive_restart_enable(false);

        let pipeline_viewport_state_create_info = PipelineViewportStateCreateInfo::builder()
            .viewport_count(1)
            .scissor_count(1);

        let pipeline_rasterization_state_create_info =
            PipelineRasterizationStateCreateInfo::builder()
                .depth_clamp_enable(false)
                .rasterizer_discard_enable(false)
                .polygon_mode(PolygonMode::FILL)
                .line_width(1.0)
                .cull_mode(CullModeFlags::BACK)
                .front_face(FrontFace::CLOCKWISE)
                .depth_bias_enable(false)
                .depth_bias_constant_factor(0.0)
                .depth_bias_clamp(0.0)
                .depth_bias_slope_factor(0.0);

        let pipeline_multisample_state_create_info = PipelineMultisampleStateCreateInfo::builder()
            .sample_shading_enable(false)
            .rasterization_samples(SampleCountFlags::TYPE_1)
            .min_sample_shading(1.0)
            .sample_mask(&[])
            .alpha_to_coverage_enable(false)
            .alpha_to_one_enable(false);

        let pipeline_color_blend_attachment_state = PipelineColorBlendAttachmentState::builder()
            .color_write_mask(ColorComponentFlags::RGBA)
            .blend_enable(false)
            .src_color_blend_factor(BlendFactor::ONE)
            .dst_color_blend_factor(BlendFactor::ZERO)
            .color_blend_op(BlendOp::ADD)
            .src_alpha_blend_factor(BlendFactor::ONE)
            .dst_alpha_blend_factor(BlendFactor::ZERO)
            .alpha_blend_op(BlendOp::ADD);

        let attachments = &[*pipeline_color_blend_attachment_state];

        let pipeline_color_blend_state_create_info = PipelineColorBlendStateCreateInfo::builder()
            .logic_op_enable(false)
            .logic_op(LogicOp::COPY)
            .attachments(attachments)
            .blend_constants([0.0; 4]);

        let color_attachment_formats = &[*swapchain.format()];
        let mut pipeline_rendering_create_info = PipelineRenderingCreateInfoKHR::builder()
            .color_attachment_formats(color_attachment_formats);

        let graphics_pipeline_create_info = GraphicsPipelineCreateInfo::builder()
            .push_next(&mut pipeline_rendering_create_info)
            .stages(&shader_stages)
            .vertex_input_state(&pipeline_vertex_input_state_create_info)
            .input_assembly_state(&pipeline_input_assembly_state_create_info)
            .viewport_state(&pipeline_viewport_state_create_info)
            .rasterization_state(&pipeline_rasterization_state_create_info)
            .multisample_state(&pipeline_multisample_state_create_info)
            // .depth_stencil_state(depth_stencil_state)
            .color_blend_state(&pipeline_color_blend_state_create_info)
            .dynamic_state(&pipeline_dynamic_state_create_info)
            .layout(*pipeline_layout)
            .render_pass(RenderPass::null())
            .subpass(0)
            .base_pipeline_handle(vk::Pipeline::null())
            .base_pipeline_index(-1);

        let graphics_pipeline = unsafe {
            device
                .handle()
                .create_graphics_pipelines(
                    PipelineCache::null(),
                    &[*graphics_pipeline_create_info],
                    None,
                )
                .map_err(|error| CreationError::Creation(error.1, "graphics pipeline"))
        }?;

        unsafe {
            device
                .handle()
                .destroy_shader_module(vertex_shader_module, None);
            device
                .handle()
                .destroy_shader_module(fragment_shader_module, None);
        }

        Ok(graphics_pipeline[0])
    }

    /// Parses the bytes of a shader
    ///
    /// Arguments:
    ///
    /// * `file`: Shader file to parse
    fn parse_shader(file: &str) -> Result<Vec<u8>, CreationError> {
        let file = File::open(file).map_err(CreationError::OpenFailure)?;
        let mut reader = BufReader::new(file);

        let mut buffer = Vec::new();
        reader
            .read_to_end(&mut buffer)
            .map_err(CreationError::ReadFailure)?;

        Ok(buffer)
    }

    /// Creates a new shader module
    ///
    /// Arguments:
    ///
    /// * `device`: Device wrapper
    /// * `shader_bytes`: Bytes of the shader
    fn create_shader_module(
        device: &Arc<Device>,
        shader_bytes: &[u8],
    ) -> Result<ShaderModule, CreationError> {
        let (prefix, code, suffix) = unsafe { shader_bytes.align_to::<u32>() };
        if !prefix.is_empty() || !suffix.is_empty() {
            return Err(CreationError::Unaligned);
        }

        let shader_module_create_info = ShaderModuleCreateInfo::builder().code(code);

        let shader_module = unsafe {
            device
                .handle()
                .create_shader_module(&shader_module_create_info, None)
                .map_err(|error| CreationError::Creation(error, "shader module"))
        }?;

        Ok(shader_module)
    }

    /// Begins the rendering
    pub(crate) fn begin_rendering(
        &self,
        swapchain: &Swapchain,
        command_buffer: &CommandBuffer,
        image: &Image,
        image_view: &ImageView,
        clear_value: ClearValue,
    ) {
        let subresource_range = ImageSubresourceRange::builder()
            .aspect_mask(ImageAspectFlags::COLOR)
            .base_mip_level(0)
            .level_count(1)
            .base_array_layer(0)
            .layer_count(1);

        let image_memory_barrier = ImageMemoryBarrier::builder()
            .dst_access_mask(AccessFlags::COLOR_ATTACHMENT_WRITE)
            .old_layout(ImageLayout::UNDEFINED)
            .new_layout(ImageLayout::COLOR_ATTACHMENT_OPTIMAL)
            .image(*image)
            .subresource_range(*subresource_range);

        unsafe {
            self.device.handle().cmd_pipeline_barrier(
                *command_buffer.handle(),
                PipelineStageFlags::TOP_OF_PIPE,
                PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT,
                DependencyFlags::empty(),
                &[],
                &[],
                &[*image_memory_barrier],
            );
        }

        let render_area_extent = swapchain.extent();
        let render_area_offset = Offset2D::builder().x(0).y(0);

        let render_area = Rect2D::builder()
            .extent(*render_area_extent)
            .offset(*render_area_offset);

        let color_attachment_info = RenderingAttachmentInfoKHR::builder()
            .image_view(*image_view)
            .image_layout(ImageLayout::ATTACHMENT_OPTIMAL_KHR)
            .load_op(AttachmentLoadOp::CLEAR)
            .store_op(AttachmentStoreOp::STORE)
            .clear_value(clear_value);

        let color_attachments = &[*color_attachment_info];
        let rendering_info = RenderingInfoKHR::builder()
            .render_area(*render_area)
            .layer_count(1)
            .color_attachments(color_attachments);

        unsafe {
            self.device
                .handle()
                .cmd_begin_rendering(*command_buffer.handle(), &rendering_info);
        }
    }

    /// Ends the rendering
    pub(crate) fn end_rendering(&self, command_buffer: &CommandBuffer, image: &Image) {
        unsafe {
            self.device
                .handle()
                .cmd_end_rendering(*command_buffer.handle());
        }

        let subresource_range = ImageSubresourceRange::builder()
            .aspect_mask(ImageAspectFlags::COLOR)
            .base_mip_level(0)
            .level_count(1)
            .base_array_layer(0)
            .layer_count(1);

        let image_memory_barrier = ImageMemoryBarrier::builder()
            .src_access_mask(AccessFlags::COLOR_ATTACHMENT_WRITE)
            .old_layout(ImageLayout::COLOR_ATTACHMENT_OPTIMAL)
            .new_layout(ImageLayout::PRESENT_SRC_KHR)
            .image(*image)
            .subresource_range(*subresource_range);

        unsafe {
            self.device.handle().cmd_pipeline_barrier(
                *command_buffer.handle(),
                PipelineStageFlags::COLOR_ATTACHMENT_OUTPUT,
                PipelineStageFlags::BOTTOM_OF_PIPE,
                DependencyFlags::empty(),
                &[],
                &[],
                &[*image_memory_barrier],
            );
        }
    }

    /// Binds the pipeline
    pub(crate) fn bind(&self, command_buffer: &CommandBuffer) {
        unsafe {
            self.device.handle().cmd_bind_pipeline(
                *command_buffer.handle(),
                PipelineBindPoint::GRAPHICS,
                self.graphics_pipeline,
            )
        }
    }
}

impl Drop for Pipeline {
    fn drop(&mut self) {
        unsafe {
            self.device
                .handle()
                .destroy_pipeline(self.graphics_pipeline, None);

            self.device
                .handle()
                .destroy_pipeline_layout(self.graphics_pipeline_layout, None);
        }
    }
}
