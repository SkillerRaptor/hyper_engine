/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use super::super::devices::device::Device;
use super::super::error::Error;

use ash::vk;
use log::debug;
use std::rc::Rc;

pub struct Fence {
    fence: vk::Fence,

    device: Rc<Device>,
}

impl Fence {
    pub fn new(device: &Rc<Device>) -> Result<Self, Error> {
        let fence_create_info =
            vk::FenceCreateInfo::builder().flags(vk::FenceCreateFlags::SIGNALED);

        let fence = unsafe {
            device
                .logical_device()
                .create_fence(&fence_create_info, None)?
        };

        debug!("Created fence");
        Ok(Self {
            fence,

            device: device.clone(),
        })
    }

    pub fn fence(&self) -> &vk::Fence {
        &self.fence
    }
}

impl Drop for Fence {
    fn drop(&mut self) {
        unsafe {
            self.device.logical_device().destroy_fence(self.fence, None);
        }
    }
}
