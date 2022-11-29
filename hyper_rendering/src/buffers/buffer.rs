/*
 * Copyright (c) 2022, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::allocator::{Allocator, MemoryLocation};

use ash::{
    vk::{self, BufferUsageFlags},
    Device,
};
use gpu_allocator::vulkan::Allocation;
use log::debug;
use std::{cell::RefCell, rc::Rc};

pub(crate) struct CreateInfo<'a> {
    pub logical_device: &'a Device,
    pub allocator: &'a Rc<RefCell<Allocator>>,
    pub allocation_size: u64,
    pub usage: BufferUsageFlags,
}

pub(crate) struct Buffer {
    internal_buffer: vk::Buffer,
    allocation: Option<Allocation>,

    allocator: Rc<RefCell<Allocator>>,
    logical_device: Device,
}

impl Buffer {
    pub fn new(create_info: &CreateInfo) -> Self {
        let internal_buffer = Self::create_internal_buffer(
            create_info.logical_device,
            create_info.allocation_size,
            create_info.usage,
        );

        let allocation = Self::allocate_memory(
            create_info.logical_device,
            create_info.allocator,
            &internal_buffer,
        );

        debug!("Created vertex buffer");

        Self {
            internal_buffer,
            allocation: Some(allocation),

            allocator: create_info.allocator.clone(),
            logical_device: create_info.logical_device.clone(),
        }
    }

    fn create_internal_buffer(
        logical_device: &Device,
        allocation_size: u64,
        usage: BufferUsageFlags,
    ) -> vk::Buffer {
        let buffer_create_info = vk::BufferCreateInfo::builder()
            .size(allocation_size)
            .usage(usage);

        // TODO: Propagate error
        let internal_buffer = unsafe {
            logical_device
                .create_buffer(&buffer_create_info, None)
                .expect("FIXME")
        };

        debug!("Created internal buffer");

        internal_buffer
    }

    fn allocate_memory(
        logical_device: &Device,
        allocator: &Rc<RefCell<Allocator>>,
        internal_buffer: &vk::Buffer,
    ) -> Allocation {
        let memory_requirements =
            unsafe { logical_device.get_buffer_memory_requirements(*internal_buffer) };

        // TODO: Propagate error
        let allocation = allocator
            .borrow_mut()
            .allocate(memory_requirements, MemoryLocation::CpuToGpu)
            .expect("FIXME");

        debug!("Allocated buffer memory");

        // TODO: Propagate error
        unsafe {
            logical_device
                .bind_buffer_memory(*internal_buffer, allocation.memory(), allocation.offset())
                .expect("FIXME");
        }

        debug!("Bound buffer memory");

        allocation
    }

    pub fn internal_buffer(&self) -> &vk::Buffer {
        &self.internal_buffer
    }

    pub fn set_data<T>(&self, data: &[T]) {
        // TODO: Propagate error
        let memory = self
            .allocation
            .as_ref()
            .expect("FIXME")
            .mapped_ptr()
            .expect("FIXME")
            .as_ptr() as *mut T;

        unsafe {
            memory.copy_from_nonoverlapping(data.as_ptr(), data.len());
        }
    }
}

impl Drop for Buffer {
    fn drop(&mut self) {
        // Frees buffer allocation
        _ = self
            .allocator
            .borrow_mut()
            // Uses unwrap because it is guaranteed that the allocation exists
            .free(self.allocation.take().unwrap());

        unsafe {
            self.logical_device
                .destroy_buffer(self.internal_buffer, None);
        }
    }
}
