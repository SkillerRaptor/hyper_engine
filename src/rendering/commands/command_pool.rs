/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use super::super::error::Error;

use ash::vk;
use log::debug;

pub struct CommandPool {
    command_pool: vk::CommandPool,

    logical_device: ash::Device,
}

impl CommandPool {
    pub fn new(logical_device: &ash::Device, graphics_queue_index: &u32) -> Result<Self, Error> {
        let create_info = vk::CommandPoolCreateInfo::builder()
            .flags(vk::CommandPoolCreateFlags::RESET_COMMAND_BUFFER)
            .queue_family_index(*graphics_queue_index);

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
