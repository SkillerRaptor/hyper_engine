/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::vulkan::core::device::Device;

use ash::vk;
use color_eyre::Result;
use std::rc::Rc;

pub(crate) struct CommandPool {
    handle: vk::CommandPool,

    device: Rc<Device>,
}

impl CommandPool {
    pub(crate) fn new(device: Rc<Device>) -> Result<Self> {
        let queue_families = device.queue_families();

        let create_info = vk::CommandPoolCreateInfo::builder()
            .queue_family_index(queue_families.graphics_family())
            .flags(vk::CommandPoolCreateFlags::RESET_COMMAND_BUFFER);

        let handle = device.create_command_pool(*create_info)?;
        Ok(Self { handle, device })
    }

    pub(crate) fn reset(&self) -> Result<()> {
        self.device
            .reset_command_pool(self, vk::CommandPoolResetFlags::empty())?;
        Ok(())
    }

    pub(crate) fn handle(&self) -> vk::CommandPool {
        self.handle
    }
}

impl Drop for CommandPool {
    fn drop(&mut self) {
        self.device.destroy_command_pool(self.handle);
    }
}
