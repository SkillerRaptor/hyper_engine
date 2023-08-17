/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    device::Device,
    error::{Error, Result},
};

use ash::vk;
use std::rc::Rc;

pub(crate) struct BinarySemaphore {
    handle: vk::Semaphore,
    device: Rc<Device>,
}

impl BinarySemaphore {
    pub(crate) fn new(device: Rc<Device>) -> Result<Self> {
        let create_info =
            vk::SemaphoreCreateInfo::builder().flags(vk::SemaphoreCreateFlags::empty());

        let handle = unsafe {
            device
                .handle()
                .create_semaphore(&create_info, None)
                .map_err(|error| Error::VulkanCreation(error, "binary semaphore"))
        }?;

        Ok(Self { handle, device })
    }

    pub(crate) fn handle(&self) -> vk::Semaphore {
        self.handle
    }
}

impl Drop for BinarySemaphore {
    fn drop(&mut self) {
        unsafe {
            self.device.handle().destroy_semaphore(self.handle, None);
        }
    }
}
