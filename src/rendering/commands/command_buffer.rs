/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use super::super::devices::device::Device;
use super::super::error::Error;
use super::command_pool::CommandPool;

use ash::vk;
use log::debug;
use std::rc::Rc;

pub struct CommandBuffer {
    command_buffer: vk::CommandBuffer,

    device: Rc<Device>,
    command_pool: Rc<CommandPool>,
}

impl CommandBuffer {
    pub fn new(
        device: &Rc<Device>,
        command_pool: &Rc<CommandPool>,
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
        Ok(Self {
            command_buffer,

            device: device.clone(),
            command_pool: command_pool.clone(),
        })
    }

    pub fn begin(
        &self,
        usage_flags: vk::CommandBufferUsageFlags,
        inheritance_info: &vk::CommandBufferInheritanceInfo,
    ) -> Result<(), Error> {
        let begin_info = vk::CommandBufferBeginInfo::builder()
            .flags(usage_flags)
            .inheritance_info(&inheritance_info);

        unsafe {
            self.device
                .logical_device()
                .begin_command_buffer(self.command_buffer, &begin_info)?;
        }

        Ok(())
    }

    pub fn end(&self) -> Result<(), Error> {
        unsafe {
            self.device
                .logical_device()
                .end_command_buffer(self.command_buffer)?;
        }

        Ok(())
    }

    pub fn reset(&self, reset_flags: vk::CommandBufferResetFlags) -> Result<(), Error> {
        unsafe {
            self.device
                .logical_device()
                .reset_command_buffer(self.command_buffer, reset_flags)?;
        }

        Ok(())
    }

    pub fn cmd_begin_rendering(&self, rendering_info: &vk::RenderingInfo) {
        unsafe {
            self.device
                .logical_device()
                .cmd_begin_rendering(self.command_buffer, rendering_info);
        }
    }

    pub fn cmd_end_rendering(&self) {
        unsafe {
            self.device
                .logical_device()
                .cmd_end_rendering(self.command_buffer);
        }
    }

    pub fn cmd_pipeline_barrier(
        &self,
        src_stage_mask: vk::PipelineStageFlags,
        dst_stage_mask: vk::PipelineStageFlags,
        dependency_flags: vk::DependencyFlags,
        memory_barriers: &[vk::MemoryBarrier],
        buffer_memory_barriers: &[vk::BufferMemoryBarrier],
        image_memory_barriers: &[vk::ImageMemoryBarrier],
    ) {
        unsafe {
            self.device.logical_device().cmd_pipeline_barrier(
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

    pub fn cmd_bind_pipeline(
        &self,
        pipeline_bind_point: vk::PipelineBindPoint,
        pipeline: vk::Pipeline,
    ) {
        unsafe {
            self.device.logical_device().cmd_bind_pipeline(
                self.command_buffer,
                pipeline_bind_point,
                pipeline,
            );
        }
    }

    pub fn cmd_bind_vertex_buffers(
        &self,
        first_binding: u32,
        buffers: &[vk::Buffer],
        offsets: &[vk::DeviceSize],
    ) {
        unsafe {
            self.device.logical_device().cmd_bind_vertex_buffers(
                self.command_buffer,
                first_binding,
                buffers,
                offsets,
            );
        }
    }

    pub fn cmd_set_scissor(&self, first_scissor: u32, scissors: &[vk::Rect2D]) {
        unsafe {
            self.device.logical_device().cmd_set_scissor(
                self.command_buffer,
                first_scissor,
                scissors,
            );
        }
    }

    pub fn cmd_set_viewport(&self, first_viewport: u32, viewports: &[vk::Viewport]) {
        unsafe {
            self.device.logical_device().cmd_set_viewport(
                self.command_buffer,
                first_viewport,
                viewports,
            );
        }
    }

    pub fn cmd_draw(
        &self,
        vertex_count: u32,
        instance_count: u32,
        first_vertex: u32,
        first_instance: u32,
    ) {
        unsafe {
            self.device.logical_device().cmd_draw(
                self.command_buffer,
                vertex_count,
                instance_count,
                first_vertex,
                first_instance,
            );
        }
    }

    pub fn command_buffer(&self) -> &vk::CommandBuffer {
        &self.command_buffer
    }
}

impl Drop for CommandBuffer {
    fn drop(&mut self) {
        unsafe {
            self.device
                .logical_device()
                .free_command_buffers(*self.command_pool.command_pool(), &[self.command_buffer]);
        }
    }
}
