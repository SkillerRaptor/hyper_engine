/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use super::super::devices::device::Device;
use super::super::error::Error;
use super::command_pool::CommandPool;

use ash::vk;
use log::debug;

pub struct CommandBuffer {
    command_buffer: vk::CommandBuffer,
}

impl CommandBuffer {
    pub fn new(
        device: &Device,
        command_pool: &CommandPool,
        level: vk::CommandBufferLevel,
    ) -> Result<Self, Error> {
        let allocate_info = vk::CommandBufferAllocateInfo::builder()
            .command_pool(*command_pool.command_pool())
            .level(level)
            .command_buffer_count(1);

        let command_buffer = unsafe {
            device
                .logical_device()
                .allocate_command_buffers(&allocate_info)?[0]
        };

        debug!("Created command buffer");
        Ok(Self { command_buffer })
    }

    pub fn command_buffer(&self) -> &vk::CommandBuffer {
        &self.command_buffer
    }
}
