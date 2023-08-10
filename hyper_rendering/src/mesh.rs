/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    allocator::{Allocator, MemoryLocation},
    buffer::Buffer,
    command_buffer::CommandBuffer,
    command_pool::CommandPool,
    descriptor_manager::DescriptorManager,
    device::Device,
    error::{CreationError, CreationResult},
    renderer::Renderer,
    resource_handle::ResourceHandle,
    timeline_semaphore::TimelineSemaphore,
};

use hyper_math::vector::Vec4f;

use ash::vk;
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
    #[allow(clippy::too_many_arguments)]
    pub(crate) fn new(
        device: Rc<Device>,
        allocator: Rc<RefCell<Allocator>>,
        descriptor_manager: Rc<RefCell<DescriptorManager>>,
        upload_command_pool: &CommandPool,
        upload_command_buffer: &CommandBuffer,
        upload_semaphore: &TimelineSemaphore,
        upload_value: &mut u64,
        vertices: Vec<Vertex>,
        indices: Option<Vec<u32>>,
    ) -> CreationResult<Self> {
        let vertex_buffer = Buffer::new(
            device.clone(),
            allocator.clone(),
            mem::size_of::<Vertex>() * vertices.len(),
            vk::BufferUsageFlags::STORAGE_BUFFER | vk::BufferUsageFlags::TRANSFER_DST,
            MemoryLocation::GpuOnly,
        )?;

        Renderer::upload_buffer(
            device.clone(),
            allocator.clone(),
            upload_command_pool,
            upload_command_buffer,
            upload_semaphore,
            upload_value,
            &vertices,
            &vertex_buffer,
        )
        .map_err(|error| CreationError::RuntimeError(Box::new(error), "upload buffer"))?;

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

            Renderer::upload_buffer(
                device,
                allocator,
                upload_command_pool,
                upload_command_buffer,
                upload_semaphore,
                upload_value,
                &indices,
                &index_buffer,
            )
            .map_err(|error| CreationError::RuntimeError(Box::new(error), "upload buffer"))?;

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
    #[allow(clippy::too_many_arguments)]
    pub(crate) fn load(
        device: Rc<Device>,
        allocator: Rc<RefCell<Allocator>>,
        descriptor_manager: Rc<RefCell<DescriptorManager>>,
        upload_command_pool: &CommandPool,
        upload_command_buffer: &CommandBuffer,
        upload_semaphore: &TimelineSemaphore,
        upload_value: &mut u64,
        mesh_file: &str,
    ) -> CreationResult<Self> {
        let (models, _) = tobj::load_obj(mesh_file, &tobj::GPU_LOAD_OPTIONS)
            .map_err(|error| CreationError::LoadFailure(error, mesh_file.to_string()))?;

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

        let mesh = Self::new(
            device,
            allocator,
            descriptor_manager,
            upload_command_pool,
            upload_command_buffer,
            upload_semaphore,
            upload_value,
            vertices,
            Some(indices),
        )?;
        Ok(mesh)
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
