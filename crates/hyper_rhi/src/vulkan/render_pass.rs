//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::sync::{Arc, Mutex};

use ash::vk;

use crate::{
    render_pass::{DrawIndexedArguments, RenderPassDescriptor},
    texture::Texture as _,
    vulkan::{
        buffer::Buffer,
        graphics_device::GraphicsDeviceShared,
        graphics_pipeline::GraphicsPipeline,
        pipeline_layout::PipelineLayout,
        texture::Texture,
    },
};

pub(crate) struct RenderPass {
    pipeline: Mutex<Option<Arc<dyn crate::graphics_pipeline::GraphicsPipeline>>>,

    _depth_stencil_attachment: Option<Arc<dyn crate::texture::Texture>>,
    color_attachment: Arc<dyn crate::texture::Texture>,

    graphics_device: Arc<GraphicsDeviceShared>,
}

impl RenderPass {
    pub(crate) fn new(
        graphics_device: &Arc<GraphicsDeviceShared>,
        descriptor: &RenderPassDescriptor,
    ) -> Self {
        let command_buffer = graphics_device.current_frame().command_buffer;

        let color_attachment = descriptor
            .color_attachment
            .downcast_ref::<Texture>()
            .unwrap();

        let subresource_range = vk::ImageSubresourceRange::default()
            .aspect_mask(vk::ImageAspectFlags::COLOR)
            .base_mip_level(0)
            .level_count(1)
            .base_array_layer(0)
            .layer_count(1);

        let image_memory_barrier = vk::ImageMemoryBarrier2::default()
            .src_stage_mask(vk::PipelineStageFlags2::TOP_OF_PIPE)
            .dst_stage_mask(vk::PipelineStageFlags2::COLOR_ATTACHMENT_OUTPUT)
            .dst_access_mask(vk::AccessFlags2::COLOR_ATTACHMENT_WRITE)
            .old_layout(vk::ImageLayout::UNDEFINED)
            .new_layout(vk::ImageLayout::COLOR_ATTACHMENT_OPTIMAL)
            .src_queue_family_index(0)
            .dst_queue_family_index(0)
            .image(color_attachment.image())
            .subresource_range(subresource_range);

        let image_memory_barriers = [image_memory_barrier];
        let dependency_info = vk::DependencyInfo::default()
            .dependency_flags(vk::DependencyFlags::empty())
            .memory_barriers(&[])
            .buffer_memory_barriers(&[])
            .image_memory_barriers(&image_memory_barriers);

        unsafe {
            graphics_device
                .device()
                .cmd_pipeline_barrier2(command_buffer, &dependency_info);
        }

        let render_area_extent = vk::Extent2D {
            width: color_attachment.width(),
            height: color_attachment.height(),
        };
        let render_area_offset = vk::Offset2D::default().x(0).y(0);

        let render_area = vk::Rect2D::default()
            .extent(render_area_extent)
            .offset(render_area_offset);

        let color_clear = vk::ClearValue {
            color: vk::ClearColorValue {
                float32: [0.0, 0.2, 0.4, 1.0],
            },
        };

        let color_attachment_info = vk::RenderingAttachmentInfo::default()
            .image_view(color_attachment.view())
            .image_layout(vk::ImageLayout::ATTACHMENT_OPTIMAL)
            .load_op(vk::AttachmentLoadOp::CLEAR)
            .store_op(vk::AttachmentStoreOp::STORE)
            .clear_value(color_clear);

        let color_attachments = [color_attachment_info];

        let rendering_info = vk::RenderingInfo::default()
            .render_area(render_area)
            .layer_count(1)
            .color_attachments(&color_attachments);

        let viewport = vk::Viewport::default()
            .x(0.0)
            .y(render_area_extent.height as f32)
            .width(render_area_extent.width as f32)
            .height(-(render_area_extent.height as f32))
            .min_depth(0.0)
            .max_depth(1.0);

        let offset = vk::Offset2D::default().x(0).y(0);

        let scissor = vk::Rect2D::default()
            .offset(offset)
            .extent(render_area_extent);

        unsafe {
            graphics_device
                .device()
                .cmd_begin_rendering(command_buffer, &rendering_info);

            graphics_device
                .device()
                .cmd_set_viewport(command_buffer, 0, &[viewport]);

            graphics_device
                .device()
                .cmd_set_scissor(command_buffer, 0, &[scissor]);
        }

        // TODO: Find a better way to do this
        let depth_stencil_attachment = if descriptor.depth_stencil_attachment.is_some() {
            Some(Arc::clone(
                descriptor.depth_stencil_attachment.as_ref().unwrap(),
            ))
        } else {
            None
        };

        Self {
            pipeline: Mutex::new(None),

            _depth_stencil_attachment: depth_stencil_attachment,
            color_attachment: Arc::clone(descriptor.color_attachment),

            graphics_device: Arc::clone(graphics_device),
        }
    }
}

