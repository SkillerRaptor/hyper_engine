/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use std::sync::Arc;

use ash::vk;
use color_eyre::eyre::Result;

use crate::core::device::DeviceShared;

pub struct CommandPool {
    raw: vk::CommandPool,
    device: Arc<DeviceShared>,
}

impl CommandPool {
    pub(crate) fn new(device: &Arc<DeviceShared>) -> Result<Self> {
        let create_info = vk::CommandPoolCreateInfo::builder()
            .flags(vk::CommandPoolCreateFlags::RESET_COMMAND_BUFFER)
            .queue_family_index(device.queue_family_index());

        let raw = unsafe { device.raw().create_command_pool(&create_info, None) }?;

        Ok(Self {
            raw,
            device: Arc::clone(device),
        })
    }

    pub fn reset(&self) -> Result<()> {
        unsafe {
            self.device
                .raw()
                .reset_command_pool(self.raw, vk::CommandPoolResetFlags::empty())?;
        }

        Ok(())
    }

    pub(crate) fn raw(&self) -> vk::CommandPool {
        self.raw
    }
}

impl Drop for CommandPool {
    fn drop(&mut self) {
        unsafe {
            self.device.raw().destroy_command_pool(self.raw, None);
        }
    }
}
