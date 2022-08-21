/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use super::super::devices::device::Device;
use super::super::error::Error;
use super::super::vertex::Vertex;

use ash::vk;
use gpu_allocator::vulkan;
use log::debug;
use std::mem::size_of;

pub struct VertexBuffer {
    buffer: vk::Buffer,
    allocation: Option<vulkan::Allocation>,
}

impl VertexBuffer {
    pub fn new(
        device: &Device,
        allocator: &mut vulkan::Allocator,
        vertices: &Vec<Vertex>,
    ) -> Result<Self, Error> {
        let create_info = vk::BufferCreateInfo::builder()
            .size((size_of::<Vertex>() * vertices.len()) as u64)
            .usage(vk::BufferUsageFlags::VERTEX_BUFFER)
            .sharing_mode(vk::SharingMode::EXCLUSIVE);

        let buffer = unsafe { device.logical_device().create_buffer(&create_info, None)? };

        let requirements = unsafe {
            device
                .logical_device()
                .get_buffer_memory_requirements(buffer)
        };

        let allocation_create_description = vulkan::AllocationCreateDesc {
            name: "Vertex Buffer",
            requirements,
            location: gpu_allocator::MemoryLocation::CpuToGpu,
            linear: true,
        };
        let allocation = allocator.allocate(&allocation_create_description)?;

        unsafe {
            device.logical_device().bind_buffer_memory(
                buffer,
                allocation.memory(),
                allocation.offset(),
            )?;
        }

        // NOTE: We use unwrap() because we need to mapped ptr to be available
        let memory = allocation.mapped_ptr().unwrap().as_ptr() as *mut Vertex;
        unsafe {
            memory.copy_from_nonoverlapping(vertices.as_ptr(), vertices.len());
        }

        debug!("Created vertex buffer");
        Ok(Self {
            buffer,
            allocation: Some(allocation),
        })
    }

    pub fn cleanup(&mut self, device: &Device, allocator: &mut vulkan::Allocator) {
        allocator.free(self.allocation.take().unwrap()).unwrap();

        unsafe {
            device.logical_device().destroy_buffer(self.buffer, None);
        }
    }

    pub fn buffer(&self) -> &vk::Buffer {
        &self.buffer
    }
}
