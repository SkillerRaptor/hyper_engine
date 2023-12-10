/*
 * Copyright (c) 2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_platform::window::Window;
use hyper_vulkan::{
    device::Device,
    instance::{Instance, InstanceDescriptor, ValidationLayers},
    surface::Surface,
    swapchain::{Swapchain, SwapchainDescriptor},
};

use color_eyre::Result;

#[derive(Debug)]
pub struct GraphicsContextDescriptor<'a> {
    pub application_title: &'a str,
}

impl<'a> Default for GraphicsContextDescriptor<'a> {
    fn default() -> Self {
        Self {
            application_title: "<unknown>",
        }
    }
}

pub struct GraphicsContext {
    swapchain: Option<Swapchain>,
    device: Device,
    surface: Surface,
    instance: Instance,
}

impl GraphicsContext {
    pub fn new(window: &Window, descriptor: &GraphicsContextDescriptor) -> Result<Self> {
        let debug = cfg!(debug_assertions);

        let instance = Instance::new(
            window,
            &InstanceDescriptor {
                application_title: descriptor.application_title,
                validation_layers: ValidationLayers {
                    general: debug,
                    gpu_assisted: debug,
                    synchronization: debug,
                },
            },
        )?;

        let surface = instance.create_surface(window)?;
        let device = instance.create_device(&surface)?;

        let swapchain = device.create_swapchain(
            &surface,
            &SwapchainDescriptor {
                width: window.framebuffer_size().0,
                height: window.framebuffer_size().1,
            },
        )?;

        Ok(Self {
            swapchain: Some(swapchain),
            device,
            surface,
            instance,
        })
    }

    pub fn resize(&mut self, width: u32, height: u32) -> Result<()> {
        let swapchain = self
            .device
            .create_swapchain(&self.surface, &SwapchainDescriptor { width, height })?;
        self.swapchain = Some(swapchain);

        Ok(())
    }
}
