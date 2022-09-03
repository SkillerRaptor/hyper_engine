/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use ash::{
    vk::{
        self, Buffer, BufferMemoryBarrier, CommandBufferAllocateInfo, CommandBufferBeginInfo,
        CommandBufferInheritanceInfo, CommandBufferLevel, CommandBufferResetFlags,
        CommandBufferUsageFlags, CommandPool, DependencyFlags, DeviceSize, ImageMemoryBarrier,
        MemoryBarrier, Pipeline, PipelineBindPoint, PipelineLayout, PipelineStageFlags, Rect2D,
        RenderingInfo, ShaderStageFlags, Viewport,
    },
    Device,
};
use log::debug;
use std::{mem, slice};
use tracing::instrument;

pub(crate) struct CommandBufferCreateInfo<'a> {
    pub logical_device: &'a Device,
    pub command_pool: &'a CommandPool,
    pub level: CommandBufferLevel,
}

pub(crate) struct CommandBuffer {
    command_buffer: vk::CommandBuffer,

    logical_device: Device,
}

impl CommandBuffer {
    #[instrument(skip_all)]
    pub fn new(create_info: &CommandBufferCreateInfo) -> Self {
        let allocate_info = CommandBufferAllocateInfo::builder()
            .command_pool(*create_info.command_pool)
            .level(create_info.level)
            .command_buffer_count(1);

        let command_buffer = unsafe {
            create_info
                .logical_device
                .allocate_command_buffers(&allocate_info)
                .expect("Failed to allocate command buffer")[0]
        };

        debug!("Created command buffer");

        Self {
            command_buffer,
            logical_device: create_info.logical_device.clone(),
        }
    }

    pub fn command_buffer(&self) -> &vk::CommandBuffer {
        &self.command_buffer
    }

    #[instrument(skip_all)]
    pub fn begin(
        &self,
        usage_flags: CommandBufferUsageFlags,
        inheritance_info: &CommandBufferInheritanceInfo,
    ) {
        let begin_info = CommandBufferBeginInfo::builder()
            .flags(usage_flags)
            .inheritance_info(inheritance_info);

        unsafe {
            self.logical_device
                .begin_command_buffer(self.command_buffer, &begin_info)
                .expect("Failed to begin command buffer");
        }
    }

    #[instrument(skip_all)]
    pub fn end(&self) {
        unsafe {
            self.logical_device
                .end_command_buffer(self.command_buffer)
                .expect("Failed to end command buffer");
        }
    }

    #[instrument(skip_all)]
    pub fn reset(&self, reset_flags: CommandBufferResetFlags) {
        unsafe {
            self.logical_device
                .reset_command_buffer(self.command_buffer, reset_flags)
                .expect("Failed to reset command buffer");
        }
    }

    #[instrument(skip_all)]
    pub fn begin_rendering(&self, rendering_info: &RenderingInfo) {
        unsafe {
            self.logical_device
                .cmd_begin_rendering(self.command_buffer, rendering_info);
        }
    }

    #[instrument(skip_all)]
    pub fn end_rendering(&self) {
        unsafe {
            self.logical_device.cmd_end_rendering(self.command_buffer);
        }
    }

    #[allow(clippy::too_many_arguments)]
    #[instrument(skip_all)]
    pub fn pipeline_barrier(
        &self,
        src_stage_mask: PipelineStageFlags,
        dst_stage_mask: PipelineStageFlags,
        dependency_flags: DependencyFlags,
        memory_barriers: &[MemoryBarrier],
        buffer_memory_barriers: &[BufferMemoryBarrier],
        image_memory_barriers: &[ImageMemoryBarrier],
    ) {
        unsafe {
            self.logical_device.cmd_pipeline_barrier(
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
    pub fn bind_pipeline(&self, pipeline_bind_point: PipelineBindPoint, pipeline: Pipeline) {
        unsafe {
            self.logical_device.cmd_bind_pipeline(
                self.command_buffer,
                pipeline_bind_point,
                pipeline,
            );
        }
    }

    #[instrument(skip_all)]
    pub fn bind_vertex_buffers(
        &self,
        first_binding: u32,
        buffers: &[Buffer],
        offsets: &[DeviceSize],
    ) {
        unsafe {
            self.logical_device.cmd_bind_vertex_buffers(
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
        pipeline_layout: PipelineLayout,
        stage_flags: ShaderStageFlags,
        offset: u32,
        constants: &T,
    ) {
        unsafe {
            self.logical_device.cmd_push_constants(
                self.command_buffer,
                pipeline_layout,
                stage_flags,
                offset,
                slice::from_raw_parts(constants as *const T as *const u8, mem::size_of::<T>()),
            );
        }
    }

    #[instrument(skip_all)]
    pub fn set_scissor(&self, first_scissor: u32, scissors: &[Rect2D]) {
        unsafe {
            self.logical_device
                .cmd_set_scissor(self.command_buffer, first_scissor, scissors);
        }
    }

    #[instrument(skip_all)]
    pub fn set_viewport(&self, first_viewport: u32, viewports: &[Viewport]) {
        unsafe {
            self.logical_device
                .cmd_set_viewport(self.command_buffer, first_viewport, viewports);
        }
    }

    #[instrument(skip_all)]
    pub fn draw(
        &self,
        vertex_count: u32,
        instance_count: u32,
        first_vertex: u32,
        first_instance: u32,
    ) {
        unsafe {
            self.logical_device.cmd_draw(
                self.command_buffer,
                vertex_count,
                instance_count,
                first_vertex,
                first_instance,
            );
        }
    }
}
