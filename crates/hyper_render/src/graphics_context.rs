/*
 * Copyright (c) 2023-2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use crate::renderer::Renderer;

use hyper_math::Vec4;
use hyper_platform::window::Window;
use hyper_vulkan::{
    device::Device,
    instance::{Instance, InstanceDescriptor},
    surface::Surface,
    swapchain::Swapchain,
};

use color_eyre::eyre::Result;

pub struct GraphicsContext {
    renderer: Renderer,

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

        let renderer = Renderer::new(&device)?;

        Ok(Self {
            renderer,

            swapchain,
            device,
            surface,
            instance,
        })
    }

    pub fn begin_frame(&mut self) -> Result<()> {
        self.renderer.begin_frame(&self.swapchain)
    }

    pub fn end_frame(&self) -> Result<()> {
        self.renderer.end_frame(&self.swapchain)
    }

    pub fn submit(&mut self) -> Result<()> {
        self.renderer.submit(&self.device, &self.swapchain)
    }

    pub fn clear(&self, color: Vec4) {
        self.renderer.clear(&self.swapchain, color);
    }
}

impl Drop for GraphicsContext {
    fn drop(&mut self) {
        self.device.wait_idle().unwrap();
    }
}
