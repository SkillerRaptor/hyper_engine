/*
 * Copyright (c) 2022, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use ash::{
    vk::{
        self, Buffer, BufferMemoryBarrier, CommandBufferAllocateInfo, CommandBufferBeginInfo,
        CommandBufferInheritanceInfo, CommandBufferLevel, CommandBufferResetFlags,
        CommandBufferUsageFlags, CommandPool, DependencyFlags, DescriptorSet, DeviceSize,
        ImageMemoryBarrier, MemoryBarrier, Pipeline, PipelineBindPoint, PipelineLayout,
        PipelineStageFlags, Rect2D, RenderingInfo, ShaderStageFlags, Viewport,
    },
    Device,
};
use log::debug;
use std::{mem, slice};

pub(crate) struct CreateInfo<'a> {
    pub logical_device: &'a Device,
    pub command_pool: &'a CommandPool,
    pub level: CommandBufferLevel,
}

pub(crate) struct CommandBuffer {
    command_buffer: vk::CommandBuffer,

    logical_device: Device,
}

impl CommandBuffer {
    pub fn new(create_info: &CreateInfo) -> Self {
        let allocate_info = CommandBufferAllocateInfo::builder()
            .command_pool(*create_info.command_pool)
            .level(create_info.level)
            .command_buffer_count(1);

        // TODO: Propagate error
        let command_buffer = unsafe {
            create_info
                .logical_device
                .allocate_command_buffers(&allocate_info)
                .expect("FIXME")[0]
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

    pub fn begin(
        &self,
        usage_flags: CommandBufferUsageFlags,
        inheritance_info: &CommandBufferInheritanceInfo,
    ) {
        let begin_info = CommandBufferBeginInfo::builder()
            .flags(usage_flags)
            .inheritance_info(inheritance_info);

        // TODO: Propagate error
        unsafe {
            self.logical_device
                .begin_command_buffer(self.command_buffer, &begin_info)
                .expect("FIXME");
        }
    }

    pub fn end(&self) {
        // TODO: Propagate error
        unsafe {
            self.logical_device
                .end_command_buffer(self.command_buffer)
                .expect("FIXME");
        }
    }

    pub fn reset(&self, reset_flags: CommandBufferResetFlags) {
        // TODO: Propagate error
        unsafe {
            self.logical_device
                .reset_command_buffer(self.command_buffer, reset_flags)
                .expect("FIXME");
        }
    }

    pub fn begin_rendering(&self, rendering_info: &RenderingInfo) {
        unsafe {
            self.logical_device
                .cmd_begin_rendering(self.command_buffer, rendering_info);
        }
    }

    pub fn end_rendering(&self) {
        unsafe {
            self.logical_device.cmd_end_rendering(self.command_buffer);
        }
    }

    #[allow(clippy::too_many_arguments)]

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

    pub fn bind_descriptor_sets(
        &self,
        pipeline_bind_point: PipelineBindPoint,
        layout: PipelineLayout,
        first_set: u32,
        descriptor_sets: &[DescriptorSet],
        dynamic_offsets: &[u32],
    ) {
        unsafe {
            self.logical_device.cmd_bind_descriptor_sets(
                self.command_buffer,
                pipeline_bind_point,
                layout,
                first_set,
                descriptor_sets,
                dynamic_offsets,
            )
        }
    }

    pub fn bind_pipeline(&self, pipeline_bind_point: PipelineBindPoint, pipeline: Pipeline) {
        unsafe {
            self.logical_device.cmd_bind_pipeline(
                self.command_buffer,
                pipeline_bind_point,
                pipeline,
            );
        }
    }

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

    pub fn set_scissor(&self, first_scissor: u32, scissors: &[Rect2D]) {
        unsafe {
            self.logical_device
                .cmd_set_scissor(self.command_buffer, first_scissor, scissors);
        }
    }

    pub fn set_viewport(&self, first_viewport: u32, viewports: &[Viewport]) {
        unsafe {
            self.logical_device
                .cmd_set_viewport(self.command_buffer, first_viewport, viewports);
        }
    }

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
