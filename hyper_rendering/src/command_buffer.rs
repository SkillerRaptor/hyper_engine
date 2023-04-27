/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    command_pool::CommandPool,
    device::{queue_family_indices, Device},
};

use ash::vk::{self, CommandBufferAllocateInfo, CommandBufferLevel};
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
    _handle: vk::CommandBuffer,
}

impl CommandBuffer {
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
            _handle: command_buffers[0],
        })
    }
}
