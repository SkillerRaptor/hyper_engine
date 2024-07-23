//
// Copyright (c) 2024, SkillerRaptor
//
// SPDX-License-Identifier: MIT
//

use hyper_platform::Window;

#[cfg(target_os = "windows")]
use crate::d3d12;
use crate::{texture::Texture, vulkan};

#[derive(Clone)]
pub struct SurfaceDescriptor<'a> {
    pub window: &'a Window,
}

enum SurfaceInner {
    #[cfg(target_os = "windows")]
    D3D12(d3d12::Surface),
    Vulkan(vulkan::Surface),
}

pub struct Surface {
    inner: SurfaceInner,
}

impl Surface {
    #[cfg(target_os = "windows")]
    pub(crate) fn new_d3d12(surface: d3d12::Surface) -> Self {
        Self {
            inner: SurfaceInner::D3D12(surface),
        }
    }

    pub(crate) fn new_vulkan(surface: vulkan::Surface) -> Self {
        Self {
            inner: SurfaceInner::Vulkan(surface),
        }
    }

    pub fn resize(&mut self, width: u32, height: u32) {
        match &mut self.inner {
            #[cfg(target_os = "windows")]
            SurfaceInner::D3D12(inner) => inner.resize(width, height),
            SurfaceInner::Vulkan(inner) => inner.resize(width, height),
        }
    }

    pub fn current_texture(&mut self) -> Texture {
        match &mut self.inner {
            #[cfg(target_os = "windows")]
            SurfaceInner::D3D12(inner) => Texture::new_d3d12(inner.current_texture()),
            SurfaceInner::Vulkan(inner) => Texture::new_vulkan(inner.current_texture()),
        }
    }

    pub fn present(&mut self) {
        match &mut self.inner {
            #[cfg(target_os = "windows")]
            SurfaceInner::D3D12(inner) => inner.present(),
            SurfaceInner::Vulkan(inner) => inner.present(),
        }
    }
}
