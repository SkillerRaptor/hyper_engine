//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use ash::vk;

use crate::{
    bindings_offset::BindingsOffset,
    resource::Resource,
    vulkan::{
        buffer::Buffer,
        graphics_device::GraphicsDevice,
        graphics_pipeline::GraphicsPipeline,
        texture::Texture,
    },
};

pub(crate) struct CommandDecoder<'a> {
    command_buffer: vk::CommandBuffer,
    graphics_device: &'a GraphicsDevice,
}

impl<'a> CommandDecoder<'a> {
    pub(crate) fn new(
        graphics_device: &'a GraphicsDevice,
        command_buffer: vk::CommandBuffer,
    ) -> Self {
        Self {
            command_buffer,
            graphics_device,
        }
    }
}

impl<'a> crate::command_decoder::CommandDecoder for CommandDecoder<'a> {
    fn begin_render_pass(&self, texture: &dyn crate::texture::Texture) {
        let texture = texture.downcast_ref::<Texture>().unwrap();

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
            self.graphics_device
                .device()
                .cmd_pipeline_barrier2(self.command_buffer, &dependency_info);
        }
    }

    fn end_render_pass(&self, texture: &dyn crate::texture::Texture) {
        let texture = texture.downcast_ref::<Texture>().unwrap();

        unsafe {
            self.graphics_device
                .device()
                .cmd_end_rendering(self.command_buffer);
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
            .image(texture.image())
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
                .cmd_pipeline_barrier2(self.command_buffer, &dependency_info);
        }
    }

    fn bind_bindings(&self, buffer: &dyn crate::buffer::Buffer) {
        let buffer = buffer.downcast_ref::<Buffer>().unwrap();

        let bindings_offset = BindingsOffset::new(buffer.resource_handle());

        unsafe {
            self.graphics_device.device().cmd_push_constants(
                self.command_buffer,
                self.graphics_device.pipeline_layout(),
                vk::ShaderStageFlags::ALL,
                0,
                bytemuck::cast_slice(&[bindings_offset]),
            );
        }
    }

    fn bind_pipeline(
        &self,
        graphics_pipeline: &dyn crate::graphics_pipeline::GraphicsPipeline,
        texture: &dyn crate::texture::Texture,
    ) {
        let graphics_pipeline = graphics_pipeline
            .downcast_ref::<GraphicsPipeline>()
            .unwrap();
        let texture = texture.downcast_ref::<Texture>().unwrap();

        let render_area_extent = vk::Extent2D {
            width: texture.width(),
            height: texture.height(),
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
            .image_view(texture.view())
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
            self.graphics_device
                .device()
                .cmd_begin_rendering(self.command_buffer, &rendering_info);
        }

        let viewport = vk::Viewport::default()
            .x(0.0)
            .y(render_area_extent.height as f32)
            .width(render_area_extent.width as f32)
            .height(-(render_area_extent.height as f32))
            .min_depth(0.0)
            .max_depth(1.0);

        unsafe {
            self.graphics_device
                .device()
                .cmd_set_viewport(self.command_buffer, 0, &[viewport]);
        }

        let offset = vk::Offset2D::default().x(0).y(0);

        let scissor = vk::Rect2D::default()
            .offset(offset)
            .extent(render_area_extent);

        unsafe {
            self.graphics_device
                .device()
                .cmd_set_scissor(self.command_buffer, 0, &[scissor]);

            self.graphics_device.device().cmd_bind_descriptor_sets(
                self.command_buffer,
                vk::PipelineBindPoint::GRAPHICS,
                self.graphics_device.pipeline_layout(),
                0,
                self.graphics_device.descriptor_sets(),
                &[],
            );

            self.graphics_device.device().cmd_bind_pipeline(
                self.command_buffer,
                vk::PipelineBindPoint::GRAPHICS,
                graphics_pipeline.pipeline(),
            );
        }
    }

    fn bind_index_buffer(&self, buffer: &dyn crate::buffer::Buffer) {
        let buffer = buffer.downcast_ref::<Buffer>().unwrap();

        unsafe {
            self.graphics_device.device().cmd_bind_index_buffer(
                self.command_buffer,
                buffer.raw(),
                0,
                vk::IndexType::UINT32,
            );
        }
    }

    fn draw(&self, vertex_count: u32, instance_count: u32, first_vertex: u32, first_instance: u32) {
        unsafe {
            self.graphics_device.device().cmd_draw(
                self.command_buffer,
                vertex_count,
                instance_count,
                first_vertex,
                first_instance,
            );
        }
    }

    fn draw_indexed(
        &self,
        index_count: u32,
        instance_count: u32,
        first_index: u32,
        vertex_offset: i32,
        first_instance: u32,
    ) {
        unsafe {
            self.graphics_device.device().cmd_draw_indexed(
                self.command_buffer,
                index_count,
                instance_count,
                first_index,
                vertex_offset,
                first_instance,
            );
        }
    }
}
