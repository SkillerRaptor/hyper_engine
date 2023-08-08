/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    device::{queue_family_indices::QueueFamilyIndices, Device},
    error::{CreationError, CreationResult, RuntimeError, RuntimeResult},
    instance::Instance,
    surface::Surface,
};

use ash::vk;
use std::rc::Rc;

pub(crate) struct CommandPool {
    handle: vk::CommandPool,
    device: Rc<Device>,
}

impl CommandPool {
    pub(crate) fn new(
        instance: &Instance,
        surface: &Surface,
        device: Rc<Device>,
    ) -> CreationResult<Self> {
        let queue_family_indices =
            QueueFamilyIndices::new(instance, surface, device.physical_device())?;

        let create_info = vk::CommandPoolCreateInfo::builder()
            .queue_family_index(queue_family_indices.graphics_family().unwrap())
            .flags(vk::CommandPoolCreateFlags::RESET_COMMAND_BUFFER);

        let handle = unsafe {
            device
                .handle()
                .create_command_pool(&create_info, None)
                .map_err(|error| CreationError::VulkanCreation(error, "command pool"))
        }?;

        Ok(Self { handle, device })
    }

    pub(crate) fn reset(&self) -> RuntimeResult<()> {
        unsafe {
            self.device
                .handle()
                .reset_command_pool(self.handle, vk::CommandPoolResetFlags::empty())
                .map_err(RuntimeError::CommandPoolReset)?;
        }

        Ok(())
    }

    pub(crate) fn handle(&self) -> vk::CommandPool {
        self.handle
    }
}

impl Drop for CommandPool {
    fn drop(&mut self) {
        unsafe {
            self.device.handle().destroy_command_pool(self.handle, None);
        }
    }
}
