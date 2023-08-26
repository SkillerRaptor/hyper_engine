/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::vulkan::{
    core::{device::Device, instance::debug_utils::DebugName},
    memory::allocator::{
        Allocation, AllocationCreateInfo, AllocationScheme, Allocator, MemoryLocation,
    },
};

use ash::vk;
use color_eyre::{eyre::eyre, Result};
use std::{cell::RefCell, rc::Rc};

pub(crate) struct Buffer {
    allocation: Option<Allocation>,
    raw: vk::Buffer,

    allocator: Rc<RefCell<Allocator>>,
    device: Rc<Device>,
}

impl Buffer {
    pub(crate) fn new(
        device: Rc<Device>,
        allocator: Rc<RefCell<Allocator>>,
        create_info: BufferCreateInfo,
    ) -> Result<Self> {
        let BufferCreateInfo {
            label,

            usage,
            size,
            location,
        } = create_info;

        let raw = Self::create_buffer(&device, label, size, usage)?;

        let mut buffer = Self {
            allocation: None,
            raw,

            allocator,
            device,
        };

        buffer.allocate_memory(label, location)?;

        Ok(buffer)
    }

    fn create_buffer(
        device: &Device,
        label: &str,
        size: u64,
        usage: vk::BufferUsageFlags,
    ) -> Result<vk::Buffer> {
        let create_info = vk::BufferCreateInfo::builder()
            .size(size)
            .usage(usage)
            .sharing_mode(vk::SharingMode::EXCLUSIVE)
            .queue_family_indices(&[]);

        let raw = device.create_vk_buffer(*create_info)?;

        device.set_object_name(DebugName {
            ty: vk::ObjectType::BUFFER,
            object: raw,
            name: label,
        })?;

        Ok(raw)
    }

    fn allocate_memory(&mut self, label: &str, location: MemoryLocation) -> Result<()> {
        let requirements = self.device.get_buffer_memory_requirements(self);

        let allocation = self.allocator.borrow_mut().allocate(AllocationCreateInfo {
            label: Some(label),
            requirements,
            location,
            scheme: AllocationScheme::DedicatedBuffer(self.raw),
        })?;
        self.allocation = Some(allocation);

        self.device.bind_buffer_memory(self)?;

        Ok(())
    }

    pub(crate) fn set_data<T>(&self, data: &[T]) -> Result<()> {
        let memory = self
            .allocation
            .as_ref()
            .ok_or(eyre!("failed to take allocation as reference"))?
            .raw()
            .mapped_ptr()
            .ok_or(eyre!("failed to get mapped pointer of allocation"))?
            .as_ptr() as *mut T;

        unsafe {
            memory.copy_from_nonoverlapping(data.as_ptr(), data.len());
        }

        Ok(())
    }

    pub(crate) fn raw(&self) -> vk::Buffer {
        self.raw
    }

    pub(crate) fn allocation(&self) -> &Allocation {
        self.allocation.as_ref().unwrap()
    }
}

impl Drop for Buffer {
    fn drop(&mut self) {
        self.allocator
            .borrow_mut()
            .free(self.allocation.take().unwrap())
            .unwrap();

        self.device.destroy_buffer(self.raw);
    }
}

pub(crate) struct BufferCreateInfo<'a> {
    pub(crate) label: &'a str,

    pub(crate) usage: vk::BufferUsageFlags,
    pub(crate) size: u64,
    pub(crate) location: MemoryLocation,
}
