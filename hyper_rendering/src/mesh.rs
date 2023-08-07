/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    allocator::Allocator,
    buffer::Buffer,
    descriptor_manager::DescriptorManager,
    device::Device,
    error::{CreationError, CreationResult},
    resource_handle::ResourceHandle,
};

use hyper_math::vector::Vec4f;

use ash::vk;
use std::{
    cell::RefCell,
    mem,
    rc::Rc,
    sync::{Arc, Mutex},
};
use tobj::LoadOptions;

#[repr(C)]
#[derive(Clone, Copy, Debug)]
pub(crate) struct Vertex {
    pub(crate) position: Vec4f,
    pub(crate) normal: Vec4f,
    pub(crate) color: Vec4f,
}

pub(crate) struct Mesh {
    vertex_buffer_handle: ResourceHandle,
    _vertex_buffer: Buffer,
    vertices: Vec<Vertex>,

    descriptor_manager: Rc<RefCell<DescriptorManager>>,
}

impl Mesh {
    pub(crate) fn new(
        device: Arc<Device>,
        allocator: Arc<Mutex<Allocator>>,
        descriptor_manager: Rc<RefCell<DescriptorManager>>,
        vertices: Vec<Vertex>,
    ) -> CreationResult<Self> {
        let vertex_buffer = Buffer::new(
            device,
            allocator,
            mem::size_of::<Vertex>() * vertices.len(),
            vk::BufferUsageFlags::STORAGE_BUFFER,
        )?;

        vertex_buffer
            .set_data(&vertices)
            .map_err(|error| CreationError::RuntimeError(Box::new(error), "set data for mesh"))?;

        let vertex_buffer_handle = descriptor_manager
            .borrow_mut()
            .allocate_buffer_handle(&vertex_buffer);

        Ok(Self {
            vertex_buffer_handle,
            _vertex_buffer: vertex_buffer,
            vertices,

            descriptor_manager,
        })
    }

    // TODO: Move this into asset manager and model class
    pub(crate) fn load(
        device: Arc<Device>,
        allocator: Arc<Mutex<Allocator>>,
        descriptor_manager: Rc<RefCell<DescriptorManager>>,
        mesh_file: &str,
    ) -> CreationResult<Self> {
        let (models, _) = tobj::load_obj(mesh_file, &LoadOptions::default())
            .map_err(|error| CreationError::LoadFailure(error, mesh_file.to_string()))?;

        let mut vertices = Vec::new();
        for model in models {
            let mesh = &model.mesh;

            for i in 0..mesh.indices.len() {
                let index = 3 * mesh.indices[i] as usize;
                let position = Vec4f::new(
                    mesh.positions[index],
                    mesh.positions[index + 1],
                    mesh.positions[index + 2],
                    1.0,
                );
                let normal = Vec4f::new(
                    mesh.normals[index],
                    mesh.normals[index + 1],
                    mesh.normals[index + 2],
                    1.0,
                );
                let color = Vec4f::new(
                    mesh.normals[index],
                    mesh.normals[index + 1],
                    mesh.normals[index + 2],
                    1.0,
                );

                let vertex = Vertex {
                    position,
                    normal,
                    color,
                };

                vertices.push(vertex);
            }
        }

        let mesh = Self::new(device, allocator, descriptor_manager, vertices)?;
        Ok(mesh)
    }

    pub(crate) fn vertices(&self) -> &[Vertex] {
        &self.vertices
    }

    pub(crate) fn vertex_buffer_handle(&self) -> ResourceHandle {
        self.vertex_buffer_handle
    }
}

impl Drop for Mesh {
    fn drop(&mut self) {
        self.descriptor_manager
            .borrow_mut()
            .retire_handle(self.vertex_buffer_handle);
    }
}
