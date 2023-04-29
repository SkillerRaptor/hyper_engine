/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    command_pool::CommandPool,
    device::{queue_family_indices, Device},
};

use ash::vk::{
    self, CommandBufferAllocateInfo, CommandBufferBeginInfo, CommandBufferLevel,
    CommandBufferResetFlags, CommandBufferUsageFlags,
};
use std::sync::Arc;
use thiserror::Error;

#[derive(Debug, Error)]
pub enum CreationError {
    #[error("Allocation of vulkan {1} failed")]
    Allocation(#[source] vk::Result, &'static str),

    #[error("creation of queue family indices failed")]
    QueueFamilyIndicesFailure(#[from] queue_family_indices::CreationError),
}

pub(crate) struct CommandBuffer {
    handle: vk::CommandBuffer,
    device: Arc<Device>,
}

impl CommandBuffer {
    pub(crate) fn new(
        device: Arc<Device>,
        command_pool: &CommandPool,
    ) -> Result<Self, CreationError> {
        let command_buffer_allocate_info = CommandBufferAllocateInfo::builder()
            .command_pool(*command_pool.handle())
            .command_buffer_count(1)
            .level(CommandBufferLevel::PRIMARY);

        let command_buffers = unsafe {
            device
                .handle()
                .allocate_command_buffers(&command_buffer_allocate_info)
                .map_err(|error| CreationError::Allocation(error, "command buffer"))
        }?;

        Ok(Self {
            handle: command_buffers[0],
            device,
        })
    }

    pub(crate) fn reset(&self) {
        // TODO: Propagate error
        unsafe {
            self.device
                .handle()
                .reset_command_buffer(self.handle, CommandBufferResetFlags::from_raw(0))
                .unwrap();
        }
    }

    pub(crate) fn begin(&self, usage_flags: CommandBufferUsageFlags) {
        // TODO: Propagate error
        let command_buffer_begin_info = CommandBufferBeginInfo::builder().flags(usage_flags);

        unsafe {
            self.device
                .handle()
                .begin_command_buffer(self.handle, &command_buffer_begin_info)
                .unwrap();
        }
    }

    pub(crate) fn end(&self) {
        // TODO: Propagate error
        unsafe {
            self.device
                .handle()
                .end_command_buffer(self.handle)
                .unwrap();
        }
    }

    pub(crate) fn handle(&self) -> &vk::CommandBuffer {
        &self.handle
    }
}
