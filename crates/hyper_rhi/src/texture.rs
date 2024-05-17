/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

#[cfg(target_os = "windows")]
use crate::d3d12;
use crate::{texture_view::TextureView, vulkan};

#[derive(Clone, Debug)]
pub struct TextureDescriptor {}

enum TextureInner {
    #[cfg(target_os = "windows")]
    D3D12(d3d12::Texture),
    Vulkan(vulkan::Texture),
}

pub struct Texture {
    inner: TextureInner,
}

impl Texture {
    #[cfg(target_os = "windows")]
    pub(crate) fn new_d3d12(texture: d3d12::Texture) -> Self {
        Self {
            inner: TextureInner::D3D12(texture),
        }
    }

    pub(crate) fn new_vulkan(texture: vulkan::Texture) -> Self {
        Self {
            inner: TextureInner::Vulkan(texture),
        }
    }

    pub fn view(&self) -> TextureView {
        match &self.inner {
            #[cfg(target_os = "windows")]
            TextureInner::D3D12(inner) => TextureView::new_d3d12(inner.view()),
            TextureInner::Vulkan(inner) => TextureView::new_vulkan(inner.view()),
        }
    }
}
