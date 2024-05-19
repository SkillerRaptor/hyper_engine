/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use ash::vk;

use crate::{
    render_pass::RenderPassDescriptor,
    render_pipeline::RenderPipeline,
    vulkan::{GraphicsDevice, Texture},
};

pub(crate) struct RenderPass {
    texture: Texture,

    graphics_device: GraphicsDevice,
}

impl RenderPass {
    pub(crate) fn new(graphics_device: &GraphicsDevice, descriptor: &RenderPassDescriptor) -> Self {
        let texture = descriptor.texture.vulkan_texture().unwrap();

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
            .image(texture.image())
            .subresource_range(subresource_range);

        let image_memory_barriers = [image_memory_barrier];
        let dependency_info = vk::DependencyInfo::default()
            .dependency_flags(vk::DependencyFlags::empty())
            .memory_barriers(&[])
            .buffer_memory_barriers(&[])
            .image_memory_barriers(&image_memory_barriers);

        unsafe {
            graphics_device.device().cmd_pipeline_barrier2(
                graphics_device.current_frame().command_buffer,
                &dependency_info,
            );
        }

        Self {
            texture: texture.clone(),

            graphics_device: graphics_device.clone(),
        }
    }

    pub(crate) fn bind_pipeline(&self, pipeline: &RenderPipeline) {
        let pipeline = pipeline.vulkan_render_pipeline().unwrap();

        let render_area_extent = vk::Extent2D {
            width: self.texture.width(),
            height: self.texture.height(),
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
            .image_view(self.texture.view())
            .image_layout(vk::ImageLayout::ATTACHMENT_OPTIMAL)
            .load_op(vk::AttachmentLoadOp::CLEAR)
            .store_op(vk::AttachmentStoreOp::STORE)
            .clear_value(color_clear);

        let color_attachments = [color_attachment_info];

        let rendering_info = vk::RenderingInfo::default()
            .render_area(render_area)
            .layer_count(1)
            .color_attachments(&color_attachments);

        unsafe {
            self.graphics_device.device().cmd_begin_rendering(
                self.graphics_device.current_frame().command_buffer,
                &rendering_info,
            );
        }

        let viewport = vk::Viewport::default()
            .x(0.0)
            .y(render_area_extent.height as f32)
            .width(render_area_extent.width as f32)
            .height(-(render_area_extent.height as f32))
            .min_depth(0.0)
            .max_depth(1.0);

        unsafe {
            self.graphics_device.device().cmd_set_viewport(
                self.graphics_device.current_frame().command_buffer,
                0,
                &[viewport],
            );
        }

        let offset = vk::Offset2D::default().x(0).y(0);

        let scissor = vk::Rect2D::default()
            .offset(offset)
            .extent(render_area_extent);

        unsafe {
            self.graphics_device.device().cmd_set_scissor(
                self.graphics_device.current_frame().command_buffer,
                0,
                &[scissor],
            );

            self.graphics_device.device().cmd_bind_pipeline(
                self.graphics_device.current_frame().command_buffer,
                vk::PipelineBindPoint::GRAPHICS,
                pipeline.pipeline(),
            );
        }
    }

    pub(crate) fn draw(
        &self,
        vertex_count: u32,
        instance_count: u32,
        first_vertex: u32,
        first_instance: u32,
    ) {
        let command_buffer = self.graphics_device.current_frame().command_buffer;
        unsafe {
            self.graphics_device.device().cmd_draw(
                command_buffer,
                vertex_count,
                instance_count,
                first_vertex,
                first_instance,
            );
        }
    }
}

impl Drop for RenderPass {
    fn drop(&mut self) {
        unsafe {
            self.graphics_device
                .device()
                .cmd_end_rendering(self.graphics_device.current_frame().command_buffer);

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
                .image(self.texture.image())
                .subresource_range(subresource_range);

            let image_memory_barriers = [image_memory_barrier];

            let dependency_info = vk::DependencyInfo::default()
                .dependency_flags(vk::DependencyFlags::empty())
                .memory_barriers(&[])
                .buffer_memory_barriers(&[])
                .image_memory_barriers(&image_memory_barriers);

            self.graphics_device.device().cmd_pipeline_barrier2(
                self.graphics_device.current_frame().command_buffer,
                &dependency_info,
            );
        }
    }
}
