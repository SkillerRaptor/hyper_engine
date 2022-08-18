/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use super::super::error::Error;

use ash::vk;
use log::debug;

pub struct Semaphore {
    semaphore: vk::Semaphore,

    logical_device: ash::Device,
}

impl Semaphore {
    pub fn new(logical_device: &ash::Device) -> Result<Self, Error> {
        let semaphore_create_info = vk::SemaphoreCreateInfo::builder();

        let semaphore = unsafe { logical_device.create_semaphore(&semaphore_create_info, None)? };

        debug!("Created semaphore");
        Ok(Self {
            semaphore,

            logical_device: logical_device.clone(),
        })
    }

    pub fn semaphore(&self) -> &vk::Semaphore {
        &self.semaphore
    }
}

impl Drop for Semaphore {
    fn drop(&mut self) {
        unsafe {
            self.logical_device.destroy_semaphore(self.semaphore, None);
        }
    }
}
