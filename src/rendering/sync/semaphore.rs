/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use super::super::devices::device::Device;
use super::super::error::Error;

use ash::vk;
use log::debug;
use std::rc::Rc;

pub struct Semaphore {
    semaphore: vk::Semaphore,

    device: Rc<Device>,
}

impl Semaphore {
    pub fn new(device: &Rc<Device>) -> Result<Self, Error> {
        let semaphore_create_info = vk::SemaphoreCreateInfo::builder();

        let semaphore = unsafe {
            device
                .logical_device()
                .create_semaphore(&semaphore_create_info, None)?
        };

        debug!("Created semaphore");
        Ok(Self {
            semaphore,

            device: device.clone(),
        })
    }

    pub fn semaphore(&self) -> &vk::Semaphore {
        &self.semaphore
    }
}

impl Drop for Semaphore {
    fn drop(&mut self) {
        unsafe {
            self.device
                .logical_device()
                .destroy_semaphore(self.semaphore, None);
        }
    }
}
