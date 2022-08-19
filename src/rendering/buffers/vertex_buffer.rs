/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use super::super::devices::device::Device;
use super::super::devices::instance::Instance;
use super::super::error::{Error, SuitabilityError};
use super::super::vertex::Vertex;

use ash::vk;
use log::debug;
use nalgebra_glm as glm;
use std::mem::size_of;
use std::ptr::copy_nonoverlapping;

pub struct VertexBuffer {
    device_memory: vk::DeviceMemory,
    buffer: vk::Buffer,

    vertices: Vec<Vertex>,
}

impl VertexBuffer {
    pub fn new(instance: &Instance, device: &Device) -> Result<Self, Error> {
        let vertices = vec![
            Vertex::new(glm::vec2(0.0, -0.5), glm::vec3(1.0, 1.0, 1.0)),
            Vertex::new(glm::vec2(0.5, 0.5), glm::vec3(0.0, 1.0, 0.0)),
            Vertex::new(glm::vec2(-0.5, 0.5), glm::vec3(0.0, 0.0, 1.0)),
        ];

        let create_info = vk::BufferCreateInfo::builder()
            .size((size_of::<Vertex>() * vertices.len()) as u64)
            .usage(vk::BufferUsageFlags::VERTEX_BUFFER)
            .sharing_mode(vk::SharingMode::EXCLUSIVE);

        let buffer = unsafe { device.logical_device().create_buffer(&create_info, None)? };

        // TODO: Replace memory allocations with VMA
        let requirements = unsafe {
            device
                .logical_device()
                .get_buffer_memory_requirements(buffer)
        };

        let memory_type_index = Self::find_memory_type_index(
            &instance,
            &device,
            vk::MemoryPropertyFlags::HOST_COHERENT | vk::MemoryPropertyFlags::HOST_VISIBLE,
            requirements,
        )?;

        let allocate_info = vk::MemoryAllocateInfo::builder()
            .allocation_size(requirements.size)
            .memory_type_index(memory_type_index);

        let device_memory = unsafe {
            device
                .logical_device()
                .allocate_memory(&allocate_info, None)?
        };

        unsafe {
            device
                .logical_device()
                .bind_buffer_memory(buffer, device_memory, 0)?;
        }

        let memory = unsafe {
            device.logical_device().map_memory(
                device_memory,
                0,
                create_info.size,
                vk::MemoryMapFlags::empty(),
            )?
        };

        unsafe {
            copy_nonoverlapping(vertices.as_ptr(), memory.cast(), vertices.len());
        }

        debug!("Created vertex buffer");
        Ok(Self {
            device_memory,
            buffer,

            vertices,
        })
    }

    fn find_memory_type_index(
        instance: &Instance,
        device: &Device,
        properties: vk::MemoryPropertyFlags,
        requirements: vk::MemoryRequirements,
    ) -> Result<u32, Error> {
        let memory = unsafe {
            instance
                .instance()
                .get_physical_device_memory_properties(*device.physical_device())
        };

        let index = (0..memory.memory_type_count)
            .find(|index| {
                let suitable = requirements.memory_type_bits & (1 << index) != 0;
                let memory_type = memory.memory_types[*index as usize];
                suitable && memory_type.property_flags.contains(properties)
            })
            .ok_or(Error::SuitabilityError(SuitabilityError(
                "Failed to find suitable memory type",
            )))?;

        Ok(index)
    }

    pub fn cleanup(&mut self, device: &Device) {
        unsafe {
            device
                .logical_device()
                .free_memory(self.device_memory, None);
            device.logical_device().destroy_buffer(self.buffer, None);
        }
    }

    pub fn vertices(&self) -> &Vec<Vertex> {
        &self.vertices
    }

    pub fn buffer(&self) -> &vk::Buffer {
        &self.buffer
    }
}
