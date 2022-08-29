/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use crate::devices::device::Device;

use ash::vk::{self, CommandPoolCreateFlags, CommandPoolCreateInfo};
use log::debug;

pub struct CommandPool {
    command_pool: vk::CommandPool,
}

impl CommandPool {
    pub fn new(device: &Device) -> Self {
        let command_pool_create_info = CommandPoolCreateInfo::builder()
            .flags(CommandPoolCreateFlags::RESET_COMMAND_BUFFER)
            .queue_family_index(*device.graphics_queue_index());

        let command_pool = unsafe {
            device
                .logical_device()
                .create_command_pool(&command_pool_create_info, None)
                .expect("Failed to create command pool")
        };

        debug!("Created command pool");

        Self { command_pool }
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
