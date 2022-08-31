/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use crate::devices::device::Device;

use ash::vk::{self, SemaphoreCreateInfo};
use log::debug;

pub(crate) struct Semaphore {
    semaphore: vk::Semaphore,
}

impl Semaphore {
    pub fn new(device: &Device) -> Self {
        let semaphore_create_info = SemaphoreCreateInfo::builder();

        let semaphore = unsafe {
            device
                .logical_device()
                .create_semaphore(&semaphore_create_info, None)
                .expect("Failed to create semaphore")
        };

        debug!("Created semaphore");

        Self { semaphore }
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
