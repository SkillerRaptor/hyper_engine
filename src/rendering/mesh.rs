/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use super::buffers::vertex_buffer::VertexBuffer;
use super::devices::device::Device;
use super::error::Error;
use super::vertex::Vertex;

use gpu_allocator::vulkan;

pub struct Mesh {
    vertices: Vec<Vertex>,
    vertex_buffer: VertexBuffer,
}

impl Mesh {
    pub fn new(
        device: &Device,
        allocator: &mut vulkan::Allocator,
        vertices: &Vec<Vertex>,
    ) -> Result<Self, Error> {
        let vertex_buffer = VertexBuffer::new(&device, allocator, &vertices)?;

        Ok(Self {
            vertices: vertices.clone(),
            vertex_buffer,
        })
    }

    pub fn cleanup(&mut self, device: &Device, allocator: &mut vulkan::Allocator) {
        self.vertex_buffer.cleanup(&device, allocator);
    }

    pub fn vertices(&self) -> &Vec<Vertex> {
        &self.vertices
    }

    pub fn vertex_buffer(&self) -> &VertexBuffer {
        &self.vertex_buffer
    }
}
