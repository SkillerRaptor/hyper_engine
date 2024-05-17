/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use windows::Win32::Graphics::Direct3D12::ID3D12Resource;

use crate::d3d12::GraphicsDevice;

pub struct TextureView {
    // NOTE: This is an index into the Heap, may change later
    index: u32,

    graphics_device: GraphicsDevice,
}

impl TextureView {
    pub(super) fn new(graphics_device: &GraphicsDevice, _resource: &ID3D12Resource) -> Self {
        // TODO: Allocate index from device from heap
        Self {
            index: 0,

            graphics_device: graphics_device.clone(),
        }
    }
}
