/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::vulkan::core::device::Device;

use ash::vk;
use std::rc::Rc;

pub(crate) struct BinarySemaphore {
    handle: vk::Semaphore,
    device: Rc<Device>,
}

impl BinarySemaphore {
    pub(crate) fn new(device: Rc<Device>) -> Self {
        let create_info =
            vk::SemaphoreCreateInfo::builder().flags(vk::SemaphoreCreateFlags::empty());

        let handle = unsafe {
            device
                .handle()
                .create_semaphore(&create_info, None)
                .expect("failed to create binary semaphore")
        };

        Self { handle, device }
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
