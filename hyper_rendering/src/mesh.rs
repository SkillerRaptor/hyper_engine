/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    allocator::Allocator, buffers::vertex_buffer::VertexBuffer, devices::device::Device,
    vertex::Vertex,
};

use nalgebra_glm as glm;
use tobj::LoadOptions;
use tracing::instrument;

pub(crate) struct Mesh {
    vertices: Vec<Vertex>,
    vertex_buffer: VertexBuffer,
}

impl Mesh {
    #[instrument(skip_all)]
    pub fn new(device: &Device, allocator: &mut Allocator, vertices: &Vec<Vertex>) -> Self {
        let vertex_buffer = VertexBuffer::new(device, allocator, vertices);

        Self {
            vertices: vertices.clone(),
            vertex_buffer,
        }
    }

    #[instrument(skip_all)]
    pub fn load(device: &Device, allocator: &mut Allocator, file_name: &str) -> Self {
        let (models, _) = tobj::load_obj(&file_name, &LoadOptions::default())
            .expect("Failed to load object file");

        let mut vertices = Vec::new();
        for model in models {
            let mesh = &model.mesh;

            for i in 0..mesh.indices.len() {
                let index = 3 * mesh.indices[i] as usize;
                let position = glm::vec3(
                    mesh.positions[index],
                    mesh.positions[index + 1],
                    mesh.positions[index + 2],
                );
                let color = glm::vec3(
                    mesh.normals[index],
                    mesh.normals[index + 1],
                    mesh.normals[index + 2],
                );
                let normal = glm::vec3(
                    mesh.normals[index],
                    mesh.normals[index + 1],
                    mesh.normals[index + 2],
                );

                let vertex = Vertex::new(position, color, normal);
                vertices.push(vertex);
            }
        }

        let vertex_buffer = VertexBuffer::new(device, allocator, &vertices);

        Self {
            vertices: vertices.clone(),
            vertex_buffer,
        }
    }

    #[instrument(skip_all)]
    pub fn cleanup(&mut self, device: &Device, allocator: &mut Allocator) {
        self.vertex_buffer.cleanup(device, allocator);
    }

    pub fn vertices(&self) -> &Vec<Vertex> {
        &self.vertices
    }

    pub fn vertex_buffer(&self) -> &VertexBuffer {
        &self.vertex_buffer
    }
}
