/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    device::Device,
    error::{CreationError, RuntimeError},
};

use ash::vk;
use std::sync::Arc;

pub(crate) struct TimelineSemaphore {
    handle: vk::Semaphore,
    device: Arc<Device>,
}

impl TimelineSemaphore {
    pub(crate) fn new(device: Arc<Device>) -> Result<Self, CreationError> {
        let mut type_create_info = vk::SemaphoreTypeCreateInfo::builder()
            .semaphore_type(vk::SemaphoreType::TIMELINE)
            .initial_value(0);

        let create_info = vk::SemaphoreCreateInfo::builder().push_next(&mut type_create_info);

        let handle = unsafe {
            device
                .handle()
                .create_semaphore(&create_info, None)
                .map_err(|error| CreationError::VulkanCreation(error, "timeline semaphore"))
        }?;

        Ok(Self { handle, device })
    }

    pub(crate) fn wait_for(&self, value: u64) -> Result<(), RuntimeError> {
        let semaphores = [self.handle];
        let values = [value];
        let wait_info = vk::SemaphoreWaitInfo::builder()
            .semaphores(&semaphores)
            .values(&values);

        unsafe {
            self.device
                .handle()
                .wait_semaphores(&wait_info, u64::MAX)
                .map_err(RuntimeError::WaitSemaphore)
        }?;

        Ok(())
    }

    pub(crate) fn handle(&self) -> vk::Semaphore {
        self.handle
    }
}

impl Drop for TimelineSemaphore {
    fn drop(&mut self) {
        unsafe {
            self.device.handle().destroy_semaphore(self.handle, None);
        }
    }
}
