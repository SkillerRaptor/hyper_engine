/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use std::sync::Arc;

use ash::vk::{self, SemaphoreCreateFlags, SemaphoreCreateInfo};
use thiserror::Error;

use crate::device::Device;

/// An enum representing the errors that can occur while constructing a semaphore
#[derive(Debug, Error)]
pub enum CreationError {
    /// Creation of a vulkan object failed
    #[error("creation of vulkan {1} failed")]
    Creation(#[source] vk::Result, &'static str),
}

/// A struct representing a wrapper for the vulkan semaphore
pub(crate) struct Semaphore {
    /// Vulkan semaphore handle
    handle: vk::Semaphore,

    /// Device Wrapper
    device: Arc<Device>,
}

impl Semaphore {
    pub(crate) fn new(device: &Arc<Device>) -> Result<Self, CreationError> {
        let semaphore_create_info =
            SemaphoreCreateInfo::builder().flags(SemaphoreCreateFlags::from_raw(0));

        let semaphore = unsafe {
            device
                .handle()
                .create_semaphore(&semaphore_create_info, None)
                .map_err(|error| CreationError::Creation(error, "semaphore"))
        }?;

        Ok(Self {
            handle: semaphore,
            device: device.clone(),
        })
    }
}

impl Drop for Semaphore {
    fn drop(&mut self) {
        unsafe {
            self.device.handle().destroy_semaphore(self.handle, None);
        }
    }
}
