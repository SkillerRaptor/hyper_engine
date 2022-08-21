/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use super::super::devices::device::Device;
use super::super::error::Error;

use ash::vk;
use log::debug;

pub struct Semaphore {
    semaphore: vk::Semaphore,
}

impl Semaphore {
    pub fn new(device: &Device) -> Result<Self, Error> {
        let semaphore_create_info = vk::SemaphoreCreateInfo::builder();

        let semaphore = unsafe {
            device
                .logical_device()
                .create_semaphore(&semaphore_create_info, None)?
        };

        debug!("Created semaphore");
        Ok(Self { semaphore })
    }

    pub fn cleanup(&mut self, device: &Device) {
        unsafe {
            device
                .logical_device()
                .destroy_semaphore(self.semaphore, None);
        }
    }

    pub fn semaphore(&self) -> &vk::Semaphore {
        &self.semaphore
    }
}
