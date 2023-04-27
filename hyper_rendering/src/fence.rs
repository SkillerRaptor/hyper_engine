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
    /// Constructs a new fence
    ///
    /// Arguments:
    ///
    /// * `device`: Vulkan device
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

    /// Waits for the fence
    pub(crate) fn wait(&self) {
        // TODO: Propagate error
        unsafe {
            self.device
                .handle()
                .wait_for_fences(&[self.handle], true, 1_000_000_000)
                .unwrap();
        }
    }

    /// Resets the fence
    pub(crate) fn reset(&self) {
        // TODO: Propagate error
        unsafe {
            self.device.handle().reset_fences(&[self.handle]).unwrap();
        }
    }

    /// Returns the vulkan fence handle
    pub(crate) fn handle(&self) -> &vk::Fence {
        &self.handle
    }
}

impl Drop for Fence {
    fn drop(&mut self) {
        unsafe {
            self.device.handle().destroy_fence(self.handle, None);
        }
    }
}
