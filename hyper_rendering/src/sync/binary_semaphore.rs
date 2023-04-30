/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::device::Device;

use ash::vk::{self, Semaphore, SemaphoreCreateFlags, SemaphoreCreateInfo};
use std::sync::Arc;
use thiserror::Error;

#[derive(Debug, Error)]
pub enum CreationError {
    #[error("failed to create binary semaphore")]
    Creation(#[source] vk::Result),
}

pub(crate) struct BinarySemaphore {
    handle: Semaphore,
    device: Arc<Device>,
}

impl BinarySemaphore {
    pub(crate) fn new(device: Arc<Device>) -> Result<Self, CreationError> {
        let create_info = SemaphoreCreateInfo::builder().flags(SemaphoreCreateFlags::empty());

        let handle = unsafe {
            device
                .handle()
                .create_semaphore(&create_info, None)
                .map_err(CreationError::Creation)
        }?;

        Ok(Self { handle, device })
    }

    pub(crate) fn handle(&self) -> &Semaphore {
        &self.handle
    }
}

impl Drop for BinarySemaphore {
    fn drop(&mut self) {
        unsafe {
            self.device.handle().destroy_semaphore(self.handle, None);
        }
    }
}
