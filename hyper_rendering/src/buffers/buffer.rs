/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
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
use tracing::instrument;

pub(crate) struct BufferCreateInfo<'a> {
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
    #[instrument(skip_all)]
    pub fn new(create_info: &BufferCreateInfo) -> Self {
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

    #[instrument(skip_all)]
    fn create_internal_buffer(
        logical_device: &Device,
        allocation_size: u64,
        usage: BufferUsageFlags,
    ) -> vk::Buffer {
        let buffer_create_info = vk::BufferCreateInfo::builder()
            .size(allocation_size)
            .usage(usage);

        let internal_buffer = unsafe {
            logical_device
                .create_buffer(&buffer_create_info, None)
                .expect("Failed to create internal buffer")
        };

        debug!("Created internal buffer");

        internal_buffer
    }

    #[instrument(skip_all)]
    fn allocate_memory(
        logical_device: &Device,
        allocator: &Rc<RefCell<Allocator>>,
        internal_buffer: &vk::Buffer,
    ) -> Allocation {
        let memory_requirements =
            unsafe { logical_device.get_buffer_memory_requirements(*internal_buffer) };

        let allocation = allocator
            .borrow_mut()
            .allocate(memory_requirements, MemoryLocation::CpuToGpu);

        debug!("Allocated buffer memory");

        unsafe {
            logical_device
                .bind_buffer_memory(*internal_buffer, allocation.memory(), allocation.offset())
                .expect("Failed to bind buffer memory");
        }

        debug!("Bound buffer memory");

        allocation
    }

    pub fn internal_buffer(&self) -> &vk::Buffer {
        &self.internal_buffer
    }

    #[instrument(skip_all)]
    pub fn set_data<T>(&self, data: &[T]) {
        let memory = self
            .allocation
            .as_ref()
            .unwrap()
            .mapped_ptr()
            .expect("Failed to map buffer memory")
            .as_ptr() as *mut T;

        unsafe {
            memory.copy_from_nonoverlapping(data.as_ptr(), data.len());
        }
    }
}

impl Drop for Buffer {
    #[instrument(skip_all)]
    fn drop(&mut self) {
        self.allocator
            .borrow_mut()
            .free(self.allocation.take().unwrap());

        unsafe {
            self.logical_device
                .destroy_buffer(self.internal_buffer, None);
        }
    }
}
