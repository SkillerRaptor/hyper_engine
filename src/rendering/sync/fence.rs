/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use super::super::error::Error;

use ash::vk;
use log::debug;

pub struct Fence {
    fence: vk::Fence,

    logical_device: ash::Device,
}

impl Fence {
    pub fn new(logical_device: &ash::Device) -> Result<Self, Error> {
        let fence_create_info =
            vk::FenceCreateInfo::builder().flags(vk::FenceCreateFlags::SIGNALED);

        let fence = unsafe { logical_device.create_fence(&fence_create_info, None)? };

        debug!("Created fence");
        Ok(Self {
            fence,

            logical_device: logical_device.clone(),
        })
    }

    pub fn fence(&self) -> &vk::Fence {
        &self.fence
    }
}

impl Drop for Fence {
    fn drop(&mut self) {
        unsafe {
            self.logical_device.destroy_fence(self.fence, None);
        }
    }
}
