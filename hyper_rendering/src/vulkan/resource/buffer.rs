/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::vulkan::{
    core::device::Device,
    memory::allocator::{
        Allocation, AllocationCreateInfo, AllocationScheme, Allocator, MemoryLocation,
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
    ) -> Self {
        let handle = Self::create_buffer(&device, allocation_size as u64, usage);

        let allocation = Self::allocate_memory(&device, &allocator, memory_location, handle);

        Self {
            handle,
            allocation: Some(allocation),

            allocator,
            device,
        }
    }

    fn create_buffer(
        device: &Device,
        allocation_size: u64,
        usage: vk::BufferUsageFlags,
    ) -> vk::Buffer {
        let create_info = vk::BufferCreateInfo::builder()
            .size(allocation_size)
            .usage(usage)
            .sharing_mode(vk::SharingMode::EXCLUSIVE)
            .queue_family_indices(&[]);

        let buffer = unsafe {
            device
                .handle()
                .create_buffer(&create_info, None)
                .expect("failed to create buffer")
        };

        buffer
    }

    fn allocate_memory(
        device: &Device,
        allocator: &RefCell<Allocator>,
        memory_location: MemoryLocation,
        handle: vk::Buffer,
    ) -> Allocation {
        let memory_requirements = unsafe { device.handle().get_buffer_memory_requirements(handle) };

        // TODO: Add label
        let allocation = allocator.borrow_mut().allocate(AllocationCreateInfo {
            label: Some("<name> buffer"),
            requirements: memory_requirements,
            location: memory_location,
            scheme: AllocationScheme::DedicatedBuffer(handle),
        });

        unsafe {
            device
                .handle()
                .bind_buffer_memory(
                    handle,
                    allocation.handle().memory(),
                    allocation.handle().offset(),
                )
                .expect("failed to bind buffer memory");
        }

        allocation
    }

    pub fn set_data<T>(&self, data: &[T]) {
        let memory = self
            .allocation
            .as_ref()
            .expect("failed to get reference to allocation")
            .handle()
            .mapped_ptr()
            .expect("failed to get mapped pointer to allocation")
            .as_ptr() as *mut T;

        unsafe {
            memory.copy_from_nonoverlapping(data.as_ptr(), data.len());
        }
    }

    pub(crate) fn handle(&self) -> vk::Buffer {
        self.handle
    }
}

impl Drop for Buffer {
    fn drop(&mut self) {
        self.allocator
            .borrow_mut()
            .free(self.allocation.take().unwrap());

        unsafe {
            self.device.handle().destroy_buffer(self.handle, None);
        }
    }
}
