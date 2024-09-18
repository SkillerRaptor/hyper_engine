//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::sync::Arc;

use ash::vk;
use gpu_allocator::{
    vulkan::{Allocation, AllocationCreateDesc, AllocationScheme},
    MemoryLocation,
};
use hyper_core::alignment::Alignment;

use crate::{
    buffer::{self, BufferDescriptor, BufferUsage},
    resource::{Resource, ResourceHandle},
    vulkan::graphics_device::{GraphicsDevice, ResourceBuffer, ResourceHandler},
};

#[derive(Debug)]
pub(crate) struct Buffer {
    resource_handle: ResourceHandle,

    size: usize,

    allocation: Option<Allocation>,
    raw: vk::Buffer,

    resource_handler: Arc<ResourceHandler>,
}

impl Buffer {
    pub(crate) fn new(graphics_device: &GraphicsDevice, descriptor: &BufferDescriptor) -> Self {
        let size = descriptor.data.len();
        let aligned_size = size.align_up(buffer::ALIGNMENT);

        let buffer = Self::create_buffer(graphics_device, aligned_size);
        let allocation = Self::create_allocation(graphics_device, false, buffer);

        graphics_device.upload_buffer(descriptor.data, buffer);

        let resource_handle = graphics_device.allocate_buffer_handle(buffer);

        if let Some(label) = descriptor.label {
            graphics_device.set_debug_name(buffer, label);
            graphics_device
                .set_debug_name(unsafe { allocation.memory() }, &format!("{} Memory", label));
        }

        tracing::debug!(
            size,
            aligned_size,
            index = resource_handle.0,
            "Buffer created"
        );

        Self {
            resource_handle,

            size: descriptor.data.len(),

            allocation: Some(allocation),
            raw: buffer,

            resource_handler: Arc::clone(graphics_device.resource_handler()),
        }
    }

    pub(crate) fn new_staging(
        graphics_device: &GraphicsDevice,
        descriptor: &BufferDescriptor,
    ) -> Self {
        let size = descriptor.data.len();
        let aligned_size = size.align_up(buffer::ALIGNMENT);

        let buffer = Self::create_buffer(graphics_device, aligned_size);
        let allocation = Self::create_allocation(graphics_device, true, buffer);

        unsafe {
            let memory = allocation.mapped_ptr().unwrap().as_ptr() as *mut u8;
            memory.copy_from_nonoverlapping(descriptor.data.as_ptr(), descriptor.data.len());
        }

        tracing::trace!(size, aligned_size, "Staging Buffer created");

        Self {
            resource_handle: ResourceHandle::default(),

            size: descriptor.data.len(),

            allocation: Some(allocation),
            raw: buffer,

            resource_handler: Arc::clone(graphics_device.resource_handler()),
        }
    }

    fn create_buffer(graphics_device: &GraphicsDevice, aligned_size: usize) -> vk::Buffer {
        // NOTE: Will this hurt any performance?
        let usage = vk::BufferUsageFlags::STORAGE_BUFFER
            | vk::BufferUsageFlags::INDEX_BUFFER
            | vk::BufferUsageFlags::TRANSFER_SRC
            | vk::BufferUsageFlags::TRANSFER_DST;

        let create_info = vk::BufferCreateInfo::default()
            .size(aligned_size as u64)
            .usage(usage)
            .sharing_mode(vk::SharingMode::EXCLUSIVE)
            .queue_family_indices(&[]);

        unsafe { graphics_device.device().create_buffer(&create_info, None) }.unwrap()
    }

    fn create_allocation(
        graphics_device: &GraphicsDevice,
        staging: bool,
        buffer: vk::Buffer,
    ) -> Allocation {
        let requirements = unsafe {
            graphics_device
                .device()
                .get_buffer_memory_requirements(buffer)
        };

        let create_description = AllocationCreateDesc {
            name: "",
            requirements,
            location: if staging {
                MemoryLocation::CpuToGpu
            } else {
                MemoryLocation::GpuOnly
            },
            linear: true,
            allocation_scheme: AllocationScheme::DedicatedBuffer(buffer),
        };

        let allocation = graphics_device
            .allocator()
            .lock()
            .unwrap()
            .allocate(&create_description)
            .unwrap();

        unsafe {
            graphics_device
                .device()
                .bind_buffer_memory(buffer, allocation.memory(), 0)
                .unwrap();
        };

        allocation
    }

    pub(crate) fn raw(&self) -> vk::Buffer {
        self.raw
    }
}

impl Drop for Buffer {
    fn drop(&mut self) {
        self.resource_handler
            .buffers
            .lock()
            .unwrap()
            .push(ResourceBuffer {
                allocation: Some(self.allocation.take().unwrap()),
                buffer: self.raw,
                handle: self.resource_handle,
            });
    }
}

impl crate::buffer::Buffer for Buffer {
    fn usage(&self) -> BufferUsage {
        todo!()
    }

    fn size(&self) -> usize {
        self.size
    }
}

impl Resource for Buffer {
    fn handle(&self) -> ResourceHandle {
        self.resource_handle
    }
}
