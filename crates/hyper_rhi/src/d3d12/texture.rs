/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use std::sync::Arc;

use windows::Win32::Graphics::Direct3D12::ID3D12Resource;

use crate::{
    d3d12::{GraphicsDevice, TextureView},
    texture::TextureDescriptor,
};

struct TextureInner {
    view: TextureView,
    resource: ID3D12Resource,

    graphics_device: GraphicsDevice,
}

#[derive(Clone)]
pub(crate) struct Texture {
    inner: Arc<TextureInner>,
}

impl Texture {
    pub(crate) fn new(_graphics_device: &GraphicsDevice, _descriptor: &TextureDescriptor) -> Self {
        todo!();
    }

    pub(super) fn new_external(graphics_device: &GraphicsDevice, resource: ID3D12Resource) -> Self {
        let view = TextureView::new(graphics_device, &resource);

        Self {
            inner: Arc::new(TextureInner {
                view,
                resource,

                graphics_device: graphics_device.clone(),
            }),
        }
    }

    pub(crate) fn resource(&self) -> &ID3D12Resource {
        &self.inner.resource
    }

    pub(crate) fn view(&self) -> &TextureView {
        &self.inner.view
    }
}
