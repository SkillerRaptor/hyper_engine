/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::device::Device;

use ash::vk::{self, SemaphoreCreateFlags, SemaphoreCreateInfo};
use std::sync::Arc;
use thiserror::Error;

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
    /// Constructs a new semaphore
    ///
    /// Arguments:
    ///
    /// * `device`: Vulkan device
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

    /// Returns the vulkan semaphore handle
    pub(crate) fn handle(&self) -> &vk::Semaphore {
        &self.handle
    }
}

impl Drop for Semaphore {
    fn drop(&mut self) {
        unsafe {
            self.device.handle().destroy_semaphore(self.handle, None);
        }
    }
}
