/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use super::super::devices::device::Device;
use super::super::error::Error;

use ash::vk;
use log::debug;

pub struct Fence {
    fence: vk::Fence,
}

impl Fence {
    pub fn new(device: &Device) -> Result<Self, Error> {
        let fence_create_info =
            vk::FenceCreateInfo::builder().flags(vk::FenceCreateFlags::SIGNALED);

        let fence = unsafe {
            device
                .logical_device()
                .create_fence(&fence_create_info, None)?
        };

        debug!("Created fence");
        Ok(Self { fence })
    }

    pub fn cleanup(&mut self, device: &Device) {
        unsafe {
            device.logical_device().destroy_fence(self.fence, None);
        }
    }

    pub fn fence(&self) -> &vk::Fence {
        &self.fence
    }

    pub fn wait(&self, device: &Device, timeout: u64) -> Result<(), Error> {
        unsafe {
            device
                .logical_device()
                .wait_for_fences(&[self.fence], true, timeout)?;
        }

        Ok(())
    }

    pub fn reset(&self, device: &Device) -> Result<(), Error> {
        unsafe {
            device.logical_device().reset_fences(&[self.fence])?;
        }

        Ok(())
    }
}
