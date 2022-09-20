/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use ash::{vk, Device};
use log::debug;
use tracing::instrument;

pub(crate) struct SemaphoreCreateInfo<'a> {
    pub logical_device: &'a Device,
}

pub(crate) struct Semaphore {
    semaphore: vk::Semaphore,

    logical_device: Device,
}

impl Semaphore {
    #[instrument(skip_all)]
    pub fn new(create_info: &SemaphoreCreateInfo) -> Self {
        let semaphore_create_info = vk::SemaphoreCreateInfo::builder();

        // TODO: Propagate error
        let semaphore = unsafe {
            create_info
                .logical_device
                .create_semaphore(&semaphore_create_info, None)
                .expect("FIXME")
        };

        debug!("Created semaphore");

        Self {
            semaphore,
            logical_device: create_info.logical_device.clone(),
        }
    }

    pub fn semaphore(&self) -> &vk::Semaphore {
        &self.semaphore
    }
}

impl Drop for Semaphore {
    #[instrument(skip_all)]
    fn drop(&mut self) {
        unsafe {
            self.logical_device.destroy_semaphore(self.semaphore, None);
        }
    }
}
