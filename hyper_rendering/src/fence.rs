/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::device::Device;

use ash::vk::{self, FenceCreateFlags, FenceCreateInfo};
use std::sync::Arc;
use thiserror::Error;

#[derive(Debug, Error)]
pub enum CreationError {
    #[error("creation of vulkan {1} failed")]
    Creation(#[source] vk::Result, &'static str),
}

pub(crate) struct Fence {
    handle: vk::Fence,
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

    pub(crate) fn wait(&self) {
        // TODO: Propagate error
        unsafe {
            self.device
                .handle()
                .wait_for_fences(&[self.handle], true, 1_000_000_000)
                .unwrap();
        }
    }

    pub(crate) fn reset(&self) {
        // TODO: Propagate error
        unsafe {
            self.device.handle().reset_fences(&[self.handle]).unwrap();
        }
    }

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
