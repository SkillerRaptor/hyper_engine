/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::device::Device;

use ash::vk::{self, FenceCreateFlags, FenceCreateInfo};
use std::sync::Arc;
use thiserror::Error;

/// An enum representing the errors that can occur while constructing a fence
#[derive(Debug, Error)]
pub enum CreationError {
    /// Creation of a vulkan object failed
    #[error("creation of vulkan {1} failed")]
    Creation(#[source] vk::Result, &'static str),
}

/// A struct representing a wrapper for the vulkan fence
pub(crate) struct Fence {
    /// Vulkan fence handle
    handle: vk::Fence,

    /// Device Wrapper
    device: Arc<Device>,
}

impl Fence {
    pub(crate) fn new(device: &Arc<Device>) -> Result<Self, CreationError> {
        let fence_create_info = FenceCreateInfo::builder().flags(FenceCreateFlags::SIGNALED);

        let fence = unsafe {
            device
                .handle()
                .create_fence(&fence_create_info, None)
                .map_err(|error| CreationError::Creation(error, "fence"))
        }?;

        Ok(Self {
            handle: fence,
            device: device.clone(),
        })
    }
}

impl Drop for Fence {
    fn drop(&mut self) {
        unsafe {
            self.device.handle().destroy_fence(self.handle, None);
        }
    }
}
