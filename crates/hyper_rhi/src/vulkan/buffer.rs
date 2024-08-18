//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::{
    mem,
    sync::{atomic::Ordering, Arc},
};

use ash::vk;
use gpu_allocator::{
    vulkan::{Allocation, AllocationCreateDesc, AllocationScheme},
    MemoryLocation,
};

use crate::{
    buffer::{BufferDescriptor, BufferUsage},
    resource::ResourceHandle,
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
        let buffer = Self::create_buffer(graphics_device, descriptor.data);
        let allocation = Self::create_allocation(graphics_device, buffer);

        Self::set_data_internal(&allocation, descriptor.data);

        let buffer_info = vk::DescriptorBufferInfo::default()
            .buffer(buffer)
            .offset(0)
            .range(vk::WHOLE_SIZE);

        let buffer_infos = [buffer_info];

        let index = graphics_device.resource_number().load(Ordering::Relaxed);
        graphics_device
            .resource_number()
            .store(index + 1, Ordering::Relaxed);

        let write_set = vk::WriteDescriptorSet::default()
            .dst_set(graphics_device.descriptor_sets()[0])
            .dst_binding(0)
            .dst_array_element(index)
            .descriptor_type(vk::DescriptorType::STORAGE_BUFFER)
            .buffer_info(&buffer_infos);

        unsafe {
            graphics_device
                .device()
                .update_descriptor_sets(&[write_set], &[]);
        }

        tracing::debug!(size = descriptor.data.len(), index, "Buffer created");

        Self {
            resource_handle: ResourceHandle(index),

            size: descriptor.data.len(),

            allocation: Some(allocation),
            raw: buffer,

            resource_handler: Arc::clone(graphics_device.resource_handler()),
        }
    }

    fn create_buffer(graphics_device: &GraphicsDevice, data: &[u8]) -> vk::Buffer {
        // NOTE: Will this hurt any performance?
        let usage = vk::BufferUsageFlags::STORAGE_BUFFER
            | vk::BufferUsageFlags::INDEX_BUFFER
            | vk::BufferUsageFlags::TRANSFER_SRC
            | vk::BufferUsageFlags::TRANSFER_DST;

        let create_info = vk::BufferCreateInfo::default()
            .size(mem::size_of_val(data) as u64)
            .usage(usage)
            .sharing_mode(vk::SharingMode::EXCLUSIVE)
            .queue_family_indices(&[]);

        unsafe { graphics_device.device().create_buffer(&create_info, None) }.unwrap()
    }

    fn create_allocation(graphics_device: &GraphicsDevice, buffer: vk::Buffer) -> Allocation {
        let requirements = unsafe {
            graphics_device
                .device()
                .get_buffer_memory_requirements(buffer)
        };

        let create_description = AllocationCreateDesc {
            // TODO: Add buffer label
            name: "",
            requirements,
            // TODO: Add memory location option for staging buffers
            location: MemoryLocation::CpuToGpu,
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

    pub(crate) fn set_data(&self, data: &[u8]) {
        assert_eq!(data.len(), self.size);

        Self::set_data_internal(self.allocation.as_ref().unwrap(), data);
    }

    fn set_data_internal(allocation: &Allocation, data: &[u8]) {
        let memory = allocation.mapped_ptr().unwrap().as_ptr() as *mut u8;
        unsafe {
            memory.copy_from_nonoverlapping(data.as_ptr(), data.len());
        }
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

impl crate::resource::Resource for Buffer {
    fn resource_handle(&self) -> ResourceHandle {
        self.resource_handle
    }
}
