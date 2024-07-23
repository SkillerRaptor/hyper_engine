//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::sync::Arc;

use windows::Win32::Graphics::Direct3D12::{ID3D12Resource, D3D12_CPU_DESCRIPTOR_HANDLE};

use crate::{d3d12::GraphicsDevice, texture::TextureDescriptor};

struct TextureInner {
    height: u32,
    width: u32,

    // NOTE: This is an index into the Heap, may change later
    index: u32,
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

    pub(super) fn new_external(
        graphics_device: &GraphicsDevice,
        resource: ID3D12Resource,
        width: u32,
        height: u32,
        index: u32,
    ) -> Self {
        let rtv_heap = graphics_device.rtv_heap();
        unsafe {
            graphics_device.device().CreateRenderTargetView(
                &resource,
                None,
                D3D12_CPU_DESCRIPTOR_HANDLE {
                    ptr: rtv_heap.handle().ptr + index as usize * rtv_heap.size(),
                },
            );
        }

        Self {
            inner: Arc::new(TextureInner {
                height,
                width,

                index,
                resource,

                graphics_device: graphics_device.clone(),
            }),
        }
    }

    pub(crate) fn resource(&self) -> &ID3D12Resource {
        &self.inner.resource
    }

    pub(crate) fn width(&self) -> u32 {
        self.inner.width
    }

    pub(crate) fn height(&self) -> u32 {
        self.inner.height
    }
}
