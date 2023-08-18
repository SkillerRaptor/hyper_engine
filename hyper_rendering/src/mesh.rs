/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    resource_handle::ResourceHandle,
    vulkan::{
        core::device::Device,
        descriptors::descriptor_manager::DescriptorManager,
        memory::allocator::{Allocator, MemoryLocation},
        resource::{buffer::Buffer, upload_manager::UploadManager},
    },
};

use hyper_math::vector::Vec4f;

use ash::vk;
use color_eyre::Result;
use std::{cell::RefCell, mem, rc::Rc};

// NOTE: Using Vec4f to avoid alignment issues
#[repr(C)]
#[derive(Clone, Copy, Debug)]
pub(crate) struct Vertex {
    pub(crate) position: Vec4f,
    pub(crate) normal: Vec4f,
    pub(crate) color: Vec4f,
    pub(crate) uv: Vec4f,
}

pub(crate) struct Mesh {
    index_buffer: Option<Buffer>,
    indices_count: usize,

    vertex_buffer_handle: ResourceHandle,
    _vertex_buffer: Buffer,
    vertices: Vec<Vertex>,

    descriptor_manager: Rc<RefCell<DescriptorManager>>,
}

impl Mesh {
    pub(crate) fn new(
        device: Rc<Device>,
        allocator: Rc<RefCell<Allocator>>,
        descriptor_manager: Rc<RefCell<DescriptorManager>>,
        upload_manager: Rc<RefCell<UploadManager>>,
        vertices: Vec<Vertex>,
        indices: Option<Vec<u32>>,
    ) -> Result<Self> {
        let vertex_buffer = Buffer::new(
            device.clone(),
            allocator.clone(),
            mem::size_of::<Vertex>() * vertices.len(),
            vk::BufferUsageFlags::STORAGE_BUFFER | vk::BufferUsageFlags::TRANSFER_DST,
            MemoryLocation::GpuOnly,
        )?;

        upload_manager
            .borrow_mut()
            .upload_buffer(&vertices, &vertex_buffer)?;

        let vertex_buffer_handle = descriptor_manager
            .borrow_mut()
            .allocate_buffer_handle(&vertex_buffer);

        let (index_buffer, indices_count) = if let Some(indices) = indices {
            let index_buffer = Buffer::new(
                device.clone(),
                allocator.clone(),
                mem::size_of::<u32>() * indices.len(),
                vk::BufferUsageFlags::INDEX_BUFFER | vk::BufferUsageFlags::TRANSFER_DST,
                MemoryLocation::GpuOnly,
            )?;

            upload_manager
                .borrow_mut()
                .upload_buffer(&indices, &index_buffer)?;

            (Some(index_buffer), indices.len())
        } else {
            (None, 0)
        };

        Ok(Self {
            index_buffer,
            indices_count,

            vertex_buffer_handle,
            _vertex_buffer: vertex_buffer,
            vertices,

            descriptor_manager,
        })
    }

    // TODO: Move this into asset manager and model class
    pub(crate) fn load(
        device: Rc<Device>,
        allocator: Rc<RefCell<Allocator>>,
        descriptor_manager: Rc<RefCell<DescriptorManager>>,
        upload_manager: Rc<RefCell<UploadManager>>,
        mesh_file: &str,
    ) -> Result<Self> {
        let (models, _) = tobj::load_obj(mesh_file, &tobj::GPU_LOAD_OPTIONS)?;

        let mut vertices = Vec::new();
        let mut indices = Vec::new();
        for model in &models {
            let mesh = &model.mesh;

            for i in &mesh.indices {
                let index = *i as usize;
                let position = Vec4f::new(
                    mesh.positions[3 * index],
                    mesh.positions[3 * index + 1],
                    mesh.positions[3 * index + 2],
                    1.0,
                );

                let normal = Vec4f::new(
                    mesh.normals[3 * index],
                    mesh.normals[3 * index + 1],
                    mesh.normals[3 * index + 2],
                    1.0,
                );

                let uv = Vec4f::new(
                    mesh.texcoords[2 * index],
                    1.0 - mesh.texcoords[2 * index + 1],
                    0.0,
                    0.0,
                );

                let vertex = Vertex {
                    position,
                    normal,
                    color: normal,
                    uv,
                };

                vertices.push(vertex);
                indices.push(indices.len() as u32);
            }
        }

        Self::new(
            device,
            allocator,
            descriptor_manager,
            upload_manager,
            vertices,
            Some(indices),
        )
    }

    pub(crate) fn indices_count(&self) -> usize {
        self.indices_count
    }

    pub(crate) fn index_buffer(&self) -> &Option<Buffer> {
        &self.index_buffer
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
