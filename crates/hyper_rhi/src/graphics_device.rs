/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use hyper_platform::window::Window;

#[cfg(target_os = "windows")]
use crate::d3d12;
use crate::{
    surface::{Surface, SurfaceDescriptor},
    vulkan,
};

#[derive(Clone, Copy, Debug)]
pub enum GraphicsApi {
    D3D12,
    Vulkan,
}

#[derive(Clone, Debug)]
pub struct GraphicsDeviceDescriptor<'a> {
    pub graphics_api: GraphicsApi,
    pub debug_mode: bool,
    pub window: &'a Window,
}

#[allow(unused)] // TODO: Remove this
enum GraphicsDeviceInner {
    #[cfg(target_os = "windows")]
    D3D12(d3d12::GraphicsDevice),
    Vulkan(vulkan::GraphicsDevice),
}

pub struct GraphicsDevice {
    inner: GraphicsDeviceInner,
}

impl GraphicsDevice {
    pub(crate) const FRAME_COUNT: u32 = 2;

    pub fn new(descriptor: &GraphicsDeviceDescriptor) -> Self {
        #[allow(unreachable_patterns)]
        let (inner, backend) = match descriptor.graphics_api {
            #[cfg(target_os = "windows")]
            GraphicsApi::D3D12 => (
                GraphicsDeviceInner::D3D12(d3d12::GraphicsDevice::new(descriptor)),
                "D3D12",
            ),
            GraphicsApi::Vulkan => (
                GraphicsDeviceInner::Vulkan(vulkan::GraphicsDevice::new(descriptor)),
                "Vulkan",
            ),
            _ => unreachable!(),
        };

        log::info!("Created {} Graphics Device", backend);

        Self { inner }
    }

    pub fn create_surface(&self, descriptor: &SurfaceDescriptor) -> Surface {
        match &self.inner {
            #[cfg(target_os = "windows")]
            GraphicsDeviceInner::D3D12(inner) => {
                Surface::new_d3d12(inner.create_surface(descriptor))
            }
            GraphicsDeviceInner::Vulkan(inner) => {
                Surface::new_vulkan(inner.create_surface(descriptor))
            }
        }
    }
}
