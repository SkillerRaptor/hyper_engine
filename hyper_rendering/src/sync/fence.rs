/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use ash::{
    vk::{self, FenceCreateFlags},
    Device,
};
use log::debug;
use tracing::instrument;

pub(crate) struct FenceCreateInfo<'a> {
    pub logical_device: &'a Device,
}

pub(crate) struct Fence {
    fence: vk::Fence,

    logical_device: Device,
}

impl Fence {
    #[instrument(skip_all)]
    pub fn new(create_info: &FenceCreateInfo) -> Self {
        let fence_create_info = vk::FenceCreateInfo::builder().flags(FenceCreateFlags::SIGNALED);

        // TODO: Propagate error
        let fence = unsafe {
            create_info
                .logical_device
                .create_fence(&fence_create_info, None)
                .expect("FIXME")
        };

        debug!("Created fence");

        Self {
            fence,
            logical_device: create_info.logical_device.clone(),
        }
    }

    pub fn fence(&self) -> &vk::Fence {
        &self.fence
    }

    #[instrument(skip_all)]
    pub fn wait(&self) {
        // TODO: Propagate error
        unsafe {
            self.logical_device
                .wait_for_fences(&[self.fence], true, u64::MAX)
                .expect("FIXME");
        }
    }

    #[instrument(skip_all)]
    pub fn reset(&self) {
        // TODO: Propagate error
        unsafe {
            self.logical_device
                .reset_fences(&[self.fence])
                .expect("FIXME");
        }
    }
}

impl Drop for Fence {
    #[instrument(skip_all)]
    fn drop(&mut self) {
        unsafe {
            self.logical_device.destroy_fence(self.fence, None);
        }
    }
}