impl crate::render_pass::RenderPass for RenderPass {
    fn set_pipeline(&self, pipeline: &Arc<dyn crate::graphics_pipeline::GraphicsPipeline>) {
        *self.pipeline.lock().unwrap() = Some(Arc::clone(pipeline));

        let pipeline = pipeline.downcast_ref::<GraphicsPipeline>().unwrap();
        let layout = pipeline.layout().downcast_ref::<PipelineLayout>().unwrap();

        let command_buffer = self.graphics_device.current_frame().command_buffer;

        unsafe {
            self.graphics_device.device().cmd_bind_descriptor_sets(
                command_buffer,
                vk::PipelineBindPoint::GRAPHICS,
                layout.layout(),
                0,
                self.graphics_device.descriptor_sets(),
                &[],
            );

            self.graphics_device.device().cmd_bind_pipeline(
                command_buffer,
                vk::PipelineBindPoint::GRAPHICS,
                pipeline.pipeline(),
            );
        }
    }

    fn set_index_buffer(&self, buffer: &Arc<dyn crate::buffer::Buffer>) {
        let buffer = buffer.downcast_ref::<Buffer>().unwrap();

        let command_buffer = self.graphics_device.current_frame().command_buffer;

        unsafe {
            self.graphics_device.device().cmd_bind_index_buffer(
                command_buffer,
                buffer.raw(),
                0,
                vk::IndexType::UINT32,
            );
        }
    }

    fn set_push_constants(&self, data: &[u8]) {
        let pipeline = self.pipeline.lock().unwrap();

        let pipeline = pipeline
            .as_ref()
            .unwrap()
            .downcast_ref::<GraphicsPipeline>()
            .unwrap();
        let layout = pipeline.layout().downcast_ref::<PipelineLayout>().unwrap();

        let command_buffer = self.graphics_device.current_frame().command_buffer;

        unsafe {
            self.graphics_device.device().cmd_push_constants(
                command_buffer,
                layout.layout(),
                vk::ShaderStageFlags::ALL,
                0,
                data,
            );
        }
    }

    fn draw_indexed(&self, arguments: &DrawIndexedArguments) {
        let command_buffer = self.graphics_device.current_frame().command_buffer;

        unsafe {
            self.graphics_device.device().cmd_draw_indexed(
                command_buffer,
                arguments.index_count,
                arguments.instance_count,
                arguments.first_index,
                arguments.vertex_offset,
                arguments.first_instance,
            );
        }
    }
}

impl Drop for RenderPass {
    fn drop(&mut self) {
        let color_attachment = self.color_attachment.downcast_ref::<Texture>().unwrap();

        let command_buffer = self.graphics_device.current_frame().command_buffer;

        unsafe {
            self.graphics_device
                .device()
                .cmd_end_rendering(command_buffer);
        }

        let subresource_range = vk::ImageSubresourceRange::default()
            .aspect_mask(vk::ImageAspectFlags::COLOR)
            .base_mip_level(0)
            .level_count(1)
            .base_array_layer(0)
            .layer_count(1);

        let image_memory_barrier = vk::ImageMemoryBarrier2::default()
            .src_stage_mask(vk::PipelineStageFlags2::COLOR_ATTACHMENT_OUTPUT)
            .src_access_mask(vk::AccessFlags2::COLOR_ATTACHMENT_WRITE)
            .dst_stage_mask(vk::PipelineStageFlags2::BOTTOM_OF_PIPE)
            .old_layout(vk::ImageLayout::COLOR_ATTACHMENT_OPTIMAL)
            .new_layout(vk::ImageLayout::PRESENT_SRC_KHR)
            .src_queue_family_index(0)
            .dst_queue_family_index(0)
            .image(color_attachment.image())
            .subresource_range(subresource_range);

        let image_memory_barriers = [image_memory_barrier];

        let dependency_info = vk::DependencyInfo::default()
            .dependency_flags(vk::DependencyFlags::empty())
            .memory_barriers(&[])
            .buffer_memory_barriers(&[])
            .image_memory_barriers(&image_memory_barriers);

        unsafe {
            self.graphics_device
                .device()
                .cmd_pipeline_barrier2(command_buffer, &dependency_info);
        }
    }
}
