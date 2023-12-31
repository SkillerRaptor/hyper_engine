/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_platform::window::Window;
use hyper_vulkan::{
    device::Device,
    instance::{Instance, InstanceDescriptor},
    surface::Surface,
    swapchain::Swapchain,
};

use color_eyre::eyre::Result;

pub struct GraphicsContext {
    swapchain: Swapchain,
    device: Device,
    surface: Surface,
    instance: Instance,
}

impl GraphicsContext {
    pub fn new(window: &Window) -> Result<Self> {
        const DEBUG_MODE: bool = cfg!(debug_assertions);

        let instance = Instance::new(
            window,
            InstanceDescriptor {
                validation_layers: DEBUG_MODE,
            },
        )?;

        let surface = instance.create_surface(window)?;
        let device = instance.create_device(&surface)?;

        let swapchain = device.create_swapchain(window, &surface)?;

        Ok(Self {
            swapchain,
            device,
            surface,
            instance,
        })
    }
}
