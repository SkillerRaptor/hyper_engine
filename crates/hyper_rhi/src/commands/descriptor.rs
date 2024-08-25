//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::{collections::BTreeMap, sync::Arc};

use crate::{
    buffer::{Buffer, BufferDescriptor, BufferUsage},
    graphics_device::GraphicsDevice,
    texture::Texture,
};

#[derive(Clone, Debug)]
pub(crate) enum Resource {
    Buffer(Arc<dyn Buffer>),
    Texture(Arc<dyn Texture>),
}

#[derive(Clone, Copy, Debug)]
pub(crate) enum AccessType {
    ReadOnly,
    ReadWrite,
}

#[derive(Clone, Debug)]
pub(crate) struct Binding {
    pub(crate) resource: Resource,
    pub(crate) access_type: AccessType,
}

#[derive(Clone, Debug)]
pub struct Descriptor {
    pub(crate) buffer: Arc<dyn Buffer>,
    pub(crate) _bindings: Vec<Binding>,
}

#[derive(Default)]
pub struct DescriptorBuilder {
    bindings: BTreeMap<u32, Binding>,
}

impl DescriptorBuilder {
    pub fn read_buffer(mut self, index: u32, buffer: &Arc<dyn Buffer>) -> Self {
        assert!(!self.bindings.contains_key(&index));

        self.bindings.insert(
            index,
            Binding {
                resource: Resource::Buffer(Arc::clone(buffer)),
                access_type: AccessType::ReadOnly,
            },
        );
        self
    }

    pub fn write_buffer(mut self, index: u32, buffer: &Arc<dyn Buffer>) -> Self {
        assert!(!self.bindings.contains_key(&index));

        self.bindings.insert(
            index,
            Binding {
                resource: Resource::Buffer(Arc::clone(buffer)),
                access_type: AccessType::ReadWrite,
            },
        );
        self
    }

    pub fn read_texture(mut self, index: u32, texture: &Arc<dyn Texture>) -> Self {
        assert!(!self.bindings.contains_key(&index));

        self.bindings.insert(
            index,
            Binding {
                resource: Resource::Texture(Arc::clone(texture)),
                access_type: AccessType::ReadOnly,
            },
        );
        self
    }

    pub fn write_texture(mut self, index: u32, texture: &Arc<dyn Texture>) -> Self {
        assert!(!self.bindings.contains_key(&index));

        self.bindings.insert(
            index,
            Binding {
                resource: Resource::Texture(Arc::clone(texture)),
                access_type: AccessType::ReadWrite,
            },
        );
        self
    }

    pub fn build(self, device: &Arc<dyn GraphicsDevice>) -> Descriptor {
        let mut resource_handle_indices = Vec::new();
        let mut bindings = Vec::new();
        for (_, binding) in self.bindings {
            let resource_handle = match &binding.resource {
                Resource::Buffer(buffer) => buffer.handle(),
                Resource::Texture(texture) => texture.handle(),
            };

            resource_handle_indices.push(resource_handle.0);
            bindings.push(binding);
        }

        let buffer = device.create_buffer(&BufferDescriptor {
            data: bytemuck::cast_slice(&resource_handle_indices),
            usage: BufferUsage::STORAGE,
        });

        Descriptor {
            buffer,
            _bindings: bindings,
        }
    }
}
