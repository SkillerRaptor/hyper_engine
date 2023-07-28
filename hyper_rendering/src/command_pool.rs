/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    device::{queue_family_indices::QueueFamilyIndices, Device},
    error::CreationError,
    instance::Instance,
    surface::Surface,
};

use ash::vk::{CommandPool as VulkanCommandPool, CommandPoolCreateFlags, CommandPoolCreateInfo};
use std::sync::Arc;

pub(crate) struct CommandPool {
    handle: VulkanCommandPool,
    device: Arc<Device>,
}

impl CommandPool {
    pub(crate) fn new(
        instance: &Instance,
        surface: &Surface,
        device: Arc<Device>,
    ) -> Result<Self, CreationError> {
        let queue_family_indices =
            QueueFamilyIndices::new(instance, surface, device.physical_device())?;

        let create_info = CommandPoolCreateInfo::builder()
            .queue_family_index(queue_family_indices.graphics_family().unwrap())
            .flags(CommandPoolCreateFlags::RESET_COMMAND_BUFFER);

        let handle = unsafe {
            device
                .handle()
                .create_command_pool(&create_info, None)
                .map_err(|error| CreationError::VulkanCreation(error, "command pool"))
        }?;

        Ok(Self { handle, device })
    }

    pub(crate) fn handle(&self) -> &VulkanCommandPool {
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
