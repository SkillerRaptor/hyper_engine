//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use std::{
    fmt::{self, Debug, Formatter},
    sync::Arc,
};

use windows::Win32::Graphics::Direct3D12::{ID3D12Resource, D3D12_CPU_DESCRIPTOR_HANDLE};

use crate::{
    d3d12::graphics_device::GraphicsDeviceShared,
    resource::{Resource, ResourceHandle},
    texture::TextureDescriptor,
};

pub(crate) struct Texture {
    height: u32,
    width: u32,

    // NOTE: This is an index into the Heap, may change later
    index: u32,
    resource: ID3D12Resource,

    graphics_device: Arc<GraphicsDeviceShared>,
}

impl Texture {
    pub(crate) fn new(
        _graphics_device: &Arc<GraphicsDeviceShared>,
        _descriptor: &TextureDescriptor,
    ) -> Self {
        todo!();
    }

    pub(crate) fn new_external(
        graphics_device: &Arc<GraphicsDeviceShared>,
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
            height,
            width,

            index,
            resource,

            graphics_device: Arc::clone(graphics_device),
        }
    }

    pub(crate) fn resource(&self) -> &ID3D12Resource {
        &self.resource
    }

    pub(crate) fn index(&self) -> u32 {
        self.index
    }
}

impl Debug for Texture {
    fn fmt(&self, f: &mut Formatter<'_>) -> fmt::Result {
        f.debug_struct("Texture")
            .field("height", &self.height)
            .field("width", &self.width)
            .field("index", &self.index)
            .field("resource", &self.resource)
            .finish()
    }
}

impl crate::texture::Texture for Texture {
    fn width(&self) -> u32 {
        self.width
    }

    fn height(&self) -> u32 {
        self.height
    }
}

impl Resource for Texture {
    fn handle(&self) -> ResourceHandle {
        todo!()
    }
}
