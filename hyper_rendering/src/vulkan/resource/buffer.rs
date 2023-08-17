/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    error::{Error, Result},
    vulkan::{
        core::device::Device,
        memory::allocator::{
            Allocation, AllocationCreateInfo, AllocationScheme, Allocator, MemoryLocation,
        },
    },
};

use ash::vk;
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
        allocation_size: usize,
        usage: vk::BufferUsageFlags,
        memory_location: MemoryLocation,
    ) -> Result<Self> {
        let handle = Self::create_buffer(&device, allocation_size as u64, usage)?;

        let allocation = Self::allocate_memory(&device, &allocator, memory_location, handle)?;

        Ok(Self {
            handle,
            allocation: Some(allocation),

            allocator,
            device,
        })
    }

    fn create_buffer(
        device: &Device,
        allocation_size: u64,
        usage: vk::BufferUsageFlags,
    ) -> Result<vk::Buffer> {
        let create_info = vk::BufferCreateInfo::builder()
            .size(allocation_size)
            .usage(usage)
            .sharing_mode(vk::SharingMode::EXCLUSIVE)
            .queue_family_indices(&[]);

        let buffer = unsafe {
            device
                .handle()
                .create_buffer(&create_info, None)
                .map_err(|error| Error::VulkanCreation(error, "buffer"))?
        };

        Ok(buffer)
    }

    fn allocate_memory(
        device: &Device,
        allocator: &RefCell<Allocator>,
        memory_location: MemoryLocation,
        handle: vk::Buffer,
    ) -> Result<Allocation> {
        let memory_requirements = unsafe { device.handle().get_buffer_memory_requirements(handle) };

        // TODO: Add label
        let allocation = allocator.borrow_mut().allocate(AllocationCreateInfo {
            label: Some("<name> buffer"),
            requirements: memory_requirements,
            location: memory_location,
            scheme: AllocationScheme::DedicatedBuffer(handle),
        })?;

        unsafe {
            device
                .handle()
                .bind_buffer_memory(
                    handle,
                    allocation.handle().memory(),
                    allocation.handle().offset(),
                )
                .map_err(|error| Error::VulkanBind(error, "buffer"))?;
        }

        Ok(allocation)
    }

    pub fn set_data<T>(&self, data: &[T]) -> Result<()> {
        let memory = self
            .allocation
            .as_ref()
            .ok_or(Error::BufferDataFailure)?
            .handle()
            .mapped_ptr()
            .ok_or(Error::BufferDataFailure)?
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
