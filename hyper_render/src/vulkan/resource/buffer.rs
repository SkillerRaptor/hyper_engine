/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::vulkan::{
    core::{debug_utils::DebugName, device::Device},
    memory::allocator::{
        Allocation, AllocationCreateInfo, AllocationScheme, Allocator, MemoryLocation,
    },
};

use ash::vk;
use color_eyre::{eyre::eyre, Result};
use std::{cell::RefCell, rc::Rc};

pub(crate) struct Buffer {
    allocation: Option<Allocation>,
    handle: vk::Buffer,

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

        let handle = Self::create_buffer(&device, label, size, usage)?;
        let allocation = Self::allocate_memory(&device, &allocator, label, location, handle)?;

        Ok(Self {
            handle,
            allocation: Some(allocation),

            allocator,
            device,
        })
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

        let handle = unsafe { device.handle().create_buffer(&create_info, None) }?;

        device.set_object_name(DebugName {
            ty: vk::ObjectType::BUFFER,
            object: handle,
            name: label,
        })?;

        Ok(handle)
    }

    fn allocate_memory(
        device: &Device,
        allocator: &RefCell<Allocator>,
        label: &str,
        location: MemoryLocation,
        handle: vk::Buffer,
    ) -> Result<Allocation> {
        let requirements = unsafe { device.handle().get_buffer_memory_requirements(handle) };

        // TODO: Add label
        let allocation = allocator.borrow_mut().allocate(AllocationCreateInfo {
            label: Some(label),
            requirements,
            location,
            scheme: AllocationScheme::DedicatedBuffer(handle),
        })?;

        unsafe {
            device.handle().bind_buffer_memory(
                handle,
                allocation.handle().memory(),
                allocation.handle().offset(),
            )?
        }

        Ok(allocation)
    }

    pub fn set_data<T>(&self, data: &[T]) -> Result<()> {
        let memory = self
            .allocation
            .as_ref()
            .ok_or(eyre!("failed to take allocation as reference"))?
            .handle()
            .mapped_ptr()
            .ok_or(eyre!("failed to get mapped pointer of allocation"))?
            .as_ptr() as *mut T;

        unsafe {
            memory.copy_from_nonoverlapping(data.as_ptr(), data.len());
        }

        Ok(())
    }

    pub(crate) fn handle(&self) -> vk::Buffer {
        self.handle
    }
}

impl Drop for Buffer {
    fn drop(&mut self) {
        self.allocator
            .borrow_mut()
            .free(self.allocation.take().unwrap())
            .unwrap();

        unsafe {
            self.device.handle().destroy_buffer(self.handle, None);
        }
    }
}

pub(crate) struct BufferCreateInfo<'a> {
    pub(crate) label: &'a str,

    pub(crate) usage: vk::BufferUsageFlags,
    pub(crate) size: u64,
    pub(crate) location: MemoryLocation,
}
