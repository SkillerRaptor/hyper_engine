/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::vulkan::core::{device::Device, instance::debug_utils::DebugName};

use ash::vk;
use color_eyre::Result;
use std::rc::Rc;

pub(crate) struct BinarySemaphore {
    handle: vk::Semaphore,

    device: Rc<Device>,
}

impl BinarySemaphore {
    pub(crate) fn new(device: Rc<Device>, create_info: BinarySemaphoreCreateInfo) -> Result<Self> {
        let BinarySemaphoreCreateInfo { label } = create_info;

        let create_info =
            vk::SemaphoreCreateInfo::builder().flags(vk::SemaphoreCreateFlags::empty());

        let handle = device.create_semaphore(*create_info)?;

        device.set_object_name(DebugName {
            ty: vk::ObjectType::SEMAPHORE,
            object: handle,
            name: label,
        })?;

        Ok(Self { handle, device })
    }

    pub(crate) fn handle(&self) -> vk::Semaphore {
        self.handle
    }
}

impl Drop for BinarySemaphore {
    fn drop(&mut self) {
        self.device.destroy_semaphore(self.handle);
    }
}

pub(crate) struct BinarySemaphoreCreateInfo<'a> {
    pub(crate) label: &'a str,
}
