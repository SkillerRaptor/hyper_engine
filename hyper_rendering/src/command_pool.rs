/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    device::{
        queue_family_indices::{self, QueueFamilyIndices},
        Device,
    },
    instance::Instance,
    surface::Surface,
};

use ash::vk::{self, CommandPoolCreateFlags, CommandPoolCreateInfo};
use std::sync::Arc;
use thiserror::Error;

/// An enum representing the errors that can occur while constructing a command pool
#[derive(Debug, Error)]
pub enum CreationError {
    /// Creation of a vulkan object failed
    #[error("creation of vulkan {1} failed")]
    Creation(#[source] vk::Result, &'static str),

    /// Creation of a queue family indices failed
    #[error("creation of queue family indices failed")]
    QueueFamilyIndicesFailure(#[from] queue_family_indices::CreationError),
}

/// A struct representing a wrapper for the vulkan command pool
pub(crate) struct CommandPool {
    /// Vulkan command pool handle
    handle: vk::CommandPool,

    /// Device Wrapper
    device: Arc<Device>,
}

impl CommandPool {
    pub(crate) fn new(
        instance: &Instance,
        surface: &Surface,
        device: &Arc<Device>,
    ) -> Result<Self, CreationError> {
        let queue_family_indices =
            QueueFamilyIndices::new(instance, surface, device.physical_device())?;

        let command_pool_create_info = CommandPoolCreateInfo::builder()
            .queue_family_index(queue_family_indices.graphics_family().unwrap())
            .flags(CommandPoolCreateFlags::RESET_COMMAND_BUFFER);

        let command_pool = unsafe {
            device
                .handle()
                .create_command_pool(&command_pool_create_info, None)
                .map_err(|error| CreationError::Creation(error, "command pool"))
        }?;

        Ok(Self {
            handle: command_pool,
            device: device.clone(),
        })
    }

    /// Returns the vulkan instance command pool
    pub(crate) fn handle(&self) -> &vk::CommandPool {
        &self.handle
    }
}

impl Drop for CommandPool {
    fn drop(&mut self) {
        unsafe {
            self.device.handle().destroy_command_pool(self.handle, None);
        }
    }
}
