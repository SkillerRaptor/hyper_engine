/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use super::super::error::{Error, SuitabilityError};
use super::super::vertex::Vertex;

use ash::vk;
use log::debug;
use nalgebra_glm as glm;
use std::mem::size_of;
use std::ptr::copy_nonoverlapping;

pub struct VertexBuffer {
    logical_device: ash::Device,

    vertices: Vec<Vertex>,
    buffer: vk::Buffer,
    device_memory: vk::DeviceMemory,
}

impl VertexBuffer {
    pub fn new(
        instance: &ash::Instance,
        physical_device: &vk::PhysicalDevice,
        logical_device: &ash::Device,
    ) -> Result<Self, Error> {
        let vertices = vec![
            Vertex::new(glm::vec2(0.0, -0.5), glm::vec3(1.0, 1.0, 1.0)),
            Vertex::new(glm::vec2(0.5, 0.5), glm::vec3(0.0, 1.0, 0.0)),
            Vertex::new(glm::vec2(-0.5, 0.5), glm::vec3(0.0, 0.0, 1.0)),
        ];

        let create_info = vk::BufferCreateInfo::builder()
            .size((size_of::<Vertex>() * vertices.len()) as u64)
            .usage(vk::BufferUsageFlags::VERTEX_BUFFER)
            .sharing_mode(vk::SharingMode::EXCLUSIVE);

        let buffer = unsafe { logical_device.create_buffer(&create_info, None)? };

        // TODO: Replace memory allocations with VMA
        let requirements = unsafe { logical_device.get_buffer_memory_requirements(buffer) };

        let memory_type_index = Self::find_memory_type_index(
            &instance,
            &physical_device,
            vk::MemoryPropertyFlags::HOST_COHERENT | vk::MemoryPropertyFlags::HOST_VISIBLE,
            requirements,
        )?;

        let allocate_info = vk::MemoryAllocateInfo::builder()
            .allocation_size(requirements.size)
            .memory_type_index(memory_type_index);

        let device_memory = unsafe { logical_device.allocate_memory(&allocate_info, None)? };

        unsafe {
            logical_device.bind_buffer_memory(buffer, device_memory, 0)?;
        }

        let memory = unsafe {
            logical_device.map_memory(
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
            logical_device: logical_device.clone(),

            vertices,
            buffer,
            device_memory,
        })
    }

    fn find_memory_type_index(
        instance: &ash::Instance,
        physical_device: &vk::PhysicalDevice,
        properties: vk::MemoryPropertyFlags,
        requirements: vk::MemoryRequirements,
    ) -> Result<u32, Error> {
        let memory = unsafe { instance.get_physical_device_memory_properties(*physical_device) };

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

    pub fn vertices(&self) -> &Vec<Vertex> {
        &self.vertices
    }

    pub fn buffer(&self) -> &vk::Buffer {
        &self.buffer
    }
}

impl Drop for VertexBuffer {
    fn drop(&mut self) {
        unsafe {
            self.logical_device.free_memory(self.device_memory, None);
            self.logical_device.destroy_buffer(self.buffer, None);
        }
    }
}
