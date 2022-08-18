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
use std::rc::Rc;

pub struct CommandBuffer {
    command_buffer: vk::CommandBuffer,

    device: Rc<Device>,
    command_pool: Rc<CommandPool>,
}

impl CommandBuffer {
    pub fn new(
        device: &Rc<Device>,
        command_pool: &Rc<CommandPool>,
        level: vk::CommandBufferLevel,
    ) -> Result<Self, Error> {
        let command_buffer_allocate_info = vk::CommandBufferAllocateInfo::builder()
            .command_pool(*command_pool.command_pool())
            .level(level)
            .command_buffer_count(1);

        let command_buffer = unsafe {
            device
                .logical_device()
                .allocate_command_buffers(&command_buffer_allocate_info)?[0]
        };

        debug!("Created command buffer");
        Ok(Self {
            command_buffer,

            device: device.clone(),
            command_pool: command_pool.clone(),
        })
    }

    pub fn command_buffer(&self) -> &vk::CommandBuffer {
        &self.command_buffer
    }
}

impl Drop for CommandBuffer {
    fn drop(&mut self) {
        unsafe {
            self.device
                .logical_device()
                .free_command_buffers(*self.command_pool.command_pool(), &[self.command_buffer]);
        }
    }
}
