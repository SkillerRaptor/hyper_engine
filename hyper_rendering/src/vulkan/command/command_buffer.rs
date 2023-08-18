/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::vulkan::{
    command::command_pool::CommandPool,
    core::device::Device,
    pipeline::{pipeline_layout::PipelineLayout, Pipeline},
    resource::buffer::Buffer,
};

use ash::vk;
use color_eyre::Result;
use std::{mem, rc::Rc, slice};

pub(crate) struct CommandBuffer {
    handle: vk::CommandBuffer,

    device: Rc<Device>,
}

impl CommandBuffer {
    pub(crate) fn new(device: Rc<Device>, command_pool: &CommandPool) -> Result<Self> {
        let allocate_info = vk::CommandBufferAllocateInfo::builder()
            .command_pool(command_pool.handle())
            .command_buffer_count(1)
            .level(vk::CommandBufferLevel::PRIMARY);

        let handle = unsafe { device.handle().allocate_command_buffers(&allocate_info) }?[0];

        Ok(Self { handle, device })
    }

    pub(crate) fn begin(&self, usage_flags: vk::CommandBufferUsageFlags) -> Result<()> {
        let command_buffer_begin_info = vk::CommandBufferBeginInfo::builder().flags(usage_flags);

        unsafe {
            self.device
                .handle()
                .begin_command_buffer(self.handle, &command_buffer_begin_info)?;
        }

        Ok(())
    }

    pub(crate) fn end(&self) -> Result<()> {
        unsafe {
            self.device.handle().end_command_buffer(self.handle)?;
        }

        Ok(())
    }

    pub(crate) fn reset(&self) -> Result<()> {
        unsafe {
            self.device
                .handle()
                .reset_command_buffer(self.handle, vk::CommandBufferResetFlags::from_raw(0))?;
        }

        Ok(())
    }

    pub(crate) fn begin_rendering(&self, rendering_info: vk::RenderingInfo) {
        unsafe {
            self.device
                .handle()
                .cmd_begin_rendering(self.handle, &rendering_info);
        }
    }

    pub(crate) fn end_rendering(&self) {
        unsafe {
            self.device.handle().cmd_end_rendering(self.handle);
        }
    }

    pub(crate) fn bind_pipeline(
        &self,
        pipeline_bind_point: vk::PipelineBindPoint,
        pipeline: &dyn Pipeline,
    ) {
        unsafe {
            self.device.handle().cmd_bind_pipeline(
                self.handle,
                pipeline_bind_point,
                pipeline.handle(),
            );
        }
    }

    pub(crate) fn pipeline_barrier2(&self, dependency_info: vk::DependencyInfo) {
        unsafe {
            self.device
                .handle()
                .cmd_pipeline_barrier2(self.handle, &dependency_info);
        }
    }

    pub(crate) fn set_viewport(&self, first_viewport: u32, viewports: &[vk::Viewport]) {
        unsafe {
            self.device
                .handle()
                .cmd_set_viewport(self.handle, first_viewport, viewports);
        }
    }

    pub(crate) fn set_scissor(&self, first_scissor: u32, scissors: &[vk::Rect2D]) {
        unsafe {
            self.device
                .handle()
                .cmd_set_scissor(self.handle, first_scissor, scissors);
        }
    }

    pub(crate) fn bind_descriptor_sets(
        &self,
        pipeline_bind_point: vk::PipelineBindPoint,
        layout: &PipelineLayout,
        first_set: u32,
        descriptor_sets: &[vk::DescriptorSet],
        dynamic_offsets: &[u32],
    ) {
        unsafe {
            self.device.handle().cmd_bind_descriptor_sets(
                self.handle,
                pipeline_bind_point,
                layout.handle(),
                first_set,
                descriptor_sets,
                dynamic_offsets,
            );
        }
    }

    pub(crate) fn push_constants<T>(
        &self,
        layout: &PipelineLayout,
        stage_flags: vk::ShaderStageFlags,
        offset: u32,
        constants: &T,
    ) {
        unsafe {
            self.device.handle().cmd_push_constants(
                self.handle,
                layout.handle(),
                stage_flags,
                offset,
                slice::from_raw_parts(constants as *const T as *const u8, mem::size_of::<T>()),
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
        unsafe {
            self.device.handle().cmd_draw(
                self.handle,
                vertex_count,
                instance_count,
                first_vertex,
                first_instance,
            );
        }
    }

    pub(crate) fn draw_indexed(
        &self,
        index_count: u32,
        instance_count: u32,
        first_index: u32,
        vertex_offset: i32,
        first_instance: u32,
    ) {
        unsafe {
            self.device.handle().cmd_draw_indexed(
                self.handle,
                index_count,
                instance_count,
                first_index,
                vertex_offset,
                first_instance,
            );
        }
    }

    pub(crate) fn bind_index_buffer(&self, buffer: &Buffer) {
        unsafe {
            self.device.handle().cmd_bind_index_buffer(
                self.handle,
                buffer.handle(),
                0,
                vk::IndexType::UINT32,
            );
        }
    }

    pub(crate) fn copy_buffer(
        &self,
        source_buffer: &Buffer,
        destination_buffer: &Buffer,
        regions: &[vk::BufferCopy],
    ) {
        unsafe {
            self.device.handle().cmd_copy_buffer(
                self.handle(),
                source_buffer.handle(),
                destination_buffer.handle(),
                regions,
            )
        }
    }

    pub(crate) fn copy_buffer_to_image(
        &self,
        source_buffer: &Buffer,
        destination_image: vk::Image,
        destination_image_layout: vk::ImageLayout,
        regions: &[vk::BufferImageCopy],
    ) {
        unsafe {
            self.device.handle().cmd_copy_buffer_to_image(
                self.handle,
                source_buffer.handle(),
                destination_image,
                destination_image_layout,
                regions,
            )
        }
    }

    pub(crate) fn handle(&self) -> vk::CommandBuffer {
        self.handle
    }
}
