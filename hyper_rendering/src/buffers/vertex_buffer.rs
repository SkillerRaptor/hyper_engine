/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    allocator::{Allocator, MemoryLocation},
    devices::device::Device,
    vertex::Vertex,
};

use ash::vk::{Buffer, BufferCreateInfo, BufferUsageFlags, SharingMode};
use gpu_allocator::vulkan::Allocation;
use log::debug;
use std::mem;

pub struct VertexBuffer {
    internal_buffer: Buffer,
    allocation: Option<Allocation>,
}

impl VertexBuffer {
    pub fn new(device: &Device, allocator: &mut Allocator, vertices: &Vec<Vertex>) -> Self {
        let internal_buffer = Self::create_internal_buffer(vertices.len(), device);
        let allocation = Self::allocate_memory(&internal_buffer, vertices, device, allocator);

        debug!("Created vertex buffer");

        Self {
            internal_buffer,
            allocation: Some(allocation),
        }
    }

    fn create_internal_buffer(vertex_count: usize, device: &Device) -> Buffer {
        let buffer_create_info = BufferCreateInfo::builder()
            .size((mem::size_of::<Vertex>() * vertex_count) as u64)
            .usage(BufferUsageFlags::VERTEX_BUFFER)
            .sharing_mode(SharingMode::EXCLUSIVE)
            .queue_family_indices(&[]);

        let internal_buffer = unsafe {
            device
                .logical_device()
                .create_buffer(&buffer_create_info, None)
                .expect("Failed to create internal vertex buffer")
        };

        debug!("Created internal vertex buffer");

        internal_buffer
    }

    fn allocate_memory(
        internal_buffer: &Buffer,
        vertices: &[Vertex],
        device: &Device,
        allocator: &mut Allocator,
    ) -> Allocation {
        let memory_requirements = unsafe {
            device
                .logical_device()
                .get_buffer_memory_requirements(*internal_buffer)
        };

        let allocation = allocator.allocate(memory_requirements, MemoryLocation::CpuToGpu);

        debug!("Allocated vertex buffer memory");

        unsafe {
            device
                .logical_device()
                .bind_buffer_memory(*internal_buffer, allocation.memory(), allocation.offset())
                .expect("Failed to bind vertex buffer memory");
        }

        let memory = allocation
            .mapped_ptr()
            .expect("Failed to map vertex buffer memory")
            .as_ptr() as *mut Vertex;

        unsafe {
            memory.copy_from_nonoverlapping(vertices.as_ptr(), vertices.len());
        }

        debug!("Mapped vertex buffer memory");

        allocation
    }

    pub fn cleanup(&mut self, device: &Device, allocator: &mut Allocator) {
        allocator.free(self.allocation.take().unwrap());

        unsafe {
            device
                .logical_device()
                .destroy_buffer(self.internal_buffer, None);
        }
    }

    pub fn internal_buffer(&self) -> &Buffer {
        &self.internal_buffer
    }
}
