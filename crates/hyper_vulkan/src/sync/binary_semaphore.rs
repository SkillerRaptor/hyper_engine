/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use std::sync::Arc;

use ash::vk;
use color_eyre::eyre::Result;

use crate::core::device::DeviceShared;

pub struct BinarySemaphore {
    raw: vk::Semaphore,
    device: Arc<DeviceShared>,
}

impl BinarySemaphore {
    pub(crate) fn new(device: &Arc<DeviceShared>) -> Result<Self> {
        let create_info = vk::SemaphoreCreateInfo::default();

        let raw = unsafe { device.raw().create_semaphore(&create_info, None) }?;

        Ok(Self {
            raw,
            device: Arc::clone(device),
        })
    }

    pub(crate) fn raw(&self) -> vk::Semaphore {
        self.raw
    }
}

impl Drop for BinarySemaphore {
    fn drop(&mut self) {
        unsafe {
            self.device.raw().destroy_semaphore(self.raw, None);
        }
    }
}
