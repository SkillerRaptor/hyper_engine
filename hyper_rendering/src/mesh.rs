/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    allocator::Allocator,
    buffers::vertex_buffer::{VertexBuffer, VertexBufferCreateInfo},
    vertex::Vertex,
};

use ash::Device;
use nalgebra_glm as glm;
use std::{cell::RefCell, rc::Rc};
use tobj::LoadOptions;

pub(crate) struct MeshCreateInfo<'a> {
    pub logical_device: &'a Device,
    pub allocator: &'a Rc<RefCell<Allocator>>,
    pub vertices: &'a [Vertex],
}

pub(crate) struct MeshLoadInfo<'a> {
    pub logical_device: &'a Device,
    pub allocator: &'a Rc<RefCell<Allocator>>,
    pub file_name: &'a str,
}

pub(crate) struct Mesh {
    vertex_buffer: VertexBuffer,
    _vertices: Vec<Vertex>,
}

impl Mesh {
    pub fn new(create_info: &MeshCreateInfo) -> Self {
        let vertex_buffer_create_info = VertexBufferCreateInfo {
            logical_device: create_info.logical_device,
            allocator: create_info.allocator,
            vertices: create_info.vertices,
        };

        let vertex_buffer = VertexBuffer::new(&vertex_buffer_create_info);

        Self {
            vertex_buffer,
            _vertices: create_info.vertices.to_vec(),
        }
    }

    pub fn load(load_info: &MeshLoadInfo) -> Self {
        // TODO: Propagate error
        let (models, _) =
            tobj::load_obj(load_info.file_name, &LoadOptions::default()).expect("FIXME");

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
                let _normal = glm::vec3(
                    mesh.normals[index],
                    mesh.normals[index + 1],
                    mesh.normals[index + 2],
                );

                let vertex = Vertex::new(position, color /*, normal*/);
                vertices.push(vertex);
            }
        }

        let vertex_buffer_create_info = VertexBufferCreateInfo {
            logical_device: load_info.logical_device,
            allocator: load_info.allocator,
            vertices: vertices.as_slice(),
        };

        let vertex_buffer = VertexBuffer::new(&vertex_buffer_create_info);

        Self {
            _vertices: vertices.clone(),
            vertex_buffer,
        }
    }

    /*
    pub fn vertices(&self) -> &Vec<Vertex> {
        &self._vertices
    }
    */

    pub fn vertex_buffer(&self) -> &VertexBuffer {
        &self.vertex_buffer
    }
}
