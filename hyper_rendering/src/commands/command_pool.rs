/*
 * Copyright (c) 2022, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use ash::{
    vk::{self, CommandPoolCreateFlags},
    Device,
};
use log::debug;

pub(crate) struct CreateInfo<'a> {
    pub logical_device: &'a Device,
    pub graphics_queue_index: &'a u32,
}

pub(crate) struct CommandPool {
    command_pool: vk::CommandPool,

    logical_device: Device,
}

impl CommandPool {
    pub fn new(create_info: &CreateInfo) -> Self {
        let command_pool_create_info = vk::CommandPoolCreateInfo::builder()
            .flags(CommandPoolCreateFlags::RESET_COMMAND_BUFFER)
            .queue_family_index(*create_info.graphics_queue_index);

        // TODO: Propagate error
        let command_pool = unsafe {
            create_info
                .logical_device
                .create_command_pool(&command_pool_create_info, None)
                .expect("FIXME")
        };

        debug!("Created command pool");

        Self {
            command_pool,
            logical_device: create_info.logical_device.clone(),
        }
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
