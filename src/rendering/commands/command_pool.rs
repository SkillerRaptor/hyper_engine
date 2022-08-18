/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use super::super::devices::device::{Device, QueueFamilyIndices};
use super::super::devices::instance::Instance;
use super::super::devices::surface::Surface;
use super::super::error::Error;

use ash::vk;
use log::debug;
use std::ops::Deref;
use std::rc::Rc;

pub struct CommandPool {
    command_pool: vk::CommandPool,

    device: Rc<Device>,
}

impl CommandPool {
    pub fn new(
        instance: &Rc<Instance>,
        surface: &Rc<Surface>,
        device: &Rc<Device>,
    ) -> Result<Self, Error> {
        let queue_families =
            QueueFamilyIndices::new(&instance, &surface, &device.physical_device())?;

        let create_info = vk::CommandPoolCreateInfo::builder()
            .flags(vk::CommandPoolCreateFlags::RESET_COMMAND_BUFFER)
            .queue_family_index(*queue_families.graphics());

        let command_pool = unsafe {
            device
                .logical_device()
                .create_command_pool(&create_info, None)?
        };

        debug!("Created command pool");
        Ok(Self {
            command_pool,

            device: device.clone(),
        })
    }

    pub fn command_pool(&self) -> &vk::CommandPool {
        &self.command_pool
    }
}

impl Drop for CommandPool {
    fn drop(&mut self) {
        unsafe {
            self.device
                .logical_device()
                .destroy_command_pool(self.command_pool, None);
        }
    }
}
