/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::vulkan::core::device::Device;

use ash::vk;
use color_eyre::Result;
use std::rc::Rc;

pub(crate) struct TimelineSemaphore {
    handle: vk::Semaphore,

    device: Rc<Device>,
}

impl TimelineSemaphore {
    pub(crate) fn new(
        device: Rc<Device>,
        create_info: TimelineSemaphoreCreateInfo,
    ) -> Result<Self> {
        let TimelineSemaphoreCreateInfo {
            label,
            initial_value,
        } = create_info;

        let mut type_create_info = vk::SemaphoreTypeCreateInfo::builder()
            .semaphore_type(vk::SemaphoreType::TIMELINE)
            .initial_value(initial_value);

        let create_info = vk::SemaphoreCreateInfo::builder().push_next(&mut type_create_info);

        let handle = unsafe { device.handle().create_semaphore(&create_info, None) }?;

        device.set_object_name(vk::ObjectType::SEMAPHORE, handle, label)?;

        Ok(Self { handle, device })
    }

    pub(crate) fn wait_for(&self, value: u64) -> Result<()> {
        let semaphores = [self.handle];
        let values = [value];
        let wait_info = vk::SemaphoreWaitInfo::builder()
            .semaphores(&semaphores)
            .values(&values);

        unsafe {
            self.device.handle().wait_semaphores(&wait_info, u64::MAX)?;
        }

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

pub(crate) struct TimelineSemaphoreCreateInfo<'a> {
    pub(crate) label: &'a str,

    pub(crate) initial_value: u64,
}
