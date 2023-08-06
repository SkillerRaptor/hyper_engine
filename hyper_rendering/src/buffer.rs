/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    allocator::{Allocation, Allocator, MemoryLocation},
    device::Device,
    error::{CreationError, RuntimeError},
};

use ash::vk;
use std::sync::{Arc, Mutex};

pub(crate) struct Buffer {
    allocation: Option<Allocation>,
    handle: vk::Buffer,

    allocator: Arc<Mutex<Allocator>>,
    device: Arc<Device>,
}

impl Buffer {
    pub(crate) fn new(
        device: Arc<Device>,
        allocator: Arc<Mutex<Allocator>>,
        allocation_size: u64,
        usage: vk::BufferUsageFlags,
    ) -> Result<Self, CreationError> {
        let handle = Self::create_buffer(&device, allocation_size, usage)?;

        let allocation = Self::allocate_memory(&device, &allocator, &handle)?;

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
    ) -> Result<vk::Buffer, CreationError> {
        let create_info = vk::BufferCreateInfo::builder()
            .size(allocation_size)
            .usage(usage)
            .sharing_mode(vk::SharingMode::EXCLUSIVE)
            .queue_family_indices(&[]);

        let buffer = unsafe {
            device
                .handle()
                .create_buffer(&create_info, None)
                .map_err(|error| CreationError::VulkanCreation(error, "buffer"))?
        };

        Ok(buffer)
    }

    fn allocate_memory(
        device: &Device,
        allocator: &Mutex<Allocator>,
        handle: &vk::Buffer,
    ) -> Result<Allocation, CreationError> {
        let memory_requirements =
            unsafe { device.handle().get_buffer_memory_requirements(*handle) };

        let allocation = allocator
            .lock()
            .unwrap()
            .allocate(MemoryLocation::CpuToGpu, memory_requirements)
            .map_err(|error| {
                CreationError::RuntimeError(Box::new(error), "allocate memory for buffer")
            })?;

        unsafe {
            device
                .handle()
                .bind_buffer_memory(*handle, allocation.0.memory(), allocation.0.offset())
                .map_err(|error| CreationError::VulkanBind(error, "buffer"))?;
        }

        Ok(allocation)
    }

    pub fn set_data<T>(&self, data: &[T]) -> Result<(), RuntimeError> {
        let memory = self
            .allocation
            .as_ref()
            .ok_or(RuntimeError::BufferFailure)?
            .0
            .mapped_ptr()
            .ok_or(RuntimeError::BufferFailure)?
            .as_ptr() as *mut T;

        unsafe {
            memory.copy_from_nonoverlapping(data.as_ptr(), data.len());
        }

        Ok(())
    }
}

impl Drop for Buffer {
    fn drop(&mut self) {
        self.allocator
            .lock()
            .unwrap()
            .free(self.allocation.take().unwrap())
            .unwrap();

        unsafe {
            self.device.handle().destroy_buffer(self.handle, None);
        }
    }
}
