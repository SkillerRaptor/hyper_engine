/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::device::DeviceShared;

use ash::vk;
use color_eyre::eyre::Result;

use std::sync::Arc;

pub struct TimelineSemaphore {
    raw: vk::Semaphore,
    device: Arc<DeviceShared>,
}

impl TimelineSemaphore {
    pub(crate) fn new(device: &Arc<DeviceShared>) -> Result<Self> {
        let mut type_create_info = vk::SemaphoreTypeCreateInfo::builder()
            .semaphore_type(vk::SemaphoreType::TIMELINE)
            .initial_value(0);

        let create_info = vk::SemaphoreCreateInfo::builder().push_next(&mut type_create_info);

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

impl Drop for TimelineSemaphore {
    fn drop(&mut self) {
        unsafe {
            self.device.raw().destroy_semaphore(self.raw, None);
        }
    }
}
