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
    raw: vk::Semaphore,

    device: Rc<Device>,
}

impl BinarySemaphore {
    pub(crate) fn new(device: Rc<Device>, create_info: BinarySemaphoreCreateInfo) -> Result<Self> {
        let BinarySemaphoreCreateInfo { label } = create_info;

        let create_info =
            vk::SemaphoreCreateInfo::builder().flags(vk::SemaphoreCreateFlags::empty());

        let raw = device.create_vk_semaphore(*create_info)?;

        device.set_object_name(DebugName {
            ty: vk::ObjectType::SEMAPHORE,
            object: raw,
            name: label,
        })?;

        Ok(Self { raw, device })
    }

    pub(crate) fn raw(&self) -> vk::Semaphore {
        self.raw
    }
}

impl Drop for BinarySemaphore {
    fn drop(&mut self) {
        self.device.destroy_semaphore(self.raw);
    }
}

pub(crate) struct BinarySemaphoreCreateInfo<'a> {
    pub(crate) label: &'a str,
}
