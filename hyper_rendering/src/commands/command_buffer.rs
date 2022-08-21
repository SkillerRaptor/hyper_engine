/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use super::super::devices::device::Device;
use super::super::error::Error;
use super::command_pool::CommandPool;

use ash::vk;
use log::debug;

pub struct CommandBuffer {
    command_buffer: vk::CommandBuffer,
}

impl CommandBuffer {
    pub fn new(
        device: &Device,
        command_pool: &CommandPool,
        level: vk::CommandBufferLevel,
    ) -> Result<Self, Error> {
        let allocate_info = vk::CommandBufferAllocateInfo::builder()
            .command_pool(*command_pool.command_pool())
            .level(level)
            .command_buffer_count(1);

        let command_buffer = unsafe {
            device
                .logical_device()
                .allocate_command_buffers(&allocate_info)?[0]
        };

        debug!("Created command buffer");
        Ok(Self { command_buffer })
    }

    pub fn command_buffer(&self) -> &vk::CommandBuffer {
        &self.command_buffer
    }

    pub fn begin(
        &self,
        device: &Device,
        usage_flags: vk::CommandBufferUsageFlags,
        inheritance_info: &vk::CommandBufferInheritanceInfo,
    ) -> Result<(), Error> {
        let begin_info = vk::CommandBufferBeginInfo::builder()
            .flags(usage_flags)
            .inheritance_info(&inheritance_info);

        unsafe {
            device
                .logical_device()
                .begin_command_buffer(self.command_buffer, &begin_info)?;
        }

        Ok(())
    }

    pub fn end(&self, device: &Device) -> Result<(), Error> {
        unsafe {
            device
                .logical_device()
                .end_command_buffer(self.command_buffer)?;
        }

        Ok(())
    }

    pub fn reset(
        &self,
        device: &Device,
        reset_flags: vk::CommandBufferResetFlags,
    ) -> Result<(), Error> {
        unsafe {
            device
                .logical_device()
                .reset_command_buffer(self.command_buffer, reset_flags)?;
        }

        Ok(())
    }

    pub fn begin_rendering(&self, device: &Device, rendering_info: &vk::RenderingInfo) {
        unsafe {
            device
                .logical_device()
                .cmd_begin_rendering(self.command_buffer, rendering_info);
        }
    }

    pub fn end_rendering(&self, device: &Device) {
        unsafe {
            device
                .logical_device()
                .cmd_end_rendering(self.command_buffer);
        }
    }

    pub fn pipeline_barrier(
        &self,
        device: &Device,
        src_stage_mask: vk::PipelineStageFlags,
        dst_stage_mask: vk::PipelineStageFlags,
        dependency_flags: vk::DependencyFlags,
        memory_barriers: &[vk::MemoryBarrier],
        buffer_memory_barriers: &[vk::BufferMemoryBarrier],
        image_memory_barriers: &[vk::ImageMemoryBarrier],
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

    pub fn bind_pipeline(
        &self,
        device: &Device,
        pipeline_bind_point: vk::PipelineBindPoint,
        pipeline: vk::Pipeline,
    ) {
        unsafe {
            device.logical_device().cmd_bind_pipeline(
                self.command_buffer,
                pipeline_bind_point,
                pipeline,
            );
        }
    }

    pub fn bind_vertex_buffers(
        &self,
        device: &Device,
        first_binding: u32,
        buffers: &[vk::Buffer],
        offsets: &[vk::DeviceSize],
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

    pub fn push_constants<T>(
        &self,
        device: &Device,
        pipeline_layout: vk::PipelineLayout,
        stage_flags: vk::ShaderStageFlags,
        offset: u32,
        constants: &T,
    ) {
        unsafe {
            device.logical_device().cmd_push_constants(
                self.command_buffer,
                pipeline_layout,
                stage_flags,
                offset,
                std::slice::from_raw_parts(
                    constants as *const T as *const u8,
                    std::mem::size_of::<T>(),
                ),
            );
        }
    }

    pub fn set_scissor(&self, device: &Device, first_scissor: u32, scissors: &[vk::Rect2D]) {
        unsafe {
            device
                .logical_device()
                .cmd_set_scissor(self.command_buffer, first_scissor, scissors);
        }
    }

    pub fn set_viewport(&self, device: &Device, first_viewport: u32, viewports: &[vk::Viewport]) {
        unsafe {
            device.logical_device().cmd_set_viewport(
                self.command_buffer,
                first_viewport,
                viewports,
            );
        }
    }

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
