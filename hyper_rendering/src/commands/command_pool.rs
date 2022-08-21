/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use super::super::devices::device::Device;
use super::super::error::Error;

use ash::vk;
use log::debug;

pub struct CommandPool {
    command_pool: vk::CommandPool,
}

impl CommandPool {
    pub fn new(device: &Device) -> Result<Self, Error> {
        let command_pool_create_info = vk::CommandPoolCreateInfo::builder()
            .flags(vk::CommandPoolCreateFlags::RESET_COMMAND_BUFFER)
            .queue_family_index(*device.graphics_queue_index());

        let command_pool = unsafe {
            device
                .logical_device()
                .create_command_pool(&command_pool_create_info, None)?
        };

        debug!("Created command pool");
        Ok(Self { command_pool })
    }

    pub fn cleanup(&mut self, device: &Device) {
        unsafe {
            device
                .logical_device()
                .destroy_command_pool(self.command_pool, None);
        }
    }

    pub fn command_pool(&self) -> &vk::CommandPool {
        &self.command_pool
    }
}
