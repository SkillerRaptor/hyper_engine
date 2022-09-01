/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{commands::command_pool::CommandPool, devices::device::Device};

use ash::vk::{
    self, Buffer, BufferMemoryBarrier, CommandBufferAllocateInfo, CommandBufferBeginInfo,
    CommandBufferInheritanceInfo, CommandBufferLevel, CommandBufferResetFlags,
    CommandBufferUsageFlags, DependencyFlags, DeviceSize, ImageMemoryBarrier, MemoryBarrier,
    Pipeline, PipelineBindPoint, PipelineLayout, PipelineStageFlags, Rect2D, RenderingInfo,
    ShaderStageFlags, Viewport,
};
use log::debug;
use std::{mem, slice};
use tracing::instrument;

pub(crate) struct CommandBuffer {
    command_buffer: vk::CommandBuffer,
}

impl CommandBuffer {
    #[instrument(skip_all)]
    pub fn new(device: &Device, command_pool: &CommandPool, level: CommandBufferLevel) -> Self {
        let allocate_info = CommandBufferAllocateInfo::builder()
            .command_pool(*command_pool.command_pool())
            .level(level)
            .command_buffer_count(1);

        let command_buffer = unsafe {
            device
                .logical_device()
                .allocate_command_buffers(&allocate_info)
                .expect("Failed to allocate command buffer")[0]
        };

        debug!("Created command buffer");

        Self { command_buffer }
    }

    pub fn command_buffer(&self) -> &vk::CommandBuffer {
        &self.command_buffer
    }

    #[instrument(skip_all)]
    pub fn begin(
        &self,
        device: &Device,
        usage_flags: CommandBufferUsageFlags,
        inheritance_info: &CommandBufferInheritanceInfo,
    ) {
        let begin_info = CommandBufferBeginInfo::builder()
            .flags(usage_flags)
            .inheritance_info(inheritance_info);

        unsafe {
            device
                .logical_device()
                .begin_command_buffer(self.command_buffer, &begin_info)
                .expect("Failed to begin command buffer");
        }
    }

    #[instrument(skip_all)]
    pub fn end(&self, device: &Device) {
        unsafe {
            device
                .logical_device()
                .end_command_buffer(self.command_buffer)
                .expect("Failed to end command buffer");
        }
    }

    #[instrument(skip_all)]
    pub fn reset(&self, device: &Device, reset_flags: CommandBufferResetFlags) {
        unsafe {
            device
                .logical_device()
                .reset_command_buffer(self.command_buffer, reset_flags)
                .expect("Failed to reset command buffer");
        }
    }

    #[instrument(skip_all)]
    pub fn begin_rendering(&self, device: &Device, rendering_info: &RenderingInfo) {
        unsafe {
            device
                .logical_device()
                .cmd_begin_rendering(self.command_buffer, rendering_info);
        }
    }

    #[instrument(skip_all)]
    pub fn end_rendering(&self, device: &Device) {
        unsafe {
            device
                .logical_device()
                .cmd_end_rendering(self.command_buffer);
        }
    }

    #[allow(clippy::too_many_arguments)]
    #[instrument(skip_all)]
    pub fn pipeline_barrier(
        &self,
        device: &Device,
        src_stage_mask: PipelineStageFlags,
        dst_stage_mask: PipelineStageFlags,
        dependency_flags: DependencyFlags,
        memory_barriers: &[MemoryBarrier],
        buffer_memory_barriers: &[BufferMemoryBarrier],
        image_memory_barriers: &[ImageMemoryBarrier],
    ) {
        unsafe {
            device.logical_device().cmd_pipeline_barrier(
                self.command_buffer,
                src_stage_mask,
                dst_stage_mask,
                dependency_flags,
                memory_barriers,
                buffer_memory_barriers,
                image_memory_barriers,
            );
        }
    }

    #[instrument(skip_all)]
    pub fn bind_pipeline(
        &self,
        device: &Device,
        pipeline_bind_point: PipelineBindPoint,
        pipeline: Pipeline,
    ) {
        unsafe {
            device.logical_device().cmd_bind_pipeline(
                self.command_buffer,
                pipeline_bind_point,
                pipeline,
            );
        }
    }

    #[instrument(skip_all)]
    pub fn bind_vertex_buffers(
        &self,
        device: &Device,
        first_binding: u32,
        buffers: &[Buffer],
        offsets: &[DeviceSize],
    ) {
        unsafe {
            device.logical_device().cmd_bind_vertex_buffers(
                self.command_buffer,
                first_binding,
                buffers,
                offsets,
            );
        }
    }

    #[instrument(skip_all)]
    pub fn push_constants<T>(
        &self,
        device: &Device,
        pipeline_layout: PipelineLayout,
        stage_flags: ShaderStageFlags,
        offset: u32,
        constants: &T,
    ) {
        unsafe {
            device.logical_device().cmd_push_constants(
                self.command_buffer,
                pipeline_layout,
                stage_flags,
                offset,
                slice::from_raw_parts(constants as *const T as *const u8, mem::size_of::<T>()),
            );
        }
    }

    #[instrument(skip_all)]
    pub fn set_scissor(&self, device: &Device, first_scissor: u32, scissors: &[Rect2D]) {
        unsafe {
            device
                .logical_device()
                .cmd_set_scissor(self.command_buffer, first_scissor, scissors);
        }
    }

    #[instrument(skip_all)]
    pub fn set_viewport(&self, device: &Device, first_viewport: u32, viewports: &[Viewport]) {
        unsafe {
            device.logical_device().cmd_set_viewport(
                self.command_buffer,
                first_viewport,
                viewports,
            );
        }
    }

    #[instrument(skip_all)]
    pub fn draw(
        &self,
        device: &Device,
        vertex_count: u32,
        instance_count: u32,
        first_vertex: u32,
        first_instance: u32,
    ) {
        unsafe {
            device.logical_device().cmd_draw(
                self.command_buffer,
                vertex_count,
                instance_count,
                first_vertex,
                first_instance,
            );
        }
    }
}
