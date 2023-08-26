/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::vulkan::core::{device::Device, instance::debug_utils::DebugName};

use ash::vk;
use color_eyre::Result;
use std::rc::Rc;

pub(crate) struct TimelineSemaphore {
    raw: vk::Semaphore,

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

        let raw = device.create_vk_semaphore(*create_info)?;

        device.set_object_name(DebugName {
            ty: vk::ObjectType::SEMAPHORE,
            object: raw,
            name: label,
        })?;

        Ok(Self { raw, device })
    }

    pub(crate) fn wait_for(&self, value: u64) -> Result<()> {
        let semaphores = [self.raw];
        let values = [value];
        let wait_info = vk::SemaphoreWaitInfo::builder()
            .semaphores(&semaphores)
            .values(&values);

        self.device.wait_semaphores(*wait_info)?;
        Ok(())
    }

    pub(crate) fn raw(&self) -> vk::Semaphore {
        self.raw
    }
}

impl Drop for TimelineSemaphore {
    fn drop(&mut self) {
        self.device.destroy_semaphore(self.raw);
    }
}

pub(crate) struct TimelineSemaphoreCreateInfo<'a> {
    pub(crate) label: &'a str,

    pub(crate) initial_value: u64,
}
