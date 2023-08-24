/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::vulkan::core::{debug_utils::DebugName, device::Device};

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

        let handle = unsafe { device.handle().create_semaphore(&create_info, None) }?;

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
        unsafe {
            self.device.handle().destroy_semaphore(self.handle, None);
        }
    }
}

pub(crate) struct BinarySemaphoreCreateInfo<'a> {
    pub(crate) label: &'a str,
}
