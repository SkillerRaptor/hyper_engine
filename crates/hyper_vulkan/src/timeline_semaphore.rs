/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use std::sync::Arc;

use ash::vk;
use color_eyre::eyre::Result;

use crate::device::DeviceShared;

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

    pub fn wait_for(&self, value: u64) -> Result<()> {
        let semaphores = [self.raw];
        let values = [value];
        let wait_info = vk::SemaphoreWaitInfo::builder()
            .semaphores(&semaphores)
            .values(&values);

        unsafe {
            self.device
                .raw()
                .wait_semaphores(&wait_info, 1_000_000_000)?;
        }

        Ok(())
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
