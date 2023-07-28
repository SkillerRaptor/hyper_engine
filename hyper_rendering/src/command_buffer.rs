/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    command_pool::CommandPool,
    device::Device,
    error::{CreationError, RuntimeError},
};

use ash::vk::{
    CommandBuffer as VulkanCommandBuffer, CommandBufferAllocateInfo, CommandBufferBeginInfo,
    CommandBufferLevel, CommandBufferResetFlags, CommandBufferUsageFlags,
};
use std::sync::Arc;

pub(crate) struct CommandBuffer {
    handle: VulkanCommandBuffer,
    device: Arc<Device>,
}

impl CommandBuffer {
    pub(crate) fn new(
        device: Arc<Device>,
        command_pool: &CommandPool,
    ) -> Result<Self, CreationError> {
        let allocate_info = CommandBufferAllocateInfo::builder()
            .command_pool(*command_pool.handle())
            .command_buffer_count(1)
            .level(CommandBufferLevel::PRIMARY);

        let handles = unsafe {
            device
                .handle()
                .allocate_command_buffers(&allocate_info)
                .map_err(|error| CreationError::VulkanAllocation(error, "command buffer"))
        }?;

        Ok(Self {
            handle: handles[0],
            device,
        })
    }

    pub(crate) fn reset(&self) -> Result<(), RuntimeError> {
        unsafe {
            self.device
                .handle()
                .reset_command_buffer(self.handle, CommandBufferResetFlags::from_raw(0))
                .map_err(RuntimeError::CommandBufferReset)
        }?;

        Ok(())
    }

    pub(crate) fn begin(&self, usage_flags: CommandBufferUsageFlags) -> Result<(), RuntimeError> {
        let command_buffer_begin_info = CommandBufferBeginInfo::builder().flags(usage_flags);

        unsafe {
            self.device
                .handle()
                .begin_command_buffer(self.handle, &command_buffer_begin_info)
                .map_err(RuntimeError::CommandBufferBegin)
        }?;

        Ok(())
    }

    pub(crate) fn end(&self) -> Result<(), RuntimeError> {
        unsafe {
            self.device
                .handle()
                .end_command_buffer(self.handle)
                .map_err(RuntimeError::CommandBufferEnd)
        }?;

        Ok(())
    }

    pub(crate) fn handle(&self) -> &VulkanCommandBuffer {
        &self.handle
    }
}
