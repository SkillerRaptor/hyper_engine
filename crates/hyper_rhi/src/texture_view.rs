/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

#[cfg(target_os = "windows")]
use crate::d3d12;
use crate::vulkan;

enum TextureViewInner<'a> {
    #[cfg(target_os = "windows")]
    D3D12(&'a d3d12::TextureView),
    Vulkan(&'a vulkan::TextureView),
}

pub struct TextureView<'a> {
    inner: TextureViewInner<'a>,
}

impl<'a> TextureView<'a> {
    #[cfg(target_os = "windows")]
    pub(crate) fn new_d3d12(texture_view: &'a d3d12::TextureView) -> Self {
        Self {
            inner: TextureViewInner::D3D12(texture_view),
        }
    }

    pub(crate) fn new_vulkan(texture_view: &'a vulkan::TextureView) -> Self {
        Self {
            inner: TextureViewInner::Vulkan(texture_view),
        }
    }
}
