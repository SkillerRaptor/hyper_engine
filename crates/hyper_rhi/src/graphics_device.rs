/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
*/

use hyper_platform::window::Window;

use crate::{d3d12, vulkan};

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
enum GraphicsDeviceInternal {
    D3D12(d3d12::GraphicsDevice),
    Vulkan(vulkan::GraphicsDevice),
}

pub struct GraphicsDevice {
    _internal: GraphicsDeviceInternal,
}

impl GraphicsDevice {
    pub fn new(descriptor: &GraphicsDeviceDescriptor) -> Self {
        let (internal, backend) = match descriptor.graphics_api {
            GraphicsApi::D3D12 => (
                GraphicsDeviceInternal::D3D12(d3d12::GraphicsDevice::new(descriptor)),
                "D3D12",
            ),
            GraphicsApi::Vulkan => (
                GraphicsDeviceInternal::Vulkan(vulkan::GraphicsDevice::new(descriptor)),
                "Vulkan",
            ),
        };

        log::info!("Created {} Graphics Device", backend);

        Self {
            _internal: internal,
        }
    }
}
