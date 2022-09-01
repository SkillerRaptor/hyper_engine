/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use crate::devices::device::Device;

use ash::vk::{self, FenceCreateFlags, FenceCreateInfo};
use log::debug;
use tracing::instrument;

pub(crate) struct Fence {
    fence: vk::Fence,
}

impl Fence {
    #[instrument(skip_all)]
    pub fn new(device: &Device) -> Self {
        let fence_create_info = FenceCreateInfo::builder().flags(FenceCreateFlags::SIGNALED);

        let fence = unsafe {
            device
                .logical_device()
                .create_fence(&fence_create_info, None)
                .expect("Failed to create fence")
        };

        debug!("Created fence");

        Self { fence }
    }

    #[instrument(skip_all)]
    pub fn cleanup(&mut self, device: &Device) {
        unsafe {
            device.logical_device().destroy_fence(self.fence, None);
        }
    }

    pub fn fence(&self) -> &vk::Fence {
        &self.fence
    }

    #[instrument(skip_all)]
    pub fn wait(&self, device: &Device) {
        unsafe {
            device
                .logical_device()
                .wait_for_fences(&[self.fence], true, u64::MAX)
                .expect("Failed to wait for fence");
        }
    }

    #[instrument(skip_all)]
    pub fn reset(&self, device: &Device) {
        unsafe {
            device
                .logical_device()
                .reset_fences(&[self.fence])
                .expect("Failed to reset fence");
        }
    }
}
