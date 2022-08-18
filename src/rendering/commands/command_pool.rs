/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use super::super::devices::device::QueueFamilyIndices;
use super::super::error::Error;

use ash::extensions::khr::Surface as SurfaceLoader;
use ash::vk;
use log::debug;

pub struct CommandPool {
    command_pool: vk::CommandPool,

    logical_device: ash::Device,
}

impl CommandPool {
    pub fn new(
        instance: &ash::Instance,
        surface_loader: &SurfaceLoader,
        surface: &vk::SurfaceKHR,
        physical_device: &vk::PhysicalDevice,
        logical_device: &ash::Device,
    ) -> Result<Self, Error> {
        let queue_families =
            QueueFamilyIndices::new(&instance, &surface_loader, &surface, &physical_device)?;

        let create_info = vk::CommandPoolCreateInfo::builder()
            .flags(vk::CommandPoolCreateFlags::RESET_COMMAND_BUFFER)
            .queue_family_index(*queue_families.graphics());

        let command_pool = unsafe { logical_device.create_command_pool(&create_info, None)? };

        debug!("Created command pool");
        Ok(Self {
            command_pool,

            logical_device: logical_device.clone(),
        })
    }

    pub fn command_pool(&self) -> &vk::CommandPool {
        &self.command_pool
    }
}

impl Drop for CommandPool {
    fn drop(&mut self) {
        unsafe {
            self.logical_device
                .destroy_command_pool(self.command_pool, None);
        }
    }
}
