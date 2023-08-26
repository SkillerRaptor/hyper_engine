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

        let handle = device.allocate_command_buffers(*allocate_info)?[0];

        Ok(Self { handle, device })
    }

    pub(crate) fn begin(&self, usage_flags: vk::CommandBufferUsageFlags) -> Result<()> {
        let command_buffer_begin_info = vk::CommandBufferBeginInfo::builder().flags(usage_flags);

        self.device
            .begin_command_buffer(self, *command_buffer_begin_info)?;
        Ok(())
    }

    pub(crate) fn end(&self) -> Result<()> {
        self.device.end_command_buffer(self)?;
        Ok(())
    }

    pub(crate) fn reset(&self) -> Result<()> {
        self.device
            .reset_command_buffer(self, vk::CommandBufferResetFlags::from_raw(0))?;
        Ok(())
    }

    pub(crate) fn begin_rendering(&self, rendering_info: vk::RenderingInfo) {
        self.device.cmd_begin_rendering(self, rendering_info);
    }

    pub(crate) fn end_rendering(&self) {
        self.device.cmd_end_rendering(self);
    }

    pub(crate) fn bind_pipeline(
        &self,
        pipeline_bind_point: vk::PipelineBindPoint,
        pipeline: &dyn Pipeline,
    ) {
        self.device
            .cmd_bind_pipeline(self, pipeline_bind_point, pipeline);
    }

    pub(crate) fn pipeline_barrier2(&self, dependency_info: vk::DependencyInfo) {
        self.device.cmd_pipeline_barrier2(self, dependency_info);
    }

    pub(crate) fn set_viewport(&self, first_viewport: u32, viewports: &[vk::Viewport]) {
        self.device
            .cmd_set_viewport(self, first_viewport, viewports);
    }

    pub(crate) fn set_scissor(&self, first_scissor: u32, scissors: &[vk::Rect2D]) {
        self.device.cmd_set_scissor(self, first_scissor, scissors);
    }

    pub(crate) fn bind_descriptor_sets(
        &self,
        pipeline_bind_point: vk::PipelineBindPoint,
        layout: &PipelineLayout,
        first_set: u32,
        descriptor_sets: &[vk::DescriptorSet],
        dynamic_offsets: &[u32],
    ) {
        self.device.cmd_bind_descriptor_sets(
            self,
            pipeline_bind_point,
            layout,
            first_set,
            descriptor_sets,
            dynamic_offsets,
        );
    }

    pub(crate) fn push_constants<T>(
        &self,
        layout: &PipelineLayout,
        stage_flags: vk::ShaderStageFlags,
        offset: u32,
        constants: &T,
    ) {
        self.device
            .cmd_push_constants(self, layout.handle(), stage_flags, offset, unsafe {
                slice::from_raw_parts(constants as *const T as *const u8, mem::size_of::<T>())
            });
    }

    pub(crate) fn draw(
        &self,
        vertex_count: u32,
        instance_count: u32,
        first_vertex: u32,
        first_instance: u32,
    ) {
        self.device.cmd_draw(
            self,
            vertex_count,
            instance_count,
            first_vertex,
            first_instance,
        );
    }

    pub(crate) fn draw_indexed(
        &self,
        index_count: u32,
        instance_count: u32,
        first_index: u32,
        vertex_offset: i32,
        first_instance: u32,
    ) {
        self.device.cmd_draw_indexed(
            self,
            index_count,
            instance_count,
            first_index,
            vertex_offset,
            first_instance,
        );
    }

    pub(crate) fn bind_index_buffer(&self, buffer: &Buffer) {
        self.device
            .cmd_bind_index_buffer(self, buffer, 0, vk::IndexType::UINT32);
    }

    pub(crate) fn copy_buffer(
        &self,
        source_buffer: &Buffer,
        destination_buffer: &Buffer,
        regions: &[vk::BufferCopy],
    ) {
        self.device
            .cmd_copy_buffer(self, source_buffer, destination_buffer, regions)
    }

    pub(crate) fn copy_buffer_to_image(
        &self,
        source_buffer: &Buffer,
        destination_image: vk::Image,
        destination_image_layout: vk::ImageLayout,
        regions: &[vk::BufferImageCopy],
    ) {
        self.device.cmd_copy_buffer_to_image(
            self,
            source_buffer,
            destination_image,
            destination_image_layout,
            regions,
        )
    }

    pub(crate) fn handle(&self) -> vk::CommandBuffer {
        self.handle
    }
}
