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

/// An enum representing the errors that can occur while constructing a command buffer
#[derive(Debug, Error)]
pub enum CreationError {
    /// Allocation of a vulkan object failed
    #[error("Allocation of vulkan {1} failed")]
    Allocation(#[source] vk::Result, &'static str),

    /// Creation of a queue family indices failed
    #[error("creation of queue family indices failed")]
    QueueFamilyIndicesFailure(#[from] queue_family_indices::CreationError),
}

/// A struct representing a wrapper for the vulkan command buffer
pub(crate) struct CommandBuffer {
    /// Vulkan command buffer handle
    handle: vk::CommandBuffer,

    /// Device Wrapper
    device: Arc<Device>,
}

impl CommandBuffer {
    /// Constructs a new command pool
    ///
    /// Arguments:
    ///
    /// * `device`: Vulkan device
    /// * `command_pool`: Command pool handle
    pub(crate) fn new(
        device: &Arc<Device>,
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
            device: device.clone(),
        })
    }

    /// Resets the command buffer
    pub(crate) fn reset(&self) {
        // TODO: Propagate error
        unsafe {
            self.device
                .handle()
                .reset_command_buffer(self.handle, CommandBufferResetFlags::from_raw(0))
                .unwrap();
        }
    }

    /// Begins the command buffer recording
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

    /// Ends the command buffer recording
    pub(crate) fn end(&self) {
        // TODO: Propagate error
        unsafe {
            self.device
                .handle()
                .end_command_buffer(self.handle)
                .unwrap();
        }
    }

    /// Returns the vulkan command buffer handle
    pub(crate) fn handle(&self) -> &vk::CommandBuffer {
        &self.handle
    }
}
