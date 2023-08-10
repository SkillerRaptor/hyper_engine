/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    allocator::{Allocator, MemoryLocation},
    bindings::Bindings,
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

use ash::vk;
use hyper_math::matrix::Mat4x4f;

use std::{cell::RefCell, mem, rc::Rc};

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
    pub(crate) fn new<T: Bindings>(
        device: Rc<Device>,
        allocator: Rc<RefCell<Allocator>>,
        descriptor_manager: Rc<RefCell<DescriptorManager>>,
        upload_command_pool: &CommandPool,
        upload_command_buffer: &CommandBuffer,
        upload_semaphore: &TimelineSemaphore,
        upload_value: &mut u64,
        mesh: &str,
        material: &str,
        transforms: Vec<Mat4x4f>,
        vertex_buffer_handle: ResourceHandle,
        projection_view_buffer_handle: ResourceHandle,
        extra_handles: &[ResourceHandle],
    ) -> CreationResult<Self> {
        ////////////////////////////////////////////////////////////////////////

        let transform_buffer = Buffer::new(
            device.clone(),
            allocator.clone(),
            mem::size_of::<Mat4x4f>() * transforms.len(),
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
            &transforms,
            &transform_buffer,
        )
        .map_err(|error| CreationError::RuntimeError(Box::new(error), "upload buffer"))?;

        let transform_handle = descriptor_manager
            .borrow_mut()
            .allocate_buffer_handle(&transform_buffer);

        ////////////////////////////////////////////////////////////////////////

        let bindings_buffer = Buffer::new(
            device.clone(),
            allocator.clone(),
            mem::size_of::<T>(),
            vk::BufferUsageFlags::STORAGE_BUFFER | vk::BufferUsageFlags::TRANSFER_DST,
            MemoryLocation::GpuOnly,
        )?;

        let mut resource_handles = vec![
            projection_view_buffer_handle,
            vertex_buffer_handle,
            transform_handle,
        ];
        resource_handles.extend_from_slice(extra_handles);

        Renderer::upload_buffer(
            device.clone(),
            allocator.clone(),
            upload_command_pool,
            upload_command_buffer,
            upload_semaphore,
            upload_value,
            &resource_handles,
            &bindings_buffer,
        )
        .map_err(|error| CreationError::RuntimeError(Box::new(error), "upload buffer"))?;

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
