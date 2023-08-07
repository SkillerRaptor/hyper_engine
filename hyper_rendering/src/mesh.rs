/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    allocator::Allocator, buffer::Buffer, descriptor_manager::DescriptorManager, device::Device,
    error::CreationError, resource_handle::ResourceHandle,
};

use hyper_math::vector::Vec4f;

use ash::vk;
use std::{
    cell::RefCell,
    mem,
    rc::Rc,
    sync::{Arc, Mutex},
};

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
    ) -> Result<Self, CreationError> {
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

    pub(crate) fn vertices(&self) -> &[Vertex] {
        &self.vertices
    }
}

impl Drop for Mesh {
    fn drop(&mut self) {
        self.descriptor_manager
            .borrow_mut()
            .retire_handle(self.vertex_buffer_handle);
    }
}
