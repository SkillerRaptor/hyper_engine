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
use nalgebra_glm as glm;

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

    pub fn load(
        device: &Device,
        allocator: &mut vulkan::Allocator,
        file_name: &str,
    ) -> Result<Self, Error> {
        let (models, _) = tobj::load_obj(&file_name, &tobj::LoadOptions::default())?;

        let mut vertices = Vec::new();
        for model in models {
            let mesh = &model.mesh;

            for i in 0..mesh.indices.len() {
                let index = 3 * mesh.indices[i] as usize;
                let position = glm::vec3(
                    mesh.positions[index + 0],
                    mesh.positions[index + 1],
                    mesh.positions[index + 2],
                );
                let color = glm::vec3(
                    mesh.normals[index + 0],
                    mesh.normals[index + 1],
                    mesh.normals[index + 2],
                );
                let normal = glm::vec3(
                    mesh.normals[index + 0],
                    mesh.normals[index + 1],
                    mesh.normals[index + 2],
                );

                let vertex = Vertex::new(position, color, normal);
                vertices.push(vertex);
            }
        }

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
