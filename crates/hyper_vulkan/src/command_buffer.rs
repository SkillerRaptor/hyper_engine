/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{command_pool::CommandPool, device::DeviceShared};

use ash::vk;
use color_eyre::eyre::Result;

pub struct CommandBuffer {
    raw: vk::CommandBuffer,
}

impl CommandBuffer {
    pub(crate) fn new(device: &DeviceShared, command_pool: &CommandPool) -> Result<Self> {
        let allocate_info = vk::CommandBufferAllocateInfo::builder()
            .command_pool(command_pool.raw())
            .command_buffer_count(1)
            .level(vk::CommandBufferLevel::PRIMARY);

        let raw = unsafe { device.raw().allocate_command_buffers(&allocate_info) }?[0];
        Ok(Self { raw })
    }
}
