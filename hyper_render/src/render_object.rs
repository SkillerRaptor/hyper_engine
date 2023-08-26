/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::{
    bindings::ObjectBindings,
    resource_handle::ResourceHandle,
    vulkan::{
        core::device::Device,
        descriptors::{descriptor_manager::DescriptorManager, upload_manager::UploadManager},
        memory::allocator::{Allocator, MemoryLocation},
        resource::buffer::{Buffer, BufferCreateInfo},
    },
};

use hyper_math::matrix::Mat4x4f;

use ash::vk;
use color_eyre::Result;
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
    pub(crate) fn new(
        device: Rc<Device>,
        allocator: Rc<RefCell<Allocator>>,
        descriptor_manager: Rc<RefCell<DescriptorManager>>,
        upload_manager: Rc<RefCell<UploadManager>>,
        mesh: &str,
        material: &str,
        transforms: Vec<Mat4x4f>,
    ) -> Result<Self> {
        ////////////////////////////////////////////////////////////////////////

        let transform_buffer = Buffer::new(
            device.clone(),
            allocator.clone(),
            BufferCreateInfo {
                label: "Buffer Transform Render Object",

                usage: vk::BufferUsageFlags::STORAGE_BUFFER | vk::BufferUsageFlags::TRANSFER_DST,
                size: (mem::size_of::<Mat4x4f>() * transforms.len()) as u64,
                location: MemoryLocation::GpuOnly,
            },
        )?;

        upload_manager
            .borrow_mut()
            .upload_buffer(&transforms, &transform_buffer)?;

        let transform_handle = descriptor_manager
            .borrow_mut()
            .allocate_buffer_handle(&transform_buffer);

        ////////////////////////////////////////////////////////////////////////

        let bindings_buffer = Buffer::new(
            device,
            allocator,
            BufferCreateInfo {
                label: "Buffer Bindings Render Object",

                usage: vk::BufferUsageFlags::STORAGE_BUFFER | vk::BufferUsageFlags::TRANSFER_DST,
                size: mem::size_of::<ObjectBindings>() as u64,
                location: MemoryLocation::GpuOnly,
            },
        )?;

        // TODO: Don't hardcode
        let object_bindings = ObjectBindings {
            geometry_index: 0,
            material_index: 0,
            instance_index: 0,
            unused_0: 0,
        };

        upload_manager
            .borrow_mut()
            .upload_buffer(&[object_bindings], &bindings_buffer)?;

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
