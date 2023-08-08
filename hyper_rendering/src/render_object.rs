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
    renderer::Bindings,
    resource_handle::ResourceHandle,
};

use ash::vk;
use hyper_math::matrix::Mat4x4f;

use std::{
    cell::RefCell,
    mem,
    rc::Rc,
    sync::{Arc, Mutex},
};

// NOTE: Temporary
pub(crate) struct RenderObject {
    // NOTE: For now it uses Strings as identifier
    mesh: String,
    material: String,

    transforms: Vec<Mat4x4f>,

    bindings_handle: ResourceHandle,
    _bindings_buffer: Buffer,

    transform_handle: ResourceHandle,
    _transform_buffer: Buffer,

    descriptor_manager: Rc<RefCell<DescriptorManager>>,
}

impl RenderObject {
    #[allow(clippy::too_many_arguments)]
    pub(crate) fn new(
        device: Arc<Device>,
        allocator: Arc<Mutex<Allocator>>,
        descriptor_manager: Rc<RefCell<DescriptorManager>>,
        mesh: &str,
        material: &str,
        transforms: Vec<Mat4x4f>,
        vertex_buffer_handle: ResourceHandle,
        projection_view_buffer_handle: ResourceHandle,
    ) -> CreationResult<Self> {
        ////////////////////////////////////////////////////////////////////////

        let transform_buffer = Buffer::new(
            device.clone(),
            allocator.clone(),
            mem::size_of::<Mat4x4f>() * transforms.len(),
            vk::BufferUsageFlags::STORAGE_BUFFER,
        )?;

        transform_buffer.set_data(&transforms).map_err(|error| {
            CreationError::RuntimeError(Box::new(error), "set data for transform buffer")
        })?;

        let transform_handle = descriptor_manager
            .borrow_mut()
            .allocate_buffer_handle(&transform_buffer);

        ////////////////////////////////////////////////////////////////////////

        let bindings_buffer = Buffer::new(
            device.clone(),
            allocator.clone(),
            mem::size_of::<Bindings>(),
            vk::BufferUsageFlags::STORAGE_BUFFER,
        )?;

        let bindings = Bindings {
            projection_view_offset: projection_view_buffer_handle,
            vertices_offset: vertex_buffer_handle,
            transforms_offset: transform_handle,
        };
        bindings_buffer.set_data(&[bindings]).map_err(|error| {
            CreationError::RuntimeError(Box::new(error), "set data for bindings buffer")
        })?;

        let bindings_handle = descriptor_manager
            .borrow_mut()
            .allocate_buffer_handle(&bindings_buffer);

        Ok(Self {
            mesh: mesh.to_string(),
            material: material.to_string(),

            transforms,

            bindings_handle,
            _bindings_buffer: bindings_buffer,
            transform_handle,
            _transform_buffer: transform_buffer,

            descriptor_manager,
        })
    }

    pub(crate) fn mesh(&self) -> &str {
        &self.mesh
    }

    pub(crate) fn material(&self) -> &str {
        &self.material
    }

    pub(crate) fn bindings_handle(&self) -> ResourceHandle {
        self.bindings_handle
    }

    pub(crate) fn transforms(&self) -> &[Mat4x4f] {
        &self.transforms
    }
}

impl Drop for RenderObject {
    fn drop(&mut self) {
        self.descriptor_manager
            .borrow_mut()
            .retire_handle(self.bindings_handle);
        self.descriptor_manager
            .borrow_mut()
            .retire_handle(self.transform_handle);
    }
}
